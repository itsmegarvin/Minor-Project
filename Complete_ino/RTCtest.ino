#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
#include <Servo.h> // Include the Servo library

#define IR_SENSOR_PIN_1 2
#define IR_SENSOR_PIN_2 3
#define IR_SENSOR_PIN_3 4
#define IR_SENSOR_EXIT 5 // Define the pin for the IR sensor at the exit
#define SERVO_PIN 6 // Define the pin for the servo motor

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial BTSerial(10, 11);
Servo barrierServo; // Create a servo object to control the barrier

DateTime entryTime1, entryTime2, entryTime3;
DateTime exitTime1, exitTime2, exitTime3;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  pinMode(IR_SENSOR_PIN_1, INPUT);
  pinMode(IR_SENSOR_PIN_2, INPUT);
  pinMode(IR_SENSOR_PIN_3, INPUT);
  pinMode(IR_SENSOR_EXIT, INPUT_PULLUP); // Set the exit IR sensor pin as INPUT_PULLUP
  barrierServo.attach(SERVO_PIN); // Attach the servo to the designated pin

  closeBarrier(); // Ensure barrier is closed initially

  if (!rtc.begin()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("RTC is running.");
  }
}

bool entryRecorded1 = false;
bool entryRecorded2 = false;
bool entryRecorded3 = false;
bool exitRecorded1 = false;
bool exitRecorded2 = false;
bool exitRecorded3 = false;
bool durationShown1 = false;
bool durationShown2 = false;
bool durationShown3 = false;

