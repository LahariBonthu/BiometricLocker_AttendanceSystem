#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Adafruit_Fingerprint.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>

SoftwareSerial mySerial(18, 19);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

Servo myservo;

LiquidCrystal lcd(14, 15, 16, 17, 20, 21);

RTC_DS3231 rtc;
File attendanceFile;

#define Password_Lenght 5 // Give enough room for six chars + NULL char

int pos = 0;    // variable to store the servo position

char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght] = "1470";
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
bool door = true;

byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad

Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); //initialize an instance of class NewKeypad

const int buzzerPin = 10;
const int chipSelect = 4; // Pin for SD card module

void setup() {
  Serial.begin(9600);

  if (!SD.begin(chipSelect)) {
    lcd.begin(16, 2);
    lcd.print("SD init failed!");
    while (1);
  }

  if (!rtc.begin()) {
    lcd.begin(16, 2);
    lcd.print("RTC init failed!");
    while (1);
  }

  finger.begin(57600);
  delay(5);
  pinMode(10, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  myservo.attach(9);
  ServoClose();
  lcd.clear();

  if (finger.verifyPassword()) {
    lcd.begin(16, 2);
    lcd.print("  Finger Print ");
    Serial.println("  Finger Print ");
    lcd.setCursor(0, 1);
    lcd.print("Sensor Connected");
    Serial.println("Sensor Connected");
    delay(3000);
  } else {
    lcd.begin(16, 2);
    lcd.print("Unable to found   ");
    Serial.println("Unable to found   ");
    lcd.setCursor(0, 1);
    lcd.print("Sensor");
    Serial.println("Sensor");
    delay(2000);
    lcd.begin(16, 2);
    lcd.print("Please Check    ");
    Serial.println("Please Check    ");
    lcd.setCursor(0,1);
    lcd.print("  Sensor Wiring ");
    Serial.println("  Sensor Wiring ");
    while (1) {
      delay(1);
    }
  }
}

void loop() {
  lcd.begin(16, 2);
  lcd.print("Place finger...");
  Serial.println("Place finger...");
  lcd.setCursor(0, 1);
  lcd.print("   start scan    ");
  Serial.println("   start scan    ");
  getFingerprintID();
  delay(50);

  if (door == 0) {
    customKey = customKeypad.getKey();

    if (customKey == '*') {
      lcd.clear();
      ServoClose();
      lcd.print("  Door is close");
      digitalWrite(10, LOW);
      digitalWrite(buzzerPin, HIGH);
      delay(700);
      digitalWrite(buzzerPin, LOW);
      door = 1;
    }
  } else {
    Open();
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    return p;
  } else if (p != FINGERPRINT_OK) {
    lcd.setCursor(0, 0);
    lcd.print("Scan Error      ");
    Serial.println("Scan Error      ");
    lcd.setCursor(0, 1);
    delay(2000);
    return p;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    lcd.setCursor(0, 0);
    lcd.print("Processing Error");
    Serial.println("Processing Error");
    lcd.setCursor(0, 1);
    lcd.print(" Try Again ");
    Serial.println(" Try Again ");
    return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Door Unlocked");
    Serial.println("  Door Unlocked");
    lcd.setCursor(0, 1);
    lcd.print("    Welcome");
    Serial.println("    Welcome");
    ServoOpen();
    digitalWrite(buzzerPin, HIGH);
    delay(700);
    digitalWrite(buzzerPin, LOW);
    
    // Log attendance
    logAttendance(finger.fingerID);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.setCursor(0, 0);
    lcd.print("Comm Error      ");
    Serial.println("Comm Error      ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    Serial.println("                ");
    delay(2000);
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.setCursor(0, 0);
    lcd.print("Access Denied   ");
    Serial.println("Access Denied   ");
    lcd.setCursor(0, 1);
    delay(2000);
    digitalWrite(buzzerPin, HIGH);
    delay(700);
    digitalWrite(buzzerPin, LOW);
    return p;
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Error in matching");
    Serial.println("Error in matching");
    lcd.setCursor(0, 1);
    lcd.print("Not Valid Finger");
    Serial.println("Not Valid Finger");
    delay(2000);
    return p;
  }

  return finger.fingerID;
}

void logAttendance(uint8_t id) {
  DateTime now = rtc.now();
  attendanceFile = SD.open("attendance.csv", FILE_WRITE);
  if (attendanceFile) {
    attendanceFile.print(id);
    attendanceFile.print(", ");
    attendanceFile.print(now.year(), DEC);
    attendanceFile.print('/');
    attendanceFile.print(now.month(), DEC);
    attendanceFile.print('/');
    attendanceFile.print(now.day(), DEC);
    attendanceFile.print(", ");
    attendanceFile.print(now.hour(), DEC);
    attendanceFile.print(':');
    attendanceFile.print(now.minute(), DEC);
    attendanceFile.print(':');
    attendanceFile.println(now.second(), DEC);
    attendanceFile.close();
  } else {
    lcd.setCursor(0, 1);
    lcd.print("SD Write Error");
  }
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}

void ServoOpen() {
  for (pos = 180; pos >= 0; pos -= 5) { // goes from 0 degrees to 180 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void ServoClose() {
  for (pos = 0; pos <= 180; pos += 5) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void Open() {
  customKey = customKeypad.getKey();
  if (customKey) {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }

  if (data_count == Password_Lenght - 1) {
    if (!strcmp(Data, Master)) {
      lcd.clear();
      digitalWrite(10, HIGH);
      ServoOpen();
      lcd.print("  Door is Open");
      digitalWrite(buzzerPin, HIGH);
      delay(700);
      digitalWrite(buzzerPin, LOW);
      door = 0;
    } else {
      lcd.clear();
      lcd.print("  Wrong Password");
      digitalWrite(buzzerPin, HIGH);
      delay(700);
      digitalWrite(buzzerPin, LOW);
      lcd.setCursor(0, 1);
      lcd.print("Try Again In");
      lcd.setCursor(13, 1);
      lcd.print("5");
      delay(1000);
      lcd.setCursor(13, 1);
      lcd.print("4");
      delay(1000);
      lcd.setCursor(13, 1);
      lcd.print("3");
      delay(1000);
      lcd.setCursor(13, 1);
      lcd.print("2");
      delay(1000);
      lcd.setCursor(13, 1);
      lcd.print("1");
      delay(1000);
      lcd.clear();
      door = 1;
    }
    clearData();
  }
}
