#include <Wire.h>

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

uint16_t calculateCRC(DataFrame_t *frame)
{
    uint16_t crc = 0xFFFF;
    uint8_t *data = (uint8_t *)frame;
    size_t len = sizeof(DataFrame_t) - sizeof(uint16_t);
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

void printFrame(const char *prefix, DataFrame_t *frame)
{
    Serial.print(prefix);
    Serial.print("measurement = ");
    Serial.print(frame->measurement);
    Serial.print(", senderID = ");
    Serial.print(frame->senderID);
    Serial.print(", pathMask = ");
    Serial.print(frame->pathMask, BIN);
    Serial.print(", crc = ");
    Serial.println(frame->crc);
}

const uint8_t MY_I2C_ADDRESS = 0x10;
const uint16_t MY_NODE_ID = ID_S1;

DataFrame_t txFrame;

int readColor(int s2State, int s3State)
{
    digitalWrite(S2, s2State);
    digitalWrite(S3, s3State);
    // Czekamy chwilę na przełączenie filtra
    delay(10);

    long pulse = pulseIn(SENSOR_OUT, LOW);

    int value = map(pulse, 20, 600, 255, 0);
    return constrain(value, 0, 255);
}

void prepareFrame(uint16_t measurement)
{
    int r = readColor(LOW, LOW);
    int g = readColor(HIGH, HIGH);
    int b = readColor(LOW, HIGH);

    uint16_t packedColor = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

    txFrame.measurement = measurement;
    txFrame.senderID = MY_NODE_ID;
    txFrame.pathMask = MY_NODE_ID;
    txFrame.crc = calculateCRC(&txFrame);
}

void requestEvent()
{

    Wire.write((uint8_t *)&txFrame, sizeof(DataFrame_t));
}

void setup()
{
    Serial.begin(UART_BAUD_RATE);

    Wire.begin(MY_I2C_ADDRESS);
    Wire.onRequest(requestEvent);

    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(SENSOR_OUT, INPUT);

    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);

    txFrame.senderID = MY_NODE_ID;
    txFrame.measurement = 0;
    txFrame.pathMask = 0;
    txFrame.crc = 0;
}

void loop()
{
    prepareFrame(random(0, 1000));
    printFrame("Sending frame: ", &txFrame);
    Serial.println(txFrame.crc);

    delay(500);
}
