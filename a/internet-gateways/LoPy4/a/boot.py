from network import WLAN
import machine
import time

wifi_creds = [
        {
            'ssid': '<Your ssid>',
            'password': '<your password>'
        },
        {
            'ssid': '<Your ssid 2>',
            'password': '<your password 2>'
        }
    ]

wlan=WLAN(WLAN.STA)

WIFI_IS_CONNECTED = False

NETWORK_FOUND=False

wlan.disconnect()

def scan_field():
    print('[INFO]\tScan started')
    nets = []
    while len(nets) == 0:
        try:
            nets = wlan.scan()
        except OSError:
            print("[WARNING]\t Scan failed.")
    print('[INFO]\tScan ended')
    return nets

# nets = scan_field()

# for net in nets:
#     print(net)

def connect_wifi(nets, wifi_creds):
    global NETWORK_FOUND
    global WIFI_IS_CONNECTED
    for net in nets:
        for wifi_cred in wifi_creds:
            if str(net[0]) == str(wifi_cred['ssid']):
                print('[INFO]\tmatch -> ' + net[0] + ' ' + wifi_cred['ssid'])
                NETWORK_FOUND=True
                try:
                    wlan.connect(ssid=wifi_cred['ssid'], auth=(WLAN.WPA2, wifi_cred['password']), timeout=5000)
                    while not wlan.isconnected():
                        machine.idle()
                    print("[INFO]\tWiFi connected succesfully")
                    print("[INFO]\tIP: " + wlan.ifconfig()[0])
                    print("[INFO]\tNetwork Mask: " + wlan.ifconfig()[1])
                    print("[INFO]\tGateway: " + wlan.ifconfig()[2])
                    print("[INFO]\tDNS: " + wlan.ifconfig()[3])
                    WIFI_IS_CONNECTED = True
                    return
                except TimeoutError:
                    print('[WARNING]\tTimeoutError')
                    WIFI_IS_CONNECTED = False
    if NETWORK_FOUND == False:
        print("[WARNING]\tNone of the networks was found.")


while WIFI_IS_CONNECTED == False:
    nets = scan_field()
    for net in nets:
        print(net)
    connect_wifi(nets, wifi_creds)
    time.sleep(1)
