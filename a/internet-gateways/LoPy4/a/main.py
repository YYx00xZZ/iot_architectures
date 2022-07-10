import sys

import utime
import time
import pycom
import ntptime
from mqtt import MQTTClient
from machine import Timer
#import DateTime
#
from LIS2HH12 import LIS2HH12
from SI7006A20 import SI7006A20
from LTR329ALS01 import LTR329ALS01
from MPL3115A2 import MPL3115A2,ALTITUDE,PRESSURE
from pycoproc_2 import Pycoproc


Device_name = "PySense_1" # Сенсор Home
MQTT_USERNAME = "<Your Username>"
MQTT_PASSWORD = "<MQTT PASSWORD>"
MQTT_PORT = 1883

pycom.heartbeat(False)
pycom.rgbled(0x0A0A08) # white

class Wifi_Daemon:

    def __init__(self):
        # self.seconds = 0
        self.wifi_status = wlan.isconnected()
        self.__alarm = Timer.Alarm(self._wifi_status_handler, 2, periodic=True)

    def _wifi_status_handler(self, alarm):
        self.wifi_status = wlan.isconnected()
        if self.wifi_status == False:
            print("[WARNING]\tWiFi disconnected")
            machine.reset()


wifi_daemon = Wifi_Daemon()


def setRTCLocalTime():
    rtc = machine.RTC()
    rtc.ntp_sync("pool.ntp.org")
    utime.sleep_ms(750)
    print('\nRTC Set from NTP to UTC:', rtc.now())
    year, month, day, hour, minute, second, millis, _tzinfo = rtc.now()
    return "%02d-%02d-%d  %02d:%02d:%02d" % (day, month, year, hour, minute, second) 


str_date_time = setRTCLocalTime()
print( '------------setRTCLocalTime---------------')
print(str_date_time)
print( '------------end setRTCLocalTime---------------')

py = Pycoproc()

mp = MPL3115A2(py,mode=ALTITUDE) # Returns height in meters. Mode may also be set to PRESSURE, returning a value in Pascals
print("MPL3115A2 temperature: " + str(mp.temperature()))
print("Altitude: " + str(mp.altitude()))
mpp = MPL3115A2(py,mode=PRESSURE) # Returns pressure in Pa. Mode may also be set to ALTITUDE, returning a value in meters
print("Pressure: " + str(mpp.pressure()))

print('BP 2')

si = SI7006A20(py)
print("Temperature: " + str(si.temperature())+ " deg C and Relative Humidity: " + str(si.humidity()) + " %RH")
print("Dew point: "+ str(si.dew_point()) + " deg C")
t_ambient = 24.4
print("Humidity Ambient for " + str(t_ambient) + " deg C is " + str(si.humid_ambient(t_ambient)) + "%RH")


lt = LTR329ALS01(py)
print("Light (channel Blue lux, channel Red lux): " + str(lt.light()))

li = LIS2HH12(py)
print("Acceleration: " + str(li.acceleration()))
print("Roll: " + str(li.roll()))
print("Pitch: " + str(li.pitch()))

print("Battery voltage: " + str(py.read_battery_voltage()))



client = MQTTClient("DeviceID-", "mqtt.flespi.io", user=MQTT_USERNAME, password=MQTT_PASSWORD, port=MQTT_PORT)

client.connect()

while True:
    try:
        # client.publish(topic="lopy4/test", msg="Hi from lopy4")
        client.publish(topic=Device_name+"/temp", msg=str(mp.temperature()))	
        utime.sleep(5)

        client.publish(topic=Device_name+"/alt", msg=str(mp.altitude()))
        utime.sleep(2)

        client.publish(topic=Device_name+"/press", msg=str(mpp.pressure()))
        utime.sleep(2)

        client.publish(topic=Device_name+"/hum", msg=str(si.humid_ambient(t_ambient)))
        utime.sleep(2)

        client.publish(topic=Device_name+"/lux_Blue", msg=str(lt.light()[0]))
        utime.sleep(2)

        client.publish(topic=Device_name+"/lux_Red", msg=str(lt.light()[1]))
        utime.sleep(2)

        client.publish(topic=Device_name+"/volt", msg=str(py.read_battery_voltage()))
        utime.sleep(2)

        client.publish(topic=Device_name+"/ipconf", msg=str(wlan.ifconfig()))
        utime.sleep(2)

        client.publish(topic=Device_name+"/str_date_time", msg=str_date_time)
        utime.sleep(2)

    except OSError:
        client.connect()
        print("Whew!", sys.exc_info(), "occurred.")
    finally:
        time.sleep(5)
