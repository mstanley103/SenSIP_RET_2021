// Starting point for this code was https://circuitdigest.com/microcontroller-projects/arduino-nano-33-ble-sense-board-review-and-getting-started-guide and the Arduino PDMSerialPlotter example
#include <Arduino_LSM9DS1.h> //Include the library for 9-axis IMU
#include <Arduino_LPS22HB.h> //Include library to read Pressure 
#include <Arduino_HTS221.h> //Include library to read Temperature and Humidity 
#include <Arduino_APDS9960.h> //Include library for colour, proximity and gesture recognition
#include <PDM.h>

short sampleBuffer[256]; // buffer to read audio samples into, each sample is 16-bits
volatile int samplesRead=0; // number of samples read
float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;
float mag_x, mag_y, mag_z;
float Pressure;
float maxAudio=-1;
float Temperature, Humidity;
int Proximity;
bool okToLog;

void setup(){
  Serial.begin(9600); //Serial monitor to display all sensor values 
  while (!Serial);
  
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

void loop()
{
  okToLog = true;
  while (!IMU.accelerationAvailable()) ;
  IMU.readAcceleration(accel_x, accel_y, accel_z);

  while (!IMU.gyroscopeAvailable());
  IMU.readGyroscope(gyro_x, gyro_y, gyro_z);

  while (!IMU.magneticFieldAvailable());
  IMU.readMagneticField(mag_x, mag_y, mag_z);

  Pressure = BARO.readPressure();
  Temperature = HTS.readTemperature();
  Humidity = HTS.readHumidity();

  while (!APDS.proximityAvailable());
  Proximity = APDS.readProximity();

  Serial.print(accel_x); Serial.print(", ");Serial.print(accel_y);Serial.print(", ");Serial.print(accel_z);Serial.print(", ");
  Serial.print(gyro_x); Serial.print(", ");Serial.print(gyro_y);Serial.print(", ");Serial.print(gyro_z);Serial.print(", ");
  Serial.print(mag_x); Serial.print(", ");Serial.print(mag_y);Serial.print(", ");Serial.print(mag_z);Serial.print(", ");
  Serial.print(Pressure);Serial.print(", ");
  Serial.print(Temperature);Serial.print(", ");
  Serial.print(Humidity);Serial.print(", ");
  Serial.print(Proximity); Serial.print(", ");
  Serial.println(maxAudio);  // This value is set in the callback for the PDM microphone
  maxAudio = -1;  // Reset the maximum audio value 

  //delay(200);
}


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