void loop() {
  int a, b, c;
  
  int sensorValue1 = digitalRead(IR_SENSOR_PIN_1);
  int sensorValue2 = digitalRead(IR_SENSOR_PIN_2);
  int sensorValue3 = digitalRead(IR_SENSOR_PIN_3);

  // Record entry time when car enters the slot
  if (sensorValue1 == LOW && !entryRecorded1) {
    entryTime1 = rtc.now();
    entryRecorded1 = true;
    Serial.print("Entry time slot 1: ");
    printDateTime(entryTime1);
  }
  if (sensorValue2 == LOW && !entryRecorded2) {
    entryTime2 = rtc.now();
    entryRecorded2 = true;
    Serial.print("Entry time slot 2: ");
    printDateTime(entryTime2);
  }
  if (sensorValue3 == LOW && !entryRecorded3) {
    entryTime3 = rtc.now();
    entryRecorded3 = true;
    Serial.print("Entry time slot 3: ");
    printDateTime(entryTime3);
  }

  // Record exit time when car exits the slot
  if (sensorValue1 == HIGH && entryRecorded1 && !exitRecorded1) {
    exitTime1 = rtc.now();
    exitRecorded1 = true;
    Serial.print("Exit time slot 1: ");
    printDateTime(exitTime1);
  }
  if (sensorValue2 == HIGH && entryRecorded2 && !exitRecorded2) {
    exitTime2 = rtc.now();
    exitRecorded2 = true;
    Serial.print("Exit time slot 2: ");
    printDateTime(exitTime2);
  }
  if (sensorValue3 == HIGH && entryRecorded3 && !exitRecorded3) {
    exitTime3 = rtc.now();
    exitRecorded3 = true;
    Serial.print("Exit time slot 3: ");
    printDateTime(exitTime3);
  }

  // Calculate and display total duration and cost for each slot
  if (exitRecorded1 && !durationShown1) {
    TimeSpan duration1 = exitTime1 - entryTime1;
    Serial.print("Total duration slot1: ");
    printTimeSpan(duration1);
    Serial.print("Cost for slot1: Rs ");
    float cost1 = calculateCost(duration1);
    Serial.println(cost1, 2); // Print the cost with 2 decimal places
    String info = String("p") + "," + String("1") + "," + String(cost1) + "," + String(duration1.hours()) + ":" + String(duration1.minutes()) + ":" + String(duration1.seconds());
    BTSerial.println(info);
    durationShown1 = true;
  }
  if (exitRecorded2 && !durationShown2) {
    TimeSpan duration2 = exitTime2 - entryTime2;
    Serial.print("Total duration slot2: ");
    printTimeSpan(duration2);
    Serial.print("Cost for slot2: Rs");
    float cost2 = calculateCost(duration2);
    Serial.println(cost2, 2); // Print the cost with 2 decimal places
    String info = String("p") + "," + String("2") + "," + String(cost2) + "," + String(duration2.hours()) + ":" + String(duration2.minutes()) + ":" + String(duration2.seconds());
    BTSerial.println(info);
    durationShown2 = true;
  }
  if (exitRecorded3 && !durationShown3) {
    TimeSpan duration3 = exitTime3 - entryTime3;
    Serial.print("Total duration slot3: ");
    printTimeSpan(duration3);
    Serial.print("Cost for slot3: Rs");
    float cost3 = calculateCost(duration3);
    Serial.println(cost3, 2); // Print the cost with 2 decimal places
    String info = String("p") + "," + String("3") + "," + String(cost3) + "," + String(duration3.hours()) + ":" + String(duration3.minutes()) + ":" + String(duration3.seconds());
    BTSerial.println(info);
    durationShown3 = true;
  }

  // Reset entry recorded flags when car exits the slot
  if (sensorValue1 == LOW && exitRecorded1) {
    entryRecorded1 = false;
    exitRecorded1 = false;
    durationShown1 = false;
  }
  if (sensorValue2 == LOW && exitRecorded2) {
    entryRecorded2 = false;
    exitRecorded2 = false;
    durationShown2 = false;
  }
  if (sensorValue3 == LOW && exitRecorded3) {
    entryRecorded3 = false;
    exitRecorded3 = false;
    durationShown3 = false;
  }
  
  // Check if a car is detected at the exit (inverted logic)
  if (digitalRead(IR_SENSOR_EXIT) == HIGH) {
    openBarrier();// Call the function to open the barrier  
  } 
  else {
    
    closeBarrier(); // Call the function to close the barrier
  }

    if(sensorValue1==1){
    a=1;
    }
    else if(sensorValue1==0){
    a=7;
      }
  if(sensorValue2==1){
    b=2;
    }
    else if(sensorValue2==0){
    b=8;
      }
   if(sensorValue3==1){
    c=3;
    }
    else {
    c=9;
      }
  // Prepare data to send over Bluetooth
String data = String(a) + "," + String(b) + "," + String(c);
  Serial.println(data);

  // Send data over Bluetooth
  BTSerial.println(data);

  // Other existing code for displaying vacant slots
  int vacantSlots = 0;
  if (sensorValue1 == HIGH) {
    vacantSlots++;
  }
  if (sensorValue2 == HIGH) {
    vacantSlots++;
  }
  if (sensorValue3 == HIGH) {
    vacantSlots++;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vacant slots: ");
  lcd.print(vacantSlots);
  
  delay(1000);
}

void openBarrier() {
  barrierServo.write(90); // Open the barrier (assuming 90 degrees is open position)
}

void closeBarrier() {
  delay(5000); // Delay for 5 seconds
  barrierServo.write(0); // Close the barrier (assuming 0 degrees is closed position)
}

void printDateTime(DateTime time) {
  Serial.print(" ");
  Serial.print(time.hour(), DEC);
  Serial.print(':');
  Serial.print(time.minute(), DEC);
  Serial.print(':');
  Serial.println(time.second(), DEC);
}

void printTimeSpan(TimeSpan span) {
  int totalSeconds = span.totalseconds();
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;
  Serial.print(hours);
  Serial.print(':');
  Serial.print(minutes);
  Serial.print(':');
  Serial.println(seconds);  
}

float calculateCost(TimeSpan duration) {
  int totalSeconds = duration.totalseconds();
  // Assuming a simple rate, adjust this according to your actual pricing model
  float costPerSecond = 1; // Rs 1 per second
  float cost = totalSeconds * costPerSecond;
  return cost;
}
