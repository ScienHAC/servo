#include <Servo.h>
#include <Arduino.h>

// Create servo objects
Servo baseServo;     // Servo 1: Base - rotates horizontally (0=forward, 90=left, 180=toward me)
Servo armServo2;     // Servo 3: Second arm segment (0=left, 140=right for grabbing)
Servo jointServo;    // Servo 4: Joint between arm and grabber (90=for picking, 0=for lifting)
Servo grabberServo1; // Servo 5: Grabber part 1 (90=initial, 150=optimal position)
Servo grabberServo2; // Servo 6: Grabber part 2 (using 0-70 range as tested)

// Define pins
const int baseServoPin = 9;
const int armServo2Pin = 11;
const int jointServoPin = 6;
const int grabberServo1Pin = 5;
const int grabberServo2Pin = 3;

// Color sensor pins
const int S0 = 2;
const int S1 = 4;
const int S2 = 7;
const int S3 = 8;
const int sensorOut = 12;

// Base positions
const int baseBluePos = 90;  // Left position (blue drop location)
const int baseRedPos = 60;   // Middle-left position (red drop location)
const int baseGreenPos = 30; // Middle-right position (green drop location)
const int baseObjectPos = 0; // Forward position (object pickup)

// Grabber positions
const int grabberOpenPos = 70;  // Open position (based on your test code)
const int grabberClosedPos = 0; // Closed position (based on your test code)

// Joint positions
const int jointPickPos = 90; // Joint position for picking objects
const int jointLiftPos = 0;  // Joint position for lifting objects

// Arm positions
const int armPickPos = 140;    // Arm position for picking objects
const int armMidPos = 60;      // Arm mid position
const int armRestPos = 0;      // Arm rest position
const int armReleasePos = 120; // Safe release position

// Color frequency readings
int redFreq = 0;
int greenFreq = 0;
int blueFreq = 0;

// Detected color
String detectedColor = "unknown";
int targetBasePosition = baseBluePos; // Default to blue position

// Function to move servo gradually with safety checks
void moveServoGradually(Servo &servo, int startAngle, int endAngle, int delayMs)
{
    if (startAngle < endAngle)
    {
        for (int angle = startAngle; angle <= endAngle; angle++)
        {
            servo.write(angle);
            delay(delayMs);
        }
    }
    else
    {
        for (int angle = startAngle; angle >= endAngle; angle--)
        {
            servo.write(angle);
            delay(delayMs);
        }
    }
}

// Function to detect color using TCS3200 sensor
String detectColor()
{
    // Set sensor to read red color
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    delay(50);
    redFreq = pulseIn(sensorOut, LOW);
    Serial.print("Red: ");
    Serial.println(redFreq);

    // Set sensor to read green color
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    delay(50);
    greenFreq = pulseIn(sensorOut, LOW);
    Serial.print("Green: ");
    Serial.println(greenFreq);

    // Set sensor to read blue color
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    delay(50);
    blueFreq = pulseIn(sensorOut, LOW);
    Serial.print("Blue: ");
    Serial.println(blueFreq);

    // Determine color based on frequency readings
    // Note: These threshold values will need calibration with your actual sensor
    if (redFreq < greenFreq && redFreq < blueFreq && redFreq < 50)
    {
        Serial.println("Detected RED object");
        targetBasePosition = baseRedPos;
        return "red";
    }
    else if (greenFreq < redFreq && greenFreq < blueFreq && greenFreq < 50)
    {
        Serial.println("Detected GREEN object");
        targetBasePosition = baseGreenPos;
        return "green";
    }
    else if (blueFreq < redFreq && blueFreq < greenFreq && blueFreq < 50)
    {
        Serial.println("Detected BLUE object");
        targetBasePosition = baseBluePos;
        return "blue";
    }
    else
    {
        Serial.println("Unknown color");
        targetBasePosition = baseBluePos; // Default to blue position
        return "unknown";
    }
}

// Function to move to initial position
void moveToInitialPosition()
{
    // Make sure grabberServo2 is attached
    grabberServo2.attach(grabberServo2Pin);

    // Set base to object position (forward)
    baseServo.write(baseObjectPos);
    delay(1000);

    // Set arm to rest position
    armServo2.write(armRestPos);
    delay(1000);

    // Set joint to pick position (90 degrees)
    jointServo.write(jointPickPos);
    delay(1000);

    // Set grabberServo1 to initial position
    grabberServo1.write(90);
    delay(1000);

    // Close grabber
    for (int angle = grabberOpenPos; angle >= grabberClosedPos; angle--)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(1000);

    Serial.println("Initial position set");
}

