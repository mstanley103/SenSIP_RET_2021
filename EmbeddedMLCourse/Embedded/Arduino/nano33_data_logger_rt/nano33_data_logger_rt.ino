/*******************************************************************************
 * Simple Arduino Nano 33 BLE Sense Datalogger Application
 * (C) 2020 by Michael Stanley
 * Last revision date: 20 August 2020
 * This second generation application logs all the sensors on the Arduino
 * over the serial port at 115,200 baud to an updated copy of the
 * Integration-Nano33 Jupyter notebook.
 * This version sends samples to the PC at a 10Hz rate, which is set with a
 * hardware timer, thus it can be considered "hard real-time"
 *******************************************************************************/

// Pull in header files
#include <Arduino_LSM9DS1.h> //Include the library for 9-axis IMU
#include <Arduino_LPS22HB.h> //Include library to read Pressure 
#include <Arduino_HTS221.h> //Include library to read Temperature and Humidity 
#include <Arduino_APDS9960.h> //Include library for colour, proximity and gesture recognition
#include <PDM.h>

// Define globals
short sampleBuffer[256]; // buffer to read audio samples into, each sample is 16-bits
volatile int samplesRead=0; // number of samples read
float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;
float mag_x, mag_y, mag_z;
float Pressure;
float maxAudio=-1;
float Temperature, Humidity;
int Proximity;
bool timerTick=false;

// Define callback for the microphone.  We keep track of the maximum value
// of the microphone input over the sampling period.
void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;

  for (int i = 0; i < samplesRead; i++) { // compute the maximum audio value
    if (sampleBuffer[i]>maxAudio) maxAudio = sampleBuffer[i];
  }
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

// Here's the initial setup for the sensors.
void setup_sensors(){
  if (!IMU.begin()) //Initialize IMU sensor 
  { Serial.println("Failed to initialize IMU!"); while (1);}

  if (!BARO.begin()) //Initialize Pressure sensor 
  { Serial.println("Failed to initialize Pressure Sensor!"); while (1);}

  if (!HTS.begin()) //Initialize Temperature and Humidity sensor 
  { Serial.println("Failed to initialize Temperature and Humidity Sensor!"); while (1);}

  if (!APDS.begin()) //Initialize Colour, Proximity and Gesture sensor 
  { Serial.println("Failed to initialize Colour, Proximity and Gesture Sensor!"); while (1);}

  // configure the data receive callback
  PDM.onReceive(onPDMdata);

  // optionally set the gain, defaults to 20
  // PDM.setGain(30);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

// This is the standard Arduino setup() function, which initializes the serial port,
// hardware timer and sensors.
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LEDR, OUTPUT);
	pinMode(LEDG, OUTPUT);
	pinMode(LEDB, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);  // This LED has a different polarity then the RGB LEDs
	digitalWrite(LEDR, HIGH);
	digitalWrite(LEDG, HIGH);
	digitalWrite(LEDB, HIGH);
	//Serial.begin(9600); //Serial monitor to display all sensor values
	Serial.begin(115200); //Serial monitor to display all sensor values
	//while (!Serial.connected());  // Cannot get past here until the PC connects if uncommented

	setup_timer();
	setup_sensors();
}

// This is the function which reads sensors and streams data to the PC
void read_and_transmit_sensor_data(int cntr, bool pipelineFilled)
{
	while (!IMU.accelerationAvailable()) ;

	IMU.readAcceleration(accel_x, accel_y, accel_z);

	while (!IMU.gyroscopeAvailable());
	IMU.readGyroscope(gyro_x, gyro_y, gyro_z);

	while (!IMU.magneticFieldAvailable());
	IMU.readMagneticField(mag_x, mag_y, mag_z);

	while (!APDS.proximityAvailable());
	Proximity = APDS.readProximity();

	if (pipelineFilled) {
		Serial.print(accel_x, 6); Serial.print(", ");Serial.print(accel_y, 6);Serial.print(", ");Serial.print(accel_z, 6);Serial.print(", ");
		Serial.print(gyro_x); Serial.print(", ");Serial.print(gyro_y);Serial.print(", ");Serial.print(gyro_z);Serial.print(", ");
		Serial.print(mag_x); Serial.print(", ");Serial.print(mag_y);Serial.print(", ");Serial.print(mag_z);Serial.print(", ");
		Serial.print(Pressure);Serial.print(", ");
		Serial.print(Temperature);Serial.print(", ");
		Serial.print(Humidity);Serial.print(", ");
		Serial.print(Proximity); Serial.print(", ");
		Serial.println(maxAudio);  // This value is set in the callback for the PDM microphone
	}

	maxAudio = -1;  // Reset the maximum audio value

	if (cntr==0) {  // These three sensors are sampled at a slower rate
	  Pressure = BARO.readPressure();
	  Temperature = HTS.readTemperature();
	  Humidity = HTS.readHumidity();
	}
}

// Respond to the following commands sent over the serial port:
// 0 = OFF
// 1 = Red   LED
// 2 = Green LED
// 3 = Red AND Green = Yellow LED
// 4 = Blue  LED
// 5 = Red + Blue   = Purple LED
// 6 = Green + Blue = Cyan LED
// 7 = Red + Green + Blue = White LED
void respond_to_commands() {
	if (Serial.available() > 0) {
		char ch = Serial.read();
		if (isDigit(ch)){
			int command = ch - '0';
			if (command<8) {
				int code=~command;  // convert asci to digit and invert for use with LEDs
				digitalWrite(LEDR, (code & 1));
				digitalWrite(LEDG, (code & 2)>1);
				digitalWrite(LEDB, (code & 4)>2);
			}
		}
	}
}

// Here we have a utility function to toggle the state of D8.  We use it to confirm the sampling
// rate via an oscilloscope.
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

// Here is the callback for the hardware timer.  The MCU will be woken (if asleep) by the timer
// interrupt, and the okToLog flag will be set.  Then the main loop() calls read_and_transmit_sensor_data().
// Do not attempt to put the latter call here, as that configuration is non-functional (assumed to be
// a result of interrupt conflicts).
extern "C"
{
   void TIMER1_IRQHandler_v()
  {
    if (NRF_TIMER1->EVENTS_COMPARE[0] == 1)
    {
        timerTick=true;
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    }
  }
}

// And here is the standard Arduino loop() function to bring it all together.
int cntr=0;
bool LED = false;
bool pipelineFilled = false;
void loop() {
	if (timerTick) { // okToLog is set in the ISR of a hardware timer
		toggle_pin(); // Toggling D8 to check timing with oscilloscope
		if (cntr==10) {
			LED = !LED;
			digitalWrite(LED_BUILTIN, LED);
			cntr=0;
			pipelineFilled = true;
		}
		read_and_transmit_sensor_data(cntr, pipelineFilled);
		cntr+=1;
		timerTick=false;
	}
	respond_to_commands();
}
