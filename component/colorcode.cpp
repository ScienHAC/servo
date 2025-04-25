#include <Arduino.h>

// Color sensor pins
const int S0 = 2;
const int S1 = 4;
const int S2 = 7;
const int S3 = 8;
const int sensorOut = 12;

// Variables to store frequency readings
int redFreq = 0;
int greenFreq = 0;
int blueFreq = 0;

void setup()
{
    // Initialize serial communication
    Serial.begin(9600);
    Serial.println("TCS3200 Color Sensor Test");

    // Set color sensor pins as OUTPUT
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);

    // Set sensor OUT pin as INPUT
    pinMode(sensorOut, INPUT);

    // Set color sensor frequency scaling to 20%
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);

    Serial.println("Setup complete. Starting color readings...");
    delay(1000);
}

void loop()
{
    // Read red color
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    delay(100);
    redFreq = pulseIn(sensorOut, LOW);
    Serial.print("Red: ");
    Serial.println(redFreq);

    // Read green color
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    delay(100);
    greenFreq = pulseIn(sensorOut, LOW);
    Serial.print("Green: ");
    Serial.println(greenFreq);

    // Read blue color
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    delay(100);
    blueFreq = pulseIn(sensorOut, LOW);
    Serial.print("Blue: ");
    Serial.println(blueFreq);

    // Try to identify the color
    Serial.print("Detected color: ");
    if (redFreq < greenFreq && redFreq < blueFreq)
    {
        Serial.println("RED");
    }
    else if (greenFreq < redFreq && greenFreq < blueFreq)
    {
        Serial.println("GREEN");
    }
    else if (blueFreq < redFreq && blueFreq < greenFreq)
    {
        Serial.println("BLUE");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    // Print divider for readability
    Serial.println("---------------------");

    // Wait before next reading
    delay(1000);
}