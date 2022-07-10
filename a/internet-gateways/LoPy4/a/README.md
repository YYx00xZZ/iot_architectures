# LoPy4
![Pycom LoPy4](https://user-images.githubusercontent.com/47386361/148212731-c2cac252-3958-4a18-be67-c3dd899a0563.png)

The folder contains code for Pycom's LoPy4 board with Pysense v2.0 extension.
For use with other extension boards just add the libs you need and change the code for reading the sensors.

Functionallities:
- Connect to WiFi network with best RSSI
- Auto-reconnect to WiFi
- Auto-reconnect to MQTT (currently does `machine.reset()` on MQTT disconnect)
