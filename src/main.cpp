#include <SPI.h>
#include <BLEPeripheral.h>
#include <Arduino.h>

//Global var definition
const float heartRateValues[] = {111,100,80,90,70,100};
int currentIndex = 0;
unsigned long previousMillis;
const long interval = 1000;

//custom boards may override default pin definitions with BLEPeripheral(PIN_REQ, PIN_RDY, PIN_RST)
BLEPeripheral                    blePeripheral                      = BLEPeripheral();

// create remote services
BLEService                 genericAttributeService            = BLEService("180D");

// create remote characteristics
BLEFloatCharacteristic     heartbitCharacteristic           = BLEFloatCharacteristic("2A37", BLERead | BLENotify);

// create remote descriptor
BLEDescriptor              tempDescriptor                      = BLEDescriptor("290C", "bpm");

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}

void blePeripheralRemoteServicesDiscoveredHandler(BLECentral& central) {
  // central remote services discovered event handler
  Serial.print(F("Remote services discovered event, central: "));
  Serial.println(central.address());
  heartbitCharacteristic.setValue(heartRateValues[currentIndex]); 
  Serial.println(String(heartbitCharacteristic.value()));
}

void setup() {
  Serial.begin(9600);

  blePeripheral.setLocalName("HeartR8");

  // set device name and appearance
  blePeripheral.setDeviceName("HeartR8");
  blePeripheral.setAppearance(0x000D);



  // Set peripehjeral configuration
  //blePeripheral.setConnectable(false);
  blePeripheral.setAdvertisedServiceUuid("180D");
  blePeripheral.addAttribute(genericAttributeService);
  blePeripheral.addAttribute(heartbitCharacteristic);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLERemoteServicesDiscovered, blePeripheralRemoteServicesDiscoveredHandler);
  //heartbitCharacteristic.setValue(heartRateValue, sizeof(heartRateValue) / sizeof(heartRateValue[0]));
  
  // begin initialization
  blePeripheral.begin();

  heartbitCharacteristic.setValue(heartRateValues[currentIndex]);

  Serial.println(F("BLE Peripheral - HeartR8"));
}

void loop() {

  blePeripheral.poll();
  heartbitCharacteristic.setValueBE(heartRateValues[currentIndex]);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the current time
    previousMillis = currentMillis;
    Serial.print(F("Heart Rate: ")); Serial.print(heartRateValues[currentIndex]); Serial.println(F(" bpm"));
    currentIndex = (currentIndex + 1) % (sizeof(heartRateValues)/sizeof(heartRateValues[0]));
  }
}
