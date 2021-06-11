/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "output_handler.h"

#include "Arduino.h"

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

void HandleOutput(float accStd, float sumMaxAudio, float *model_output) {
  // The first time this method runs, set up our LED
  static bool is_initialized = false;
  static int previousClass=-1;
  if (!is_initialized) {
    pinMode(LED_BUILTIN, OUTPUT);
    is_initialized = true;
  }
  // Toggle the LED every time an inference is performed
  static int count = 0;
  ++count;
  if (count & 1) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  float choice_index = argMax(4, model_output);
  
  if (previousClass != choice_index) {
     previousClass = choice_index;
     Serial.print(accStd, 6); Serial.print(", ");
     Serial.print(sumMaxAudio); Serial.print(", ");
     Serial.print(choice_index); Serial.print(": ");
     switch (previousClass){
      case 0:
        Serial.println("Fan OFF");
        break;
      case 1:
        Serial.println("Fan LOW");
        break;
      case 2: 
        Serial.println("Fan MEDIUM");
        break;
      case 3:
        Serial.println("Fan HIGH");
        break;
      default:
        Serial.println("UNEXPECTED CLASS ASSIGNMENT");
     }
  }
}
