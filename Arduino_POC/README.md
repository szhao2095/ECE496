# Arduino_POC

This folder contains proof of concept code for the arduino module, each sub folder contains code to one aspect/module that is being 
proven.

## HM_10_SD_RTC_ultrasonic

[wiring]: wiring_diagram.jpg "wiring_diagram.jpg"

![alt text][wiring]

Setup procedure. Take a look at the readme in the folder, essentially:

- Install D3231.zip as a library in Arduino INO
- Install Time library using Arduino INO library manager
- Set up modules and UNO according to circuit diagram provide, or look at code comments for pin out
- Run code given, note that there will be C++ warnings/errors regarding chars and strings, just ignore
- Be sure to provide a up to date Unix Timestamp

Most important folder here and is the culmination of all the other folders. Implements the following
- Sets RTC up based on Unix Timestamp given (have to give otherwise there's no need for a RTC in the first place)
- Sets up ultrasonic sensor to sample every X seconds
- Sensor reading is saved to file on the MicroSD card
- Upon receiving correct command over Bluetooth, the data file is dumped over Bluetooth

TODO
- Determine format to store the data as
- Determine if there's a need to send only desired files (i.e. command to include file names)
- Bluetooth datarate should be bumped up if possible (currently 9600Baud/s)
- When and how the data/file deletion should be done on the MicroSD card