// Function to pick up object
void pickUpObject()
{
    Serial.println("PICKING UP OBJECT");

    // Make sure grabberServo2 is attached
    grabberServo2.attach(grabberServo2Pin);
    Serial.println("Grabber servo attached");
    delay(500);

    // 1. Move arm to middle position (60)
    Serial.println("Moving arm to middle position");
    moveServoGradually(armServo2, armRestPos, armMidPos, 15);
    delay(1000);

    // 2. Move grabberServo1 to optimal position
    Serial.println("Adjusting grabber position");
    moveServoGradually(grabberServo1, 90, 150, 15);
    delay(1000);

    // 3. Open grabber
    Serial.println("Opening grabber");
    for (int angle = grabberClosedPos; angle <= grabberOpenPos; angle++)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(1000);

    // 4. Move arm to picking position (140)
    Serial.println("Moving arm to picking position");
    moveServoGradually(armServo2, armMidPos, armPickPos, 15);
    delay(1000);

    // 5. Close grabber to grab object
    Serial.println("Closing grabber to grab object");
    for (int angle = grabberOpenPos; angle >= grabberClosedPos; angle--)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(1000);

    // 6. Detach grabber servo to prevent overheating and servo strain while holding
    Serial.println("Detaching grabber servo to prevent overheating");
    grabberServo2.detach();
    delay(1000);

    // 7. Move arm back to middle position
    Serial.println("Moving arm to middle position");
    moveServoGradually(armServo2, armPickPos, armMidPos, 15);
    delay(1000);

    // 8. Read the color of the object using the sensor
    Serial.println("Reading object color...");
    detectedColor = detectColor();
    Serial.print("Detected color: ");
    Serial.println(detectedColor);
    delay(1000);

    // 9. Move joint to lifting position (0) using gradual movement
    Serial.println("Moving joint to lifting position");
    moveServoGradually(jointServo, jointPickPos, jointLiftPos, 15);
    delay(1000);

    // 10. Move arm to rest position (0)
    Serial.println("Moving arm to rest position");
    moveServoGradually(armServo2, armMidPos, armRestPos, 15);
    delay(1000);

    // 11. Move base to position based on detected color
    Serial.print("Moving base to ");
    Serial.print(detectedColor);
    Serial.print(" position (");
    Serial.print(targetBasePosition);
    Serial.println(" degrees)");
    moveServoGradually(baseServo, baseObjectPos, targetBasePosition, 20);
    delay(2000);
}

// Function to release object
void releaseObject()
{
    Serial.println("RELEASING OBJECT");

    // 1. Move arm to middle position (60)
    Serial.println("Moving arm to middle position");
    moveServoGradually(armServo2, armRestPos, armMidPos, 15);
    delay(1000);

    // 2. Return joint to pick position (90)
    Serial.println("Moving joint to picking position");
    moveServoGradually(jointServo, jointLiftPos, jointPickPos, 15);
    delay(1000);

    // 3. Move arm to release position (120, safer than 140)
    Serial.println("Moving arm to release position");
    moveServoGradually(armServo2, armMidPos, armReleasePos, 15);
    delay(1000);

    // 4. Reattach grabber servo
    Serial.println("Reattaching grabber servo");
    grabberServo2.attach(grabberServo2Pin);
    delay(1000);

    // 5. Open grabber to release object
    Serial.println("Opening grabber to release object");
    for (int angle = grabberClosedPos; angle <= grabberOpenPos; angle++)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(2000);

    // 6. Move arm back to middle position
    Serial.println("Moving arm to middle position");
    moveServoGradually(armServo2, armReleasePos, armMidPos, 15);
    delay(1000);

    // 7. Close grabber
    Serial.println("Closing grabber");
    for (int angle = grabberOpenPos; angle >= grabberClosedPos; angle--)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(1000);

    // 8. Reset grabberServo1 to initial position
    Serial.println("Resetting grabber position");
    moveServoGradually(grabberServo1, 150, 90, 15);
    delay(1000);

    // 9. Move arm back to rest position
    Serial.println("Moving arm to rest position");
    moveServoGradually(armServo2, armMidPos, armRestPos, 15);
    delay(1000);

    // 10. Move base back to object position
    Serial.println("Moving base to object position");
    moveServoGradually(baseServo, targetBasePosition, baseObjectPos, 20);
    delay(2000);

    // 11. Detach grabber servo until next cycle
    Serial.println("Detaching grabber servo until next cycle");
    grabberServo2.detach();
}

void setup()
{
    // Initialize serial communication
    Serial.begin(9600);
    Serial.println("Starting Robotic Arm with Color Sensor Setup");

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

    // Test the color sensor
    Serial.println("Testing color sensor...");
    detectColor();
    delay(1000);

    // Attach all servos
    Serial.println("Attaching servos...");
    baseServo.attach(baseServoPin);
    armServo2.attach(armServo2Pin);
    jointServo.attach(jointServoPin);
    grabberServo1.attach(grabberServo1Pin);
    grabberServo2.attach(grabberServo2Pin);

    // Test grabberServo2 first to verify it's working
    Serial.println("Testing grabber servo 2...");
    for (int angle = 0; angle <= 70; angle++)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(1000);

    for (int angle = 70; angle >= 0; angle--)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(1000);

    // Set initial positions
    moveToInitialPosition();

    Serial.println("Robotic Arm with Color Sensor Ready!");
    delay(2000);
}

void loop()
{
    // Complete pick and place cycle
    pickUpObject();
    delay(1000);

    releaseObject();
    delay(1000);

    Serial.println("Cycle complete - waiting before next cycle");
    delay(3000);
}