/* 車載統合ソフトウェア */
/* メインの処理を行う  */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "src/PositionID_Detector.h"

PositionID_Detector positionID_Detector;

static const char SERVICE_UUID[] = "63cb613b-6562-4aa5-b602-030f103834a4";
static const char CHARACTERISTIC_MOTORINPUT_UUID[] = "88c9d9ae-bd53-4ab3-9f42-b3547575a743";
static const char CHARACTERISTIC_POSITIONID_UUID[] = "8bcd68d5-78ca-c1c3-d1ba-96d527ce8968";

BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pCharacteristicMotorInput = NULL;
BLECharacteristic *pCharacteristicPositionId = NULL;
BLEAdvertising *pAdvertising = NULL;

std::string getTrainName() {
  uint64_t chipId = ESP.getEfuseMac();
  switch (chipId) {
    case 0x702e93bd9e7c:
      return "E5";
    case 0x9867e3ab6224:
      return "E6";
    case 0xdceacf1f9c9c:
      return "Dr.";
    default:
      return "unknown";
  }
}

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Connected");
  }

  void onDisconnect(BLEServer *pServer) {
    Serial.println("Disconnected");
    pServer->startAdvertising();
    ledcWrite(0, 0); //モータを停止
  }
};

class CharacteristicMotorInputCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristicMotorInput) {
    std::string value = pCharacteristicMotorInput->getValue();

    if (value.length() > 0) {
      Serial.print("motorInput: ");
      int motorInput = std::stoi(value);
      Serial.print(motorInput);
      ledcWrite(0, motorInput);
      Serial.println();
    }
  }
};

void setup() {

    /* Serial */
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Starting BLE");

    Serial.print("Chip ID: ");
    Serial.println(ESP.getEfuseMac());

    BLEDevice::init("ESPlarail (" + getTrainName() + ")");

    Serial.print("Address: ");
    Serial.println(BLEDevice::getAddress().toString().c_str());

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    pService = pServer->createService(SERVICE_UUID);

    pCharacteristicMotorInput = pService->createCharacteristic(
        CHARACTERISTIC_MOTORINPUT_UUID,
        BLECharacteristic::PROPERTY_READ | 
        BLECharacteristic::PROPERTY_WRITE | 
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );
    pCharacteristicMotorInput->setCallbacks(new CharacteristicMotorInputCallbacks());
    pCharacteristicMotorInput->setValue("Initial value");
    pCharacteristicMotorInput->addDescriptor(new BLE2902());

    pCharacteristicPositionId = pService->createCharacteristic(
        CHARACTERISTIC_POSITIONID_UUID,
        BLECharacteristic::PROPERTY_READ | 
        BLECharacteristic::PROPERTY_WRITE | 
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
    );
    pCharacteristicPositionId->setValue("Initial value");
    pCharacteristicPositionId->addDescriptor(new BLE2902());

    pService->start();

    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();
    Serial.println("Advertising started");

    /* ledcセットアップ */
    ledcSetup(0, 20000, 8);
    ledcAttachPin(25, 0);
    ledcWrite(0, 0);
    Serial.println("LEDC Setup done!!");

    /* MFRC522セットアップ */
    positionID_Detector.MFRC522Setup();
    Serial.println("MFRC522 Setup done!!");

}

void loop(){

    /* 積算位置検知(IPS) */
    //TODO: IPSのコードを書く
    // 左右のエンコーダから入力が来たらcharacteristec_ips(value: 'o')をnotifyする

    /* 絶対位置検知(APS) */
    int     positionID      = positionID_Detector.getPositionID();
    if (positionID > 0) {
        pCharacteristicPositionId->setValue((uint8_t*)&positionID, 1);
        pCharacteristicPositionId->notify();
        Serial.println("Notified PositionId");
    }
}