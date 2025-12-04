#include <Wire.h>
#include "wiring_private.h" // Potrzebne dla pinPeripheral

typedef struct DataFrame
{
    uint16_t measurement;
    uint16_t senderID;
    uint32_t pathMask;
    uint16_t crc;
} DataFrame_t;

void recieveStructure(byte *structurePointer, int structureLength)
{
    mySerial.readBytes(structurePointer, structureLength);
}

#define UART_BAUD_RATE 9600

DataFrame_t frame;
uint8_t buffer[128];

// RX na Pin 1 (PA23, PAD[1]), TX na Pin 0 (PA22, PAD[0])
Uart mySerial (&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0);

void SERCOM3_Handler()
{
    mySerial.IrqHandler();
}

void setup() {
    Serial.begin(UART_BAUD_RATE);
    
    mySerial.begin(UART_BAUD_RATE); 

    pinPeripheral(1, PIO_SERCOM); // RX
    pinPeripheral(0, PIO_SERCOM); // TX
    
    Serial.println("Start odbiornika UART...");
}

void loop() {
    if (mySerial.available() >= sizeof(DataFrame_t)) {
        Serial.println("Odbieram ramkę...");
        
        mySerial.readBytes(buffer, sizeof(DataFrame_t));

        memcpy(&frame, buffer, sizeof(DataFrame_t));

        Serial.print("Otrzymano ID: ");
        Serial.println(frame.senderID);
        Serial.print("Pomiar: ");
        Serial.println(frame.measurement);
        Serial.print("CRC: ");
        Serial.println(frame.crc, HEX);
        Serial.println("---");
    }
    Serial.println(mySerial.available() );
    delay(500);
}