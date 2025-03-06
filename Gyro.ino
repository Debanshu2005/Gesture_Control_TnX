#include <RF24.h>
#include <nRF24L01.h>
#include <Wire.h>
#include <MPU6050_light.h>

// Define NRF24L01 pins
RF24 radio(9, 10);
const byte address[6] = "00001";

// Initialize MPU6050
MPU6050 mpu(Wire);
unsigned long timer = 0;

void setup() {
    Serial.begin(9600);

    // Initialize NRF24L01
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.setChannel(108);
    
    radio.stopListening(); // Set as transmitter
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.setRetries(5,15);  // 5 retries with 15 delay units (4000µs)
    // Initialize MPU6050
    Wire.begin();
    while (mpu.begin() != 0) {
        Serial.println("Could not connect to MPU6050!");
        delay(1000);
    }
    Serial.println(F("Calculating offsets, do not move MPU6050"));
    delay(1000);
    mpu.calcOffsets();
    Serial.println("Done!");
}

void loop() {
    mpu.update();

    if ((millis() - timer) > 100) { // Update every 100ms
        const char* text = nullptr;

        // Determine MPU6050 orientation and set message
        if ((mpu.getAngleY() >= -8.00 && mpu.getAngleY() < 10.00) &&
            (mpu.getAngleX() <= 10.00 && mpu.getAngleX() > -9.00)) {
            text = "STRAIGHT";
        } else if (mpu.getAngleY() < -8.00) {
            text = "LEFT";
        } else if (mpu.getAngleY() > 10.00) {
            text = "RIGHT";
        } else if (mpu.getAngleX() > 10.00) {
            text = "UP";
        } else if (mpu.getAngleX() < -10.00) {
            text = "DOWN";
        }
          
        // Send the message
        if (text) {
            bool success = radio.write(text, strlen(text) + 1); // Include null terminator
            Serial.print("Sent: ");
            Serial.print(text);
            if (success) {
                Serial.println(" - Success");
            } else {
                Serial.println(" - Failed");
            }
        }

        timer = millis(); // Reset timer
    }
}
