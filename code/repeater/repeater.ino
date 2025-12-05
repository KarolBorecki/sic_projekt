#include <Wire.h>
#include "wiring_private.h"

typedef struct DataFrame
{
    uint16_t measurement;
    uint16_t senderID;
    uint32_t pathMask;
    uint16_t crc;
} DataFrame_t;

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



const uint8_t SENSOR_ADDRS[] = {0x10, 0x11}; 
const int NUM_SENSORS = sizeof(SENSOR_ADDRS) / sizeof(SENSOR_ADDRS[0]);
Uart mySerial (&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM3_Handler() {
    mySerial.IrqHandler();
}

void sendStructure(byte *structurePointer, int structureLength)
{
    mySerial.write(structurePointer, structureLength);
    mySerial.flush();
    Serial.write(structurePointer, structureLength);
    Serial.flush();
}

void setup() {
    Serial.begin(9600);
    mySerial.begin(9600);
    pinPeripheral(1, PIO_SERCOM); //Assign RX function to pin 1
    pinPeripheral(0, PIO_SERCOM); //Assign TX function to pin 0
    Wire.begin();
    Wire.setClock(100000); 
}

void loop() {
    // Serial.println("checking");
    for (int i = 0; i < NUM_SENSORS; i++) {
        readFromSensor(SENSOR_ADDRS[i]);

        delay(100);
    }
    delay(500);
}

void readFromSensor(uint8_t address) {
            DataFrame_t frame;

    uint8_t bytesReceived = Wire.requestFrom((int)address, (int)sizeof(DataFrame_t));

    if (bytesReceived >= sizeof(DataFrame_t)) {
        uint8_t *pFrame = (uint8_t *)&frame;
        
        for (size_t k = 0; k < sizeof(DataFrame_t); k++) {
            pFrame[k] = Wire.read();
        }

        // Serial.print("Odebrano od ID: ");
        // Serial.println(frame.senderID);
        // Serial.print("Pomiar: ");
        // Serial.println(frame.measurement);
        // Serial.println("---");
         
    } else {
        // Serial.print("Błąd komunikacji z adresem: 0x");
        // Serial.println(address, HEX);
    }
    sendStructure((byte*)&frame, sizeof(frame));  
}