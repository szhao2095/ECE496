#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>

#include <DS3231.h>
#include <Time.h>
#include <TimeLib.h>

// 5V to breadboard for HM-10 and SD Card Module
// HM-10 TX to D_PIN 2, HM-10 RX to D_PIN 3
// SD MISO to D_PIN 12
// SD MOSI to D_PIN 11
// SD SCK to D_PIN 13
// SD CS to D_PIN 10
// Ultrasonic sensor Trigger to D_PIN 7, Echo to D_PIN 6

// Voltage supplies
// HM-10 3.3V
// D3231 RTC 3.3V
// MicroSD 5V
// Ultrasonic sensor 5V

SoftwareSerial HM10(2, 3); // TX = 2, RX = 3
char appData;
String inData = "";

int CS_PIN = 10;
File file;

// In seconds
unsigned long measurement_delay = 2000;
unsigned long last_trigger = 0; 
  
const int pingPin = 7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

void setup()
{
  // Setup Serial connection
  Serial.begin(9600);
  // Uncomment the next line if you are using an Arduino Leonardo
  //while (!Serial) {}

  // Initialize current time object
  setTime(1602450000UL);
  time_t t = now();
  int weekday_num = weekday(t);
  // Need to re-map the numbers, might have to change depending on library version
  // Monday: Need 1, weekday(t) returns 2
  // Tuesday: Need 2, weekday(t) returns 3
  // ...
  // Sunday: Need 7, weekday(t) return 1
  weekday_num -= 1;
  if (weekday_num == 0) {
    weekday_num = 7;
  }
  //Serial.println(weekday_num);
  
  // Initialize the rtc object
  rtc.begin();
  
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014
  rtc.setDOW(weekday_num);    
  rtc.setTime(hour(t), minute(t), second(t));   
  rtc.setDate(day(t), month(t), year(t)); 

  // Use this line to retrieve the unix time from RTC if needed
  // Serial.print(rtc.getUnixTime(rtc.getTime()));

  // set HM10 serial at 9600 baud rate
  HM10.begin(9600); 

  initializeSD();


//  // Sample code for writing to, opening, and deleting file
//  createFile("test2.txt");
//  writeToFile("This is sample text 123123!");
//  closeFile();
//
//  openFile("test2.txt", false);
//  Serial.println(readLine());
//  closeFile();
//
//  SD.remove("test2.txt");
//  
//  openFile("test2.txt", false);
//  Serial.println(readLine());
//  closeFile();
}

void loop()
{
  HM10.listen();  // listen the HM10 port
  while (HM10.available() > 0) {   // if HM10 sends something then read
    appData = HM10.read();
    inData += String(appData);  // save the data in string format
    //Serial.println(appData);
  }
  if (HM10.available() == 0 && inData != "") {
    if (inData == "A") {
      openFile("test.txt", false);

      // This is dumping whole file to Bluetooth link
      //HM10.print(first_line);
      while (file.available()) {
        //Serial.write(file.read());
        HM10.write(file.read());
      }
//      String first_line = readLine();
//      Serial.println(first_line);
//      Serial.println(readLine());
//      Serial.println(readLine());
      closeFile();
      inData = "";
    }
  }
//  // Send Day-of-Week
//  Serial.print(rtc.getDOWStr());
//  Serial.print(" ");
//  
//  // Send date
//  Serial.print(rtc.getDateStr());
//  Serial.print(" -- ");
//
//  // Send time
//  Serial.println(rtc.getTimeStr());
//  
//  // Wait one second before repeating :)
//  delay(1000);


  if ((rtc.getUnixTime(rtc.getTime()) - last_trigger) >= measurement_delay){
    last_trigger = rtc.getUnixTime(rtc.getTime());
    long duration, inches, cm;
    duration = ultrasoundTakeReading();
    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();

    String data = "";
    data += String(last_trigger);
    data += ": ";
    data += String(duration);
    data += ", ";
    data += String(inches);
    data += "inches, ";
    data += String(cm);
    data += "cm \n";
    char str_array[data.length()];
    data.toCharArray(str_array, data.length());

//    createFile("test.txt");
    openFile("test.txt", true);
    writeToFile(str_array);
    closeFile();
  }
}


void initializeSD() {
  Serial.println("Initializing SD card...");
  pinMode(CS_PIN, OUTPUT);

  if (SD.begin()) {
    Serial.println("SD card is ready to use.");
  } else {
    Serial.println("SD card initialization failed");
    return;
  }
}

int createFile(char filename[]) {
  file = SD.open(filename, FILE_WRITE);
  if (file) {
    Serial.println("File created successfully.");
    return 1;
  } else {
    Serial.println("Error while creating file.");
    return 0;
  }
}

int writeToFile(char text[]) {
  if (file) {
    file.println(text);
    Serial.println("Writing to file: ");
    Serial.println(text);
    return 1;
  } else {
    Serial.println("Couldn't write to file");
    return 0;
  }
}

void closeFile() {
  if (file) {
    file.close();
    Serial.println("File closed");
  }
}

int openFile(char filename[], bool write) {
  if (write) {
    file = SD.open(filename, FILE_WRITE);
  } else {
    file = SD.open(filename, FILE_READ);
  }
  if (file) {
    Serial.println("File opened with success!");
    return 1;
  } else {
    Serial.println("Error opening file...");
    return 0;
  }
}

String readLine() {
  String received = "";
  char ch;
  while (file.available()) {
    ch = file.read();
    if (ch == '\n') {
      return String(received);
    }
    else {
      received += ch;
    }
  }
  return "";
}

long ultrasoundTakeReading(){
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

long microsecondsToInches(long microseconds) {
   return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
