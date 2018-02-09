# Influx-Arduino
Simple library for writing to InfluxDB from an Arduino device

For more details on how this came about, see the [Medium post](https://medium.com/@teebr/iot-with-an-esp32-influxdb-and-grafana-54abc9575fb2).

This M0-wifi branch has been written to work with an [Adafruit M0 WiFi board](https://www.adafruit.com/product/3010), which uses ATWINC1500 and the standard WiFi101.h library. Hopefully, any board with this WiFi chip should be compatible, especially the Arduino MKR1000 which uses the same processor.

## Changes from master branch
The master branch uses the _ESP32 WiFiClientSecure_ library. This M0-wifi branch uses the _WiFiClient_ class from the _WiFi101_ library. These have similar names but quite different implementations, and I can't see a way of easily merging them right now (the Espressif library is a lot more user friendly for creating requests and adding headers...). In terms of the library, there are a couple of changes from the master branch:
- `addCertificate(const char*)` has been replaced by `useTLS(bool)`, because there is no need to save the certificate as a variable (see below).
- `authorize(const char*, const char*)` has been replaced by `authorize(const char*)`, where the single argument has to be your username and password with a colon in between, __encoded in base64__ (I could add a library to do this but it doesn't seem worth it). See below on how to convert the values.

I am currently thinking about how to handle these two libraries, in particular having a shared interface.
### Adding the certificate
Unlike for the ESP32, the root certificate is flashed to the firmware. However, you can't use the GUI in the Arduino program to add a self-signed certificate. To add your certificate:

1. download the [Wifi101 firmware updater command line tool](https://github.com/arduino-libraries/WiFi101-FirmwareUpdater/releases/latest) and CD to its directory.


1. first convert the .crt file to a binary cer file (not base64 encoding for text):
`openssl base64 -d -in </location/of/cert/name>.crt  -out certs/,<name>.cer`

1. now upload (obviously change the port name to whatever you need)
./winc1500-uploader -port /dev/cu.usbmodemXXXX -certs certs

### Encoding authentication details
To add authentication, need to convert to base64:
echo -n '<username>:<password>' | openssl base64