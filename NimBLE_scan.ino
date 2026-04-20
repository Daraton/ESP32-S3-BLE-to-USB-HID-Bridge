#include <NimBLEDevice.h>

class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        Serial.printf("Знайдено пристрій: %s | RSSI: %d | Адреса: %s\n", 
                      advertisedDevice->getName().c_str(), 
                      (int)advertisedDevice->getRSSI(),
                      advertisedDevice->getAddress().toString().c_str());
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Запуск сканера...");
  
  NimBLEDevice::init("");
  NimBLEScan* pScan = NimBLEDevice::getScan();
  pScan->setScanCallbacks(new MyScanCallbacks(), false);
  pScan->setActiveScan(true);
  pScan->start(0, false); 
}

void loop() {
  delay(1000);
}
