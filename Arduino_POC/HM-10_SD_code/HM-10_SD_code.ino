#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>

// 5V to breadboard for HM-10 and SD Card Module
// HM-10 TX to D_PIN 2, HM-10 RX to D_PIN 3
// SD MISO to D_PIN 12
// SD MOSI to D_PIN 11
// SD SCK to D_PIN 13
// SD CS to D_PIN 10

SoftwareSerial HM10(2, 3); // TX = 2, RX = 3
char appData;
String inData = "";

int CS_PIN = 10;
File file;

void setup()
{
  Serial.begin(9600);
  Serial.println("HM10 serial started at 9600");
  HM10.begin(9600); // set HM10 serial at 9600 baud rate

  initializeSD();
}

void loop()
{
  HM10.listen();  // listen the HM10 port
  while (HM10.available() > 0) {   // if HM10 sends something then read
    appData = HM10.read();
    inData = String(appData);  // save the data in string format
    Serial.write(appData);
  }


  if (Serial.available()) {           // Read user input if available.
    delay(10);
    HM10.write(Serial.read());
  }

  if (inData == "A") {
    openFile("test.txt");

    String first_line = readLine();
    HM10.print(first_line);


    Serial.println(first_line);
    Serial.println(readLine());
    Serial.println(readLine());
    Serial.println(readLine());
    closeFile();
    inData = "";
  }
}


void initializeSD()
{
  Serial.println("Initializing SD card...");
  pinMode(CS_PIN, OUTPUT);

  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
}


void closeFile()
{
  if (file)
  {
    file.close();
    Serial.println("File closed");
  }
}

int openFile(char filename[])
{
  file = SD.open(filename);
  if (file)
  {
    Serial.println("File opened with success!");
    return 1;
  } else
  {
    Serial.println("Error opening file...");
    return 0;
  }
}


String readLine()
{
  String received = "";
  char ch;
  while (file.available())
  {
    ch = file.read();
    if (ch == '\n')
    {
      return String(received);
    }
    else
    {
      received += ch;
    }
  }
  return "";
}
