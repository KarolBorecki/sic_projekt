#include <Arduino.h>
#include "wiring_private.h"

typedef struct DataFrame
{
    uint16_t measurement;
    uint16_t senderID;
    uint32_t pathMask;
    uint16_t crc;
} DataFrame_t ;

#define UART_BAUD_RATE 9600

#define S1_NR 0
#define S2_NR 1
#define S3_NR 2
#define S4_NR 3
#define S5_NR 4
#define W1_NR 5
#define W2_NR 6
#define W3_NR 7
#define W4_NR 8
#define W0_NR 9

#define ID_S1 (1 << 0)
#define ID_S2 (1 << 1)
#define ID_S3 (1 << 2)
#define ID_S4 (1 << 3)
#define ID_S5 (1 << 4)
#define ID_W1 (1 << 5)
#define ID_W2 (1 << 6)
#define ID_W3 (1 << 7)
#define ID_W4 (1 << 8)
#define ID_W0 (1 << 9)

uint16_t calculateCRC(uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

DataFrame_t frame;
DataFrame_t frame2;
Uart mySerial (&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0);


void recieveStructure(byte *structurePointer,  int structureLength)
{
    mySerial.readBytes(structurePointer, structureLength);
}

void recieveStructure2(byte *structurePointer,  int structureLength)
{
    Serial.readBytes(structurePointer, structureLength);
}

void SERCOM3_Handler() {
    mySerial.IrqHandler();
}

void setup() {
    Serial.begin(9600);
    mySerial.begin(9600);
    pinPeripheral(1, PIO_SERCOM); //Assign RX function to pin 1
    pinPeripheral(0, PIO_SERCOM); //Assign TX function to pin 0

    frame.measurement = 0;
    frame2.measurement = 0;
}

void loop() {
    Serial.println("Checking");
    Serial.println(mySerial.available());
    Serial.println(Serial.available());
    if (mySerial.available() >= sizeof(frame)) {
        recieveStructure((byte*)&frame, sizeof(frame));
        Serial.println("Data from 1");
        Serial.println(frame.measurement);
        Serial.println(frame.senderID);
    } 

    if (Serial.available() >= sizeof(frame2)) {
        recieveStructure2((byte*)&frame2, sizeof(frame2));
        Serial.println("Data from 2");
        Serial.println(frame2.measurement);
        Serial.println(frame2.senderID);
    } 
    delay(100);
}