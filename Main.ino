#include "USB.h"
#include "USBHIDConsumerControl.h"
#include <BLEDevice.h>

USBHIDConsumerControl ConsumerControl;
static BLEAddress targetAddress("96:07:20:0f:5f:de"); // АДРЕС 
BLEClient* pClient;

void notifyCallback(BLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify) {
    if (length > 0) {
        // Читаємо обидва байти, щоб нічого не пропустити
        uint8_t b1 = pData[0];
        uint8_t b2 = (length > 1) ? pData[1] : 0;
        
        // Визначаємо активний код (якщо перший 0, дивимось другий)
        uint8_t key = (b1 != 0) ? b1 : b2;

        // 1. ГУЧНІСТЬ
        if (key == 0x01 || key == 0xE9)         ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
        else if (key == 0x02 || key == 0xEA)    ConsumerControl.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
        // 2. ПОПЕРЕДНІЙ ТРЕК
        else if (key == 0x08)                   ConsumerControl.press(CONSUMER_CONTROL_SCAN_PREVIOUS);
        // 3. НАСТУПНИЙ ТРЕК
        else if (key == 0x04)                   ConsumerControl.press(CONSUMER_CONTROL_SCAN_NEXT);

        // 4. ЦЕНТРАЛЬНА КНОПКА (PLAY/PAUSE)
        else if (key == 0x10)                   ConsumerControl.press(CONSUMER_CONTROL_PLAY_PAUSE);

        delay(50); 
        ConsumerControl.release();
    }
}

void setup() {
    // Жодного Serial, щоб не перевантажувати USB-стек
    ConsumerControl.begin();
    USB.begin();

    BLEDevice::init("S3_Steering_Wheel");
    pClient = BLEDevice::createClient();
    
    // Спроба підключення
    if (pClient->connect(targetAddress)) {
        BLERemoteService* pSvc = pClient->getService("1812"); // HID Service
        if (pSvc) {
            // Спробуємо підписатися на характеристики
            BLERemoteCharacteristic* pChr = pSvc->getCharacteristic("2A4D");
            if (pChr) pChr->registerForNotify(notifyCallback);
        }
    }
}

void loop() {
    // Якщо зв'язок розірвався, ESP32-S3 сама перезавантажиться через 30 сек
    // або можна додати перевірку тут:
    if (!pClient->isConnected()) {
        delay(5000);
        ESP.restart();
    }
    delay(100);
}
