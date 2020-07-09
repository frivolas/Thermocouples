# Thermocouples

Arduino and Processing sketches to collect data from an array of K-type thermocouples amplified with MAX31855K amplifiers.

Both Arduino sketches are heavily based on the Adafruit learn examples. Maybe just a bit modified to add a check and to do the data transfer.

### Temp_logger_SD
Reads the thermocouples every minute. But you can modify this by editing the value of the global variable
`unsigned long LONG_DELAY_MS = 60000; //1 min in ms`

Make sure you specify the delay in milliseconds

This skecth saves each reading with a timestamp to a CSV file in the SD card. Add a large enough SD card and you can have this run for a very long time.

Once you're done, simply remove power from the system and eject the SD card.

If you lack an SD card slot in your computer, use the Temp_to_Serial sketch instead and collect the data with the processing sketch.

### Temp_to_serial
You need to load the processing sketch to interface with this Arduino sketch.
There's a simple handshake at the beginning and then the thermocouple data is sent in an array of bytes evey so many milliseconds

The processing sketch shows a nice bar graph with color coding and saves the data to a CSV file that is created in the /Data folder under the same sketch folder that your processing sketch lives in.
