# Influx-Arduino
Simple library for writing to InfluxDB from an Arduino device

For more details on how this came about, see this [Medium post](https://medium.com/@teebr/iot-with-an-esp32-influxdb-and-grafana-54abc9575fb2).

This has been tested with an ESP32: any board should work as long as it has an HTTPClient library and an encryption library (e.g mbed TLS) to go with it.
**UPDATE**
it turns out the ESP32 HTTPClient is a bit different to the Arduino one, so the code isn't directly compatible. I'm working on setting it up on a M0 / ATWINC1500, and I'll see what changes are involved there.


