#include "MFRC522Uart.h"

#define MFRC522_RX_PIN 5
#define MFRC522_TX_PIN 19
#define MFRC522_RST_PIN 27

MFRC522Uart mfrc522(&Serial2, MFRC522_RX_PIN, MFRC522_TX_PIN, MFRC522_RST_PIN);

void setup()  {
    Serial.begin(115200);
    mfrc522.PCD_Init();
    delay(4);
    mfrc522.PCD_DumpVersionToSerial();
}

void loop() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) return;

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) return;
	
	byte positionID = mfrc522.uid.uidByte[0]; //UIDの最初の1バイトをPositionIDとする
	Serial.print("pID: ");
	Serial.println(positionID);

	mfrc522.PICC_HaltA(); // 卡片進入停止模式
}
