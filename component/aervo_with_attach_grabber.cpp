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

// Base positions
const int baseBluePos = 90;  // Left position (blue drop location)
const int baseObjectPos = 0; // Forward position (object pickup)

// Grabber positions
const int grabberOpenPos = 70;  // Open position (based on your test code)
const int grabberClosedPos = 0; // Closed position (based on your test code)

// Joint positions
const int jointPickPos = 90; // Joint position for picking objects
const int jointLiftPos = 0;  // Joint position for lifting objects

// Arm positions
const int armPickPos = 140; // Arm position for picking objects
const int armMidPos = 60;   // Arm mid position
const int armRestPos = 0;   // Arm rest position

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

// Function to move to initial position
void moveToInitialPosition()
{
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
    delay(2000);

    // 6. Move arm back to middle position
    Serial.println("Moving arm to middle position");
    moveServoGradually(armServo2, armPickPos, armMidPos, 15);
    delay(1000);

    // 7. Move joint to lifting position (0) using gradual movement
    Serial.println("Moving joint to lifting position");
    moveServoGradually(jointServo, jointPickPos, jointLiftPos, 15);
    delay(1000);

    // 8. Move arm to rest position (0)
    Serial.println("Moving arm to rest position");
    moveServoGradually(armServo2, armMidPos, armRestPos, 15);
    delay(1000);

    // 9. Move base to blue position (90)
    Serial.println("Moving base to blue position");
    moveServoGradually(baseServo, baseObjectPos, baseBluePos, 20);
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

    // 3. Move arm to picking position (140)
    Serial.println("Moving arm to release position");
    moveServoGradually(armServo2, armMidPos, armPickPos, 15);
    delay(1000);

    // 4. Open grabber to release object
    Serial.println("Opening grabber to release object");
    for (int angle = grabberClosedPos; angle <= grabberOpenPos; angle++)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(2000);

    // 5. Move arm back to middle position
    Serial.println("Moving arm to middle position");
    moveServoGradually(armServo2, armPickPos, armMidPos, 15);
    delay(1000);

    // 6. Close grabber
    Serial.println("Closing grabber");
    for (int angle = grabberOpenPos; angle >= grabberClosedPos; angle--)
    {
        grabberServo2.write(angle);
        delay(15);
    }
    delay(1000);

    // 7. Reset grabberServo1 to initial position
    Serial.println("Resetting grabber position");
    moveServoGradually(grabberServo1, 150, 90, 15);
    delay(1000);

    // 8. Move arm back to rest position
    Serial.println("Moving arm to rest position");
    moveServoGradually(armServo2, armMidPos, armRestPos, 15);
    delay(1000);

    // 9. Move base back to object position
    Serial.println("Moving base to object position");
    moveServoGradually(baseServo, baseBluePos, baseObjectPos, 20);
    delay(2000);
}

void setup()
{
    // Initialize serial communication
    Serial.begin(9600);
    Serial.println("Starting Robotic Arm Setup");

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

    Serial.println("Robotic Arm Ready!");
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