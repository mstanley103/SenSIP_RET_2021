/*
  APDS9960 - Color Sensor

  This example reads Color data from the on-board APDS9960 sensor of the
  Nano 33 BLE Sense and prints the color RGB (red, green, blue) values
  to the Serial Monitor once a second.

  The circuit:
  - Arduino Nano 33 BLE Sense

  This example code is in the public domain.
*/
#include <Wire.h>
#include <math.h>
#include <Arduino_APDS9960.h>

int predict(float features[3]) {

    int classes[4];
        
    if (features[1] <= 0.30323268473148346) {
        if (features[1] <= 0.2431386336684227) {
            if (features[2] <= 0.20256410539150238) {
                if (features[1] <= 0.17380952835083008) {
                    classes[0] = 0; 
                    classes[1] = 0; 
                    classes[2] = 1; 
                    classes[3] = 0; 
                } else {
                    classes[0] = 0; 
                    classes[1] = 3; 
                    classes[2] = 0; 
                    classes[3] = 0; 
                }
            } else {
                if (features[1] <= 0.23446089029312134) {
                    classes[0] = 0; 
                    classes[1] = 0; 
                    classes[2] = 63; 
                    classes[3] = 0; 
                } else {
                    if (features[1] <= 0.23774703592061996) {
                        classes[0] = 0; 
                        classes[1] = 1; 
                        classes[2] = 0; 
                        classes[3] = 0; 
                    } else {
                        classes[0] = 0; 
                        classes[1] = 0; 
                        classes[2] = 3; 
                        classes[3] = 0; 
                    }
                }
            }
        } else {
            if (features[2] <= 0.24795082211494446) {
                if (features[1] <= 0.2936105579137802) {
                    classes[0] = 0; 
                    classes[1] = 59; 
                    classes[2] = 0; 
                    classes[3] = 0; 
                } else {
                    if (features[0] <= 0.5655637383460999) {
                        if (features[0] <= 0.5542124509811401) {
                            if (features[0] <= 0.5350223481655121) {
                                classes[0] = 0; 
                                classes[1] = 1; 
                                classes[2] = 0; 
                                classes[3] = 0; 
                            } else {
                                if (features[1] <= 0.30003558099269867) {
                                    classes[0] = 2; 
                                    classes[1] = 0; 
                                    classes[2] = 0; 
                                    classes[3] = 0; 
                                } else {
                                    classes[0] = 0; 
                                    classes[1] = 1; 
                                    classes[2] = 0; 
                                    classes[3] = 0; 
                                }
                            }
                        } else {
                            classes[0] = 0; 
                            classes[1] = 3; 
                            classes[2] = 0; 
                            classes[3] = 0; 
                        }
                    } else {
                        classes[0] = 1; 
                        classes[1] = 0; 
                        classes[2] = 0; 
                        classes[3] = 0; 
                    }
                }
            } else {
                if (features[1] <= 0.297852486371994) {
                    classes[0] = 0; 
                    classes[1] = 0; 
                    classes[2] = 6; 
                    classes[3] = 0; 
                } else {
                    classes[0] = 0; 
                    classes[1] = 1; 
                    classes[2] = 0; 
                    classes[3] = 0; 
                }
            }
        }
    } else {
        if (features[1] <= 0.35032467544078827) {
            if (features[2] <= 0.3045814484357834) {
                if (features[2] <= 0.1909814327955246) {
                    if (features[2] <= 0.18804792314767838) {
                        classes[0] = 2; 
                        classes[1] = 0; 
                        classes[2] = 0; 
                        classes[3] = 0; 
                    } else {
                        classes[0] = 0; 
                        classes[1] = 2; 
                        classes[2] = 0; 
                        classes[3] = 0; 
                    }
                } else {
                    if (features[0] <= 0.470058798789978) {
                        if (features[0] <= 0.4554065465927124) {
                            classes[0] = 10; 
                            classes[1] = 0; 
                            classes[2] = 0; 
                            classes[3] = 0; 
                        } else {
                            if (features[1] <= 0.32842105627059937) {
                                classes[0] = 1; 
                                classes[1] = 0; 
                                classes[2] = 0; 
                                classes[3] = 0; 
                            } else {
                                if (features[2] <= 0.2400471493601799) {
                                    classes[0] = 1; 
                                    classes[1] = 0; 
                                    classes[2] = 0; 
                                    classes[3] = 0; 
                                } else {
                                    classes[0] = 0; 
                                    classes[1] = 0; 
                                    classes[2] = 0; 
                                    classes[3] = 3; 
                                }
                            }
                        }
                    } else {
                        if (features[0] <= 0.5572599470615387) {
                            classes[0] = 49; 
                            classes[1] = 0; 
                            classes[2] = 0; 
                            classes[3] = 0; 
                        } else {
                            if (features[1] <= 0.3101494610309601) {
                                classes[0] = 2; 
                                classes[1] = 0; 
                                classes[2] = 0; 
                                classes[3] = 0; 
                            } else {
                                classes[0] = 0; 
                                classes[1] = 1; 
                                classes[2] = 0; 
                                classes[3] = 0; 
                            }
                        }
                    }
                }
            } else {
                classes[0] = 0; 
                classes[1] = 0; 
                classes[2] = 0; 
                classes[3] = 3; 
            }
        } else {
            if (features[0] <= 0.4944310039281845) {
                if (features[2] <= 0.30628518760204315) {
                    if (features[1] <= 0.3550420254468918) {
                        if (features[1] <= 0.3521462678909302) {
                            classes[0] = 0; 
                            classes[1] = 0; 
                            classes[2] = 0; 
                            classes[3] = 1; 
                        } else {
                            classes[0] = 1; 
                            classes[1] = 0; 
                            classes[2] = 0; 
                            classes[3] = 0; 
                        }
                    } else {
                        classes[0] = 0; 
                        classes[1] = 0; 
                        classes[2] = 0; 
                        classes[3] = 72; 
                    }
                } else {
                    if (features[0] <= 0.4209381490945816) {
                        classes[0] = 0; 
                        classes[1] = 0; 
                        classes[2] = 0; 
                        classes[3] = 4; 
                    } else {
                        classes[0] = 3; 
                        classes[1] = 0; 
                        classes[2] = 0; 
                        classes[3] = 0; 
                    }
                }
            } else {
                if (features[0] <= 0.5298701524734497) {
                    classes[0] = 1; 
                    classes[1] = 0; 
                    classes[2] = 0; 
                    classes[3] = 0; 
                } else {
                    classes[0] = 0; 
                    classes[1] = 0; 
                    classes[2] = 1; 
                    classes[3] = 0; 
                }
            }
        }
    }

    int index = 0;
    for (int i = 0; i < 4; i++) {
        index = classes[i] > classes[index] ? i : index;
    }
    return index;
}


void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor.");
  }
  // Turn on White LED light source
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);

}

void loop() {
  int r, g, b, c;
  float features[3];
  int result;

  // check if a color reading is available
  while (! APDS.colorAvailable()) {
    delay(5);
  }

  // read the color
  APDS.readColor(r, g, b, c);
  features[0]=(float) r / (float) c;
  features[1]=(float) g / (float) c;
  features[2]=(float) b / (float) c;
  result = predict(features);

  switch (result) {
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
      Serial.println("NOT RECOGNIZED");
  }

  // wait a bit before reading again
  delay(500);
}
