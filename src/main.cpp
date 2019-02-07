#include <Arduino.h>
#include <SPI.h>
#include <BLEPeripheral.h>
#include <Wire.h>
#include "MMA7660.h"
MMA7660 accelemeter;
long previousMillis = 0;
unsigned char buff[9];
BLEPeripheral blePeripheral = BLEPeripheral();
BLEService bleService = BLEService("19b10000e8f2537e4f6cd104768a1214");
BLECharacteristic bleCharacteristic = BLECharacteristic("19b10001e8f2537e4f6cd104768a1214", BLERead | BLENotify, 9);

void updateAccData() {
  /* Read the current voltage level on the A0 analog input pin.
     This is used here to simulate the heart rate's measurement.
  */
  // int heartRateMeasurement = analogRead(A0);
  // int heartRate = map(heartRateMeasurement, 0, 1023, 0, 100);
  // if (heartRate != oldHeartRate) {      // if the heart rate has changed
  //   Serial.print("Heart Rate is now: "); // print it
  //   Serial.println(heartRate);
  //   const unsigned char heartRateCharArray[2] = { 0, (char)heartRate };
  //   heartRateChar.setValue(heartRateCharArray, 2);  // and update the heart rate measurement characteristic
  //   oldHeartRate = heartRate;           // save the level for next comparison
  // }


  // int8_t x;
	// int8_t y;
	// int8_t z;
	float ax,ay,az;
	// accelemeter.getXYZ(&x,&y,&z);

	// Serial.print("x = ");
  // Serial.println(x);
  // Serial.print("y = ");
  // Serial.println(y);
  // Serial.print("z = ");
  // Serial.println(z);

	accelemeter.getAcceleration(&ax,&ay,&az);
  memcpy(&buff[1], &ax, 4);
  memcpy(&buff[5], &ay, 4);
  bleCharacteristic.BLECharacteristic::setValue(buff, 9);
  // Serial.println("accleration of X/Y/Z: ");
	// Serial.print(buff[1]);
	// Serial.println(" g");
	// Serial.print(buff[5]);
	// Serial.println(" g");
	// Serial.print(az);
	// Serial.println(" g");
	// Serial.println("*************");
	// delay(500);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  accelemeter.init();
  buff[0] = 0;
  Serial.begin(9600);
    blePeripheral.setAdvertisedServiceUuid(bleService.uuid());
    blePeripheral.addAttribute(bleService);
    blePeripheral.addAttribute(bleCharacteristic);
    blePeripheral.setLocalName("Wobbly");
    blePeripheral.begin();
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
  }

  // check the heart rate measurement every 200ms
 // as long as the central is still connected:
 while (central.connected()) {
   long currentMillis = millis();
   // if 200ms have passed, check the heart rate measurement:
   if (currentMillis - previousMillis >= 200) {
     previousMillis = currentMillis;
     updateAccData();
   }
 }

 Serial.print("Disconnected from central: ");
 Serial.println(central.address());
}