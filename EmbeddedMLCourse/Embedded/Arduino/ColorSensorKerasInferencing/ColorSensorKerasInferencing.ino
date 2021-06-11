/*******************************************************************************
 * Simple Arduino Nano 33 BLE Sense / TensorFlow Lite Example
 * (C) 2021 by Michael Stanley
 * Last revision date: 29 May 2021
 *
 * This example reads Color data from the on-board APDS9960 sensor of the
 * Nano 33 BLE Sense and uses that to infer "banana, "orange", "GreenApple" or "RedApple"
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
#include "model_data.h"                                  // Declares generated TensorFlow Lite model data structures
#include "tensorflow/lite/micro/all_ops_resolver.h"      // Provides operations used by the TF interpreter to run models
// alternately #include "tensorflow/lite/micro/micro_mutable_ops_resolver.h"  // for memory efficient builds

#include "tensorflow/lite/micro/micro_error_reporter.h"  // Error handler
#include "tensorflow/lite/micro/micro_interpreter.h"     // model handling & running inference
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include <Wire.h>
#include <math.h>
#include <Arduino_APDS9960.h>

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

// The output layer of the TensorFlow model is 4 neurons wide, each representing a 
// probability that the particular class (corresponding to that neuron) has been detected.
// We'll need to run an argMax function to determine which of the four has the
// highest probability.
int argMax(int N, float *model_output) {
  float choice = model_output[0];
  int choice_index = 0;
  for (int i=1; i<N; i++) {
    float candidate = model_output[i];
    if (candidate>choice) {
      choice = candidate;
      choice_index = i;
     }
  }
  return(choice_index);
}

// setup_hardware() is responsible for initializing the RGB LEDs, light sensor and serial port.
TfLiteStatus setup_hardware() {
  Serial.begin(9600);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor.");
    return kTfLiteError;
  }
  // Turn on White LED light source
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);

  return kTfLiteOk;
}

//--------------------------------------------------------------------------------------
// setup_tensorflow() is responsible for initializing the TensorFlow Lite datastructures.
// Note that this version uses the simpler, but less inefficient AllOpsResolver version.
// You can use MicroMutableOpResolver instead to pull in only the operations needed for
// a specific model.  The simpler version is used here for clarities sake.
//--------------------------------------------------------------------------------------
TfLiteStatus setup_tensorflow() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  static tflite::MicroErrorReporter micro_error_reporter;  // NOLINT
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(model_data);
  // Make sure that the model generator and interpreter are consistent
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
      "Model provided is schema version %d not equal "
      "to supported version %d.",
      model->version(), TFLITE_SCHEMA_VERSION);
    return kTfLiteError;
  }

  // This pulls in all the operation implementations we need.  This is NOT the most
  // memory efficient way to do this.  Instead, MicroOpResolver can be used to pull in
  // only the functions required.
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

//--------------------------------------------------------------------------------------
// Top level initialization function to configure hardware and software drivers.
void setup() {
  setup_hardware();
  setup_tensorflow();
}

//--------------------------------------------------------------------------------------
// Here's the standard Arduino loop() construct, which does all the work.
void loop() {

    int r, g, b, c;  // These will hold the output of the RGB light sensor

    // check if a color reading is available
    while (! APDS.colorAvailable()) {
      delay(5);
    }

    // read the color (Red, Green, Blue and Clear channels)
    APDS.readColor(r, g, b, c);
    if (c !=0) {
        model_input->data.f[0] = (float) r / (float) c;
        model_input->data.f[1] = (float) g / (float) c;
        model_input->data.f[2] = (float) b / (float) c;
    	  
    		// Run inference, and report any error
    		TfLiteStatus invoke_status = interpreter->Invoke();
    		if (invoke_status != kTfLiteOk) {
      	    error_reporter->Report("Invoke failed\n");
      			return;
    		}

        // Figure out which class has the highest probability of being true
        int choice_index = argMax(4, model_output->data.f);

        // Print the resulting class assignment to the serial port output
        switch (choice_index){
        case 0:
           Serial.println("Banana");
           break;
        case 1:
           Serial.println("Orange");
           break;
        case 2: 
           Serial.println("Red Apple");
           break;
        case 3:
           Serial.println("Green Apple");
           break;
        default:
           Serial.println("UNEXPECTED CLASS ASSIGNMENT");
        }
    } else {
         Serial.println("Clear channel=0, skipping inference");
    }
}
