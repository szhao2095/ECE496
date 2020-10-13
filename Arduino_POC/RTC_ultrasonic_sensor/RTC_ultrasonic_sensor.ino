#include <DS3231.h>
#include <Time.h>
#include <TimeLib.h>

// In seconds
unsigned long measurement_delay = 1;
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
  //Serial.print(weekday_num);
  
  // Initialize the rtc object
  rtc.begin();
  Serial.println("3");
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014
  rtc.setDOW(weekday_num);    
  Serial.println("3.1");
  rtc.setTime(hour(t), minute(t), second(t));   
  Serial.println("3.2");
  rtc.setDate(day(t), month(t), year(t)); 
  Serial.println("3.3");

  // Use this line to retrieve the unix time from RTC if needed
  // Serial.print(rtc.getUnixTime(rtc.getTime()));
}

void loop()
{
  
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
  }
   
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
