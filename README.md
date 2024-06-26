# BiometricLocker_AttendanceSystem
 
The main objective of this project is to design a fingerprint sensor locker such that system can be accessed using a reliable fingerprint sensor to ensure that only authorized individuals can access the locker. Creating a system that accurately records attendance data for users, including timestamps and user identifiers and integrating this to a reliable locking mechanism controlled by the system to secure the contents present in the locker. 


## Components and Their Connections

### Fingerprint Sensor:
Connect the sensor's TX to Arduino pin 18 (RX).\
Connect the sensor's RX to Arduino pin 19 (TX).\
Connect VCC and GND to 5V and GND, respectively.

### Keypad:
Connect the row pins to Arduino pins 0, 1, 2, and 3.\
Connect the column pins to Arduino pins 4, 5, 6, and 7.

### LCD Display:
Connect the RS, E, D4, D5, D6, and D7 pins to Arduino pins 14, 15, 16, 17, 20, and 21, respectively.\
Connect VCC and GND to 5V and GND, respectively.

### Servo Motor:
Connect the control wire to Arduino pin 9.\
Connect VCC and GND to an appropriate power supply.

### Real-Time Clock (RTC) Module:
Connect SDA to A4 and SCL to A5 (for most Arduino boards).\
Connect VCC and GND to 5V and GND, respectively.

### SD Card Module:
Connect CS to pin 4 (chip select).\
Connect SCK to pin 13 (serial clock).\
Connect MOSI to pin 11 (master out, slave in).\
Connect MISO to pin 12 (master in, slave out).\
Connect VCC and GND to 5V and GND, respectively.

### Buzzer:
Connect one pin to Arduino pin 10.\
Connect the other pin to GND.


## Libraries Used
Keypad.h: To handle the keypad input.\
LiquidCrystal.h: To control the LCD display.\
Servo.h: To control the servo motor.\
Adafruit_Fingerprint.h: To interface with the fingerprint sensor.\
RTClib.h: To interface with the real-time clock (RTC) module.\
SD.h: To read and write data to the SD card.\
SPI.h: Required for communication with the SD card module.


## Mechanism
The arduino code includes controlling servo motor with the keypad using correct password and by using verifed and accessible fingerprint both.
Using servo we can open or close the door to take the contents of the box.\
The fingerprint sensor also records attendance of the user with time stamp and user ID if the user is enrolled in the module already.\
Once the door is opened we can close it by using the ‘#’ key in the keypad directly according to our code and buzzer sounds occurs after every step of accessing the locker or attendance.\
logAttendance function logs the fingerprint ID, date, and time to an attendance.csv file on the SD card.\
The logAttendance function inside the getFingerprintID function when a fingerprint match is found, and the door is unlocked.\
The Arduino project will now log attendance with date and time whenever a registered fingerprint is recognized and the door is unlocked.\
The logged data can be imported into an Excel sheet for further processing.

## Backup
And if the the system fails due to some power issue or loose connections, then we can open the the locker using a physical key lock mechanism.\
We have to place the lock such that the rim coming out after opening with the key should push the servo motor to 90 degrees and with this we should be able to open the door.
