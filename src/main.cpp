#include <Arduino.h>
#include <SPI.h>
#include <BLEPeripheral.h>
#include <Wire.h>
#include "MMA7660.h"

// If you change these, please change their respective values in the Flutter app.
static const char * DEVICE_NAME = "Wobbly";
static const char * SERVICE_UUID = "19b10000-e8f2-537e-4f6c-d104768a1214";
static const char * CHAR_UUID = "19b10001-e8f2-537e-4f6c-d104768a1214";

long previousMillis = 0;
unsigned char buff[3];

MMA7660 accelemeter;
BLEPeripheral blePeripheral = BLEPeripheral();
BLEService bleService = BLEService(SERVICE_UUID);
BLECharacteristic bleCharacteristic = BLECharacteristic(CHAR_UUID, BLERead | BLENotify, 9);

void updateAccData() {
	int8_t x, y, unused;
	accelemeter.getXYZ(&x, &y, &unused);
  memcpy(&buff[1], &x, 1);
  memcpy(&buff[2], &y, 1);
  bleCharacteristic.BLECharacteristic::setValue(buff, 3);
}

void setup() {
  accelemeter.init();
  buff[0] = 0;
  Serial.begin(9600);
  blePeripheral.setAdvertisedServiceUuid(bleService.uuid());
  blePeripheral.addAttribute(bleService);
  blePeripheral.addAttribute(bleCharacteristic);
  blePeripheral.setLocalName(DEVICE_NAME);
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

 // as long as the central is still connected:
 while (central.connected()) {
   long currentMillis = millis();
   // if 100ms have passed, set X & Y:
   if (currentMillis - previousMillis >= 100) {
     previousMillis = currentMillis;
     updateAccData();
   }
 }

 Serial.print("Disconnected from central: ");
 Serial.println(central.address());
}
