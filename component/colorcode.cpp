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

// Range for valid color readings
const int MIN_VALID = 0;
const int MAX_VALID = 116;

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
    delay(150); // Slightly increased delay for stability
    redFreq = pulseIn(sensorOut, LOW);
    Serial.print("Red: ");
    Serial.println(redFreq);

    // Read green color
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    delay(150); // Slightly increased delay for stability
    greenFreq = pulseIn(sensorOut, LOW);
    Serial.print("Green: ");
    Serial.println(greenFreq);

    // Read blue color
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    delay(150); // Slightly increased delay for stability
    blueFreq = pulseIn(sensorOut, LOW);
    Serial.print("Blue: ");
    Serial.println(blueFreq);

    // Check if readings are within valid range
    bool validRed = (redFreq >= MIN_VALID && redFreq <= MAX_VALID);
    bool validGreen = (greenFreq >= MIN_VALID && greenFreq <= MAX_VALID);
    bool validBlue = (blueFreq >= MIN_VALID && blueFreq <= MAX_VALID);

    Serial.print("Valid readings - Red: ");
    Serial.print(validRed ? "Yes" : "No");
    Serial.print(", Green: ");
    Serial.print(validGreen ? "Yes" : "No");
    Serial.print(", Blue: ");
    Serial.println(validBlue ? "Yes" : "No");

    // Try to identify the color
    Serial.print("Detected color: ");

    // First check if any readings are valid
    if (!validRed && !validGreen && !validBlue)
    {
        Serial.println("UNKNOWN (all readings out of valid range)");
    }
    else
    {
        // Find the minimum frequency among valid readings
        int minFreq = 999; // Start with high value
        String minColor = "none";

        if (validRed)
        {
            minFreq = redFreq;
            minColor = "red";
        }

        if (validGreen && greenFreq < minFreq)
        {
            minFreq = greenFreq;
            minColor = "green";
        }

        if (validBlue && blueFreq < minFreq)
        {
            minFreq = blueFreq;
            minColor = "blue";
        }

        // Now check if the minimum is significantly lower than others (using 0.9 factor)
        bool redDominant = validRed && (redFreq == minFreq) &&
                           (!validGreen || redFreq < greenFreq * 0.9) &&
                           (!validBlue || redFreq < blueFreq * 0.9);

        bool greenDominant = validGreen && (greenFreq == minFreq) &&
                             (!validRed || greenFreq < redFreq * 0.9) &&
                             (!validBlue || greenFreq < blueFreq * 0.9);

        bool blueDominant = validBlue && (blueFreq == minFreq) &&
                            (!validRed || blueFreq < redFreq * 0.9) &&
                            (!validGreen || blueFreq < greenFreq * 0.9);

        // Display the result
        if (redDominant)
        {
            Serial.println("RED (within range and dominant)");
        }
        else if (greenDominant)
        {
            Serial.println("GREEN (within range and dominant)");
        }
        else if (blueDominant)
        {
            Serial.println("BLUE (within range and dominant)");
        }
        else
        {
            Serial.print("Ambiguous - lowest valid color is ");
            Serial.print(minColor);
            Serial.println(" but not clearly dominant");
        }
    }

    // Print divider for readability
    Serial.println("---------------------");

    // Wait before next reading
    delay(1000);
}