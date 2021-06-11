/*******************************************************************************
 * Simple Arduino Nano 33 BLE Sense / TensorFlow Lite Example
 * (C) 2020 by Michael Stanley
 * Last revision date: 4 July 2020
 *
 * This example pulls in sensor data from the Nano 33's accelerometer and
 * microphone.  It computes one feature from each data stream and feeds these
 * to a TensorFlow Lite model that predicts the speed of a simple 4-speed
 * table top fan from the features.
 *
 * Note that this isn't necessarily a GOOD model, but it illustrates a number
 * of integration issues.
 *
 * Portions of this application were pulled from the Tensorflow Lite for MCUs
 * Hello World application, which comes with the following license:
 *
 * Copyright 2019 The TensorFlow Authors. All Rights Reserved.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  ==============================================================================*/

#include <TensorFlowLite.h>                              // Standard TensorFlow Lite header
//#include "main_functions.h"                              // Arduino style function declarations
#include "fandemo_model_data.h"                          // Declares generated TensorFlow Lite model data structures
#include "output_handler.h"                              // Declares functions for serial output
#include "tensorflow/lite/micro/all_ops_resolver.h"      // Standard TensorFlow Lite for MCUs headers
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include <Arduino_LSM9DS1.h>                             // Include the Arduino library for 9-axis IMU
#include <PDM.h>                                         // Include the Arduino library for the microphone

short sampleBuffer[256];      // buffer to read audio samples into, each sample is 16-bits
volatile int samplesRead = 0; // number of samples read
float maxAudio = -1;          // The maximum audio level detected in the previous audio sampling interval
bool onTick;                  // Set to true at 0.1 second intervals by a hardware timer

// Globals, used for compatibility with Arduino-style sketches.
namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;
  TfLiteTensor* model_output = nullptr;
  int input_length;
  int output_length;
  
  // Create an area of memory to use for input, output, and intermediate arrays.
  // The size of this will depend on the model you're using, and may need to be
  // determined by experimentation.
  constexpr int kTensorArenaSize = 60 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
}
//--------------------------------------------------------------------------------------
// This is the callback function that is invoked to service the microphone.
void onPDMdata() {

  int bytesAvailable = PDM.available(); // query the number of bytes available

  PDM.read(sampleBuffer, bytesAvailable); // read into the sample buffer
  samplesRead = bytesAvailable / 2; // 16-bit, 2 bytes per sample

  for (int i = 0; i < samplesRead; i++) { // compute the maximum audio value
    if (sampleBuffer[i] > maxAudio) maxAudio = sampleBuffer[i];
  }
}

//--------------------------------------------------------------------------------------
// setup_sensors() is responsible for initializing the IMU and microphone.
TfLiteStatus setup_sensors() {

  if (!IMU.begin()) //Initialize IMU sensor
  {
    Serial.println("Failed to initialize IMU!");
    return kTfLiteError;
  }

  PDM.onReceive(onPDMdata); // configure the data receive callback

  // optionally set the gain, defaults to 20
  // PDM.setGain(30);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    return kTfLiteError;
  }
  return kTfLiteOk;
}

//--------------------------------------------------------------------------------------
// setup_tensorflow() is responsible for initializing the TensorFlow Lite datastructures.
// Note that this version uses the simpler, but less inefficient AllOpsResolver version.
// You can use MicroMutableOpResolver instead to pull in only the operations needed for
// a specific model.  The simpler version is used here for clarities sake.
TfLiteStatus setup_tensorflow() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  static tflite::MicroErrorReporter micro_error_reporter;  // NOLINT
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(fandemo_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
      "Model provided is schema version %d not equal "
      "to supported version %d.",
      model->version(), TFLITE_SCHEMA_VERSION);
    return kTfLiteError;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor
  model_input = interpreter->input(0);
  model_output = interpreter->output(0);

  input_length = model_input->bytes / sizeof(float);
  output_length = model_output->bytes / sizeof(float);
  return kTfLiteOk;
}

// We are using TIMER1 to set the 10Hz rate.  This has been confirmed via scope probe
// on the D8 output pin of the Arduino.
void setup_timer() {
    NRF_TIMER1->BITMODE =  0; // 16-bit mode
    NRF_TIMER1->PRESCALER = 8;
    // "The TIMER base frequency is always given as 16 MHz divided by the prescaler value. = 2MHz"
    NRF_TIMER1->CC[0] = 6250;  // This should give us 1ms between clocks
    NRF_TIMER1->INTENSET = 0x00010000; // Interrupt Enable A
    NRF_TIMER1->SHORTS = 1; // Shortcut between event COMPARE[0] and CLEAR

    NVIC_EnableIRQ(TIMER1_IRQn);
    NRF_TIMER1->TASKS_START = 1;
}

//--------------------------------------------------------------------------------------
// Top level initialization function to configure hardware and software drivers.
void setup() {
  TfLiteStatus sts;
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600); //Serial monitor to display all sensor values
  //Serial.begin(115200); //Serial monitor to display all sensor values
  //while (!Serial.connected());  // Cannot get past here until the PC connects if uncommented

  setup_sensors();
  setup_timer();
  setup_tensorflow();

  onTick=false;
}

// Here we have a utility function to toggle the state of the D8 pin on the Arduino Nano 33 BLE Sense board.
// We use it to confirm the sampling rate via an oscilloscope.
volatile int flag = 0;
int dpin = 8; // D8
void toggle_pin() {
    if (flag) {
      flag=0;
      digitalWrite(dpin, LOW);   // switch input low
    } else {
      flag=1;
      digitalWrite(dpin, HIGH);   // switch input high
    }
}

//--------------------------------------------------------------------------------------
// get_features is a hand-crafted function that replicates two of the feature computations
// done in our Jupyter notebooks and later used as inputs to our TensorFlow Lite model.
// In this case, we compute the sample standard deviation of the accelerometer vector
// magnitude as well as the sum (over the 0.1 second interval) of the maximum value
// computed in each call of the microphone callback.  See the course materials for a
// discussion of issues one might encounter with a simple model like this.
bool get_features(float& accStd, float& sumMaxAudio)
{
	const int N = 10;  // This function works on a rolling window of 'N' samples
	static float accelBuffer[N];
	static float maxAudioBuffer[N];
	static int buffer_index = 0;
	static bool full = false;
	float accX, accY, accZ;
	float meanAcc;

	while (!IMU.accelerationAvailable()) ;
	IMU.readAcceleration(accX, accY, accZ);

	accelBuffer[buffer_index] = sqrt(accX * accX + accY * accY + accZ * accZ);
	maxAudioBuffer[buffer_index] = maxAudio;
	maxAudio = -1;  // Reset the maximum audio value

	buffer_index++;
	if (buffer_index >= N) {
		full = true;
		buffer_index = 0;
	}
	if (full) {  // Compute the features
		sumMaxAudio = 0;
		meanAcc = 0;
		accStd = 0;
		for (int i=0; i<N; i++) {
			sumMaxAudio += maxAudioBuffer[i];
			meanAcc += accelBuffer[i];
		}
		meanAcc /= N;
		for (int j = 0; j < N; j++) {
			float temp = accelBuffer[j] - meanAcc;
			accStd += temp * temp;
		}
		accStd = accStd/(N-1);
		accStd = sqrt(accStd);  // In Excel, this is STDEV.S (Sample Standard Deviation)
	}
	return(full); // accStd and sumMaxAudio will be valid when full=true upon return
}

// Here is the callback for the hardware timer.  The callback simply sets a flog that
// tells the main loop() to compute features and call the TensorFlow Lite model.
// Do not attempt to put the latter call heres, as that configuration is non-functional (assumed to be
// a result of interrupt conflicts).
extern "C"
{
   void TIMER1_IRQHandler_v()
  {
    if (NRF_TIMER1->EVENTS_COMPARE[0] == 1)
    {
        onTick=true;
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    }
  }
}

// We usually normalize the feature inputs to any machine learning algorithm.
// This function normalizes the data stream using values computed by our
// Jupyter notebook and embedded in the C++ code as const definitions for
// "means" and "stds".
void normalize(float& sumMaxAudio, float& accStd) {
	sumMaxAudio = (sumMaxAudio - means[0]) / stds[0];
	accStd      = (accStd      - means[1]) / stds[1];
}
//--------------------------------------------------------------------------------------
// Here's the standard Arduino loop()  construct, which does all the work.
void loop() {

  static float accStd=0, sumMaxAudio=0;
  static long loopNumber = 0;
  bool valid;
  if (onTick) {
	  toggle_pin(); // Toggling D8 to check timing with oscilloscope
	  onTick=false;
	  loopNumber++;
	  valid = get_features(accStd, sumMaxAudio); // accStd and sumMaxAudio will be valid when loop_index==0 upon return
	  if (valid) {
		  // get_features() includes internal buffers for the feature streams.  One item is added to each buffer for each call.
		  // Once the buffers are full, get_features returns valid=true and we start passing the computed features
		  // on to the TensorFlow Lite model.
		  normalize(sumMaxAudio, accStd);          // This step is usually missing from most ML examples, but is required
		                                           // for sensor-based applications/
		  model_input->data.f[0] = sumMaxAudio;
		  model_input->data.f[1] = accStd;

		  // Run inference, and report any error
		  TfLiteStatus invoke_status = interpreter->Invoke();
		  if (invoke_status != kTfLiteOk) {
			error_reporter->Report("Invoke failed on index: %d\n", loopNumber);
			return;
		  }

		  HandleOutput(accStd, sumMaxAudio, model_output->data.f);
	  }
  }
}
