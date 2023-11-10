// Line Sensor Example
#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4LineSensors lineSensors;
Zumo32U4Encoders encoders;
Zumo32U4ButtonB buttonB;
Zumo32U4OLED display;

unsigned long lastMillis = 0;
unsigned long lastMillisBat = 0;
float motorSpeed = 0;
long lastPos = 0;
float lastPosBat = 0;
long travel = 0;
float batteryLevel = 100;
float batteryDrain = 0;
const char encoderErrorLeft[] PROGMEM = "!<c2";
const char encoderErrorRight[] PROGMEM = "!<e2";
long totCountsLeft = 0;
long totCountsRight = 0;
// Sensor values will go in this array
unsigned int lineSensorValues[5];
// int sensVal = 0;
int speed1 = 75;
// int speed2 = 150;
// int speed3 = 200;
// int mid = 2000;

void setup()
{
        Serial.begin(9600);
        buttonA.waitForButton();
        // Initialize the sensors, and turn on the LEDs for them.
        lineSensors.initFiveSensors();
        lineSensors.emittersOn();
        // Calibrates the linesensors
        //calibrate();
        //delay(1000);
}

void calibrate()
{
        for (int i = 100; i < 200; i++)
        {
                motors.setSpeeds(i, -i);
                lineSensors.calibrate();
        }

        for (int i = 200; i > 0; i--)
        {
                motors.setSpeeds(i, -i);
                lineSensors.calibrate();
        }

        for (int i = 100; i < 200; i++)
        {
                motors.setSpeeds(-i, i);
                lineSensors.calibrate();
        }

        for (int i = 200; i > 0; i--)
        {
                motors.setSpeeds(-i, i);
                lineSensors.calibrate();
        }
}

void OLED()
{
        int16_t position = lineSensors.readLine(lineSensorValues);
        static uint8_t lastDisplayTime;

        if ((uint8_t)(millis() - lastDisplayTime) >= 100)
        {
                lastDisplayTime = millis();
                display.clear();
                display.gotoXY(0, 0);
                display.print(travel);
                display.gotoXY(4, 0);
                display.print("cm");
/*              display.gotoXY(0, 1);
                display.print(motorSpeed);
                display.gotoXY(4, 1);
                display.print("cm/s");
*/
                 display.gotoXY(0, 1);
                 display.print(round(batteryLevel),1);
                 display.gotoXY(4, 1);
                display.print("%");
        }
}

void calculations()
{
        totCountsLeft += encoders.getCountsAndResetLeft();
        totCountsRight += encoders.getCountsAndResetRight();

        travel = ((totCountsLeft + totCountsRight) / 2) / 7.9 / 10;

        unsigned long currentMillis = millis();
        float currentPos = (totCountsLeft + totCountsRight) / 2;
        if (currentMillis - lastMillis >= 100)
        {
                motorSpeed = (((currentPos - lastPos) / 7.9) / (currentMillis - lastMillis)) * 100;
            if (batteryLevel > 0){   
                batteryLevel -= 0.1 + (currentPos-lastPos) / 10000 * motorSpeed;} 

                lastMillis = currentMillis;
                lastPos = currentPos;
        }
        //Serial.println(motorSpeed);
        Serial.println(batteryLevel);
}

void RUN()
{
        int16_t position = lineSensors.readLine(lineSensorValues);

        // Mid
        if (1700 < position < 2300)
        {
                motors.setSpeeds(speed1, speed1);
        }

        // Høyre

        if (position < 1700)
        {
                motors.setSpeeds(speed1 * 0.5, speed1 * 1.5);
        }

        if (position <= 1000)
        {
                motors.setSpeeds(speed1 * 0.25, speed1 * 2);
        }

        // Venstre

        if (position > 2300)
        {
                motors.setSpeeds(speed1 * 1.5, speed1 * 0.5);
        }

        if (position >= 3000)
        {
                motors.setSpeeds(speed1 * 2, speed1 * 0.25);
        }

        // Serial.println(position);
}

void battery()
{
       /* unsigned long currentMillisBat = millis();
        float currentPosBat = (totCountsLeft + totCountsRight) / 2;
        if (currentMillisBat - lastMillisBat >= 100)
        {
                batteryLevel -= 0.1;// + (abs(currentPosBat-lastPosBat) / 1000) * motorSpeed;
                // int batteryLevel -= batteryDrain;
                //  Serial.println(batteryDrain);
                lastMillisBat = currentMillisBat;
                lastPosBat = currentPosBat;

                //Serial.println(round(batteryLevel), 2);
               

        }*/
}

void loop()
{
        RUN();
        OLED();
       // battery();
        calculations();
}