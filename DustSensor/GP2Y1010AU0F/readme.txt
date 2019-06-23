LOW=0 : HIGH=1 : INPUT=0 : OUTPUT=1

digitalWrite(LED_PIN, LOW);       // power on the LED
delayMicroseconds(samplingTime);   //0280 -> 0.28
readVoltage = analogRead(DUST_PIN); // read the dust value
delayMicroseconds(deltaTime);      //0040 -> 0.04
digitalWrite(LED_PIN, HIGH);      // turn the LED off
delayMicroseconds(sleepTime);      //9680 -> 9.68 -+-> 10ms


readVoltage :
  126 - just after cooking