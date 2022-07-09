/*
 * Connect to the best wifi network or the one that is
 * available based on predefined list. If the network
 * that is currently in use disapers and a second one from
 * the list is available the device will auto-connect to it.
 * Wifi reconnect to the same network is also supported.
 * The device will then connect to MQTT broker and again
 * will auto reconnect if needed.
 */
/* WiFi.status() return codes
   Value Constant           Meaning
    0     WL_IDLE_STATUS      temporary status assigned when WiFi.begin() is called
    1     WL_NO_SSID_AVAIL    when no SSID are available
    2     WL_SCAN_COMPLETED   scan networks is completed
    3     WL_CONNECTED        when connected to a WiFi network
    4     WL_CONNECT_FAILED   when the connection fails for all the attempts
    5     WL_CONNECTION_LOST  when the connection is lost
    6     WL_DISCONNECTED     when disconnected from a network
*/

#include "WiFi.h"
#include "PubSubClient.h"

#define MQTT_SERVER "<Broker IP Or Hostname>"
#define MQTT_USERNAME "<Your Username Here"
#define MQTT_PASSWORD "<your Password Here>"
#define MQTT_PORT 1883

#define SCAN_DELAY 2000
#define RECONNECT_DELAY 2000

#define SSID_MAX_LEN      32
#define PASS_MAX_LEN      64

unsigned long previousMillis = 0;
unsigned long interval = 10000;

typedef struct
{
  char ssid[SSID_MAX_LEN + 1];
  char pass[PASS_MAX_LEN + 1];
}  WiFi_Credentials;

WiFi_Credentials WiFi_Creds[] =
{
  { "<A SSID here>", "<PaSsWoRd>"},
  { "<A SSID here>", "<PaSsWoRd>"},
  { "<A SSID here>", "<PaSsWoRd>"},
};

#define NUMBER_SSIDS    ( sizeof(WiFi_Creds) / sizeof(WiFi_Credentials) )

int nNetworks;

WiFiClient wifi_client;
PubSubClient mqttClient(wifi_client);

void printWifiStatus() {
  switch (WiFi.status()) {
    case 0 :
      Serial.println( "WL_IDLE_STATUS");
      break;
    case 1 :
      Serial.println( "WL_NO_SSID_AVAIL");
      break;
    case 2 :
      Serial.println( "WL_SCAN_COMPLETED");
      break;
    case 3 :
      Serial.println( "WL_CONNECTED");
      break;
    case 4 :
      Serial.println( "WL_CONNECT_FAILED");
      break;
    case 5 :
      Serial.println( "WL_CONNECTION_LOST");
      break;
    case 6 :
      Serial.println( "WL_DISCONNECTED");
      break;
  }
}

/*
   Callback function that executes every time a message is received
*/
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.println("[MQTT][INFO]\tMessage arrived");
  Serial.print("[MQTT][TOPIC]\t");    Serial.println(topic);
  Serial.print("[MQTT][PAYLOAD]\t");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void setup() {
  Serial.begin(115200);

  Serial.println("\n\n\t\t\tSTART SETUP\n\n");

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // disconnect from an AP if it was previously connected
  WiFi.disconnect();
  delay(100);

  nNetworks = scanField();

  connectWIFI(nNetworks, NUMBER_SSIDS);

  // MQTT PART
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);
  // MQTT PART

  Serial.println("\n\n\t\tEND SETUP\n\n");
}

void loop(void) {
  if (!mqttClient.connected()) {
    Serial.println("[MQTT][INFO]\tDisconnected.");
    mqttReconnect();
  }

  unsigned long currentMillis = millis();

  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    Serial.println("[INFO]\tWiFi disconnected");
    Serial.println(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    delay(RECONNECT_DELAY);
    nNetworks = scanField();
    connectWIFI(nNetworks, NUMBER_SSIDS);
    Serial.println("[INFO]\tReconnected to WiFi");
    //Alternatively, you can restart your board
    //ESP.restart();
    //Serial.println(WiFi.RSSI());
    previousMillis = currentMillis;
  }
  mqttClient.loop();

}

int scanField() {
  Serial.println("[INFO]\tscan start");
  Serial.println("[INFO]\tscan done");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  /*
       int16_t WiFiScanClass::scanComplete()
       called to get the scan state in Async mode

       return:
       scan result as number of found networks or status -1 if scan not finished -2 if scan not triggered
  */
  //int16_t scanStatus = WiFi.scanComplete();
  if (n == 0) {
    Serial.println("[INFO]\tno networks found");
  }
  else {
    Serial.print("[INFO]\t");
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);

      Serial.print(", SSID=");
      Serial.print(WiFi.SSID(i));

      Serial.print(", BSSID=");
      Serial.print(WiFi.BSSIDstr(i));

      Serial.print(", RSSI=");
      Serial.print(WiFi.RSSI(i));

      Serial.print(", ENCRYPTION TYPE=");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN :
          Serial.println( "OPEN");
          break;
        case WIFI_AUTH_WEP :
          Serial.println( "WEP");
          break;
        case WIFI_AUTH_WPA_PSK :
          Serial.println( "WPA_PSK");
          break;
        case WIFI_AUTH_WPA2_PSK :
          Serial.println( "WPA2_PSK");
          break;
        case WIFI_AUTH_WPA_WPA2_PSK :
          Serial.println( "WPA_WPA2_PSK");
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE :
          Serial.println( "WPA2_ENTERPRISE");
          break;
        case WIFI_AUTH_MAX :
          Serial.println( "MAX");
          break;
      }
      delay(10);
    }
  }
  Serial.println();
  // Wait a bit before scanning again
  delay(SCAN_DELAY);
  return n;
}

void connectWIFI(int nNets, int nSSIDS) {
  Serial.println("[INFO]\tStart matching network");
  for (int i = 0; i < nNets; ++i) {
    for (int ii = 0; ii < nSSIDS; ii++) {
      if (String(WiFi_Creds[ii].ssid) == WiFi.SSID(i)) {
        Serial.print("[INFO]\tMATCH: ");
        Serial.print(String(WiFi_Creds[ii].ssid));
        Serial.print(" -> ");
        Serial.println(WiFi.SSID(i));
        WiFi.begin(WiFi_Creds[ii].ssid, WiFi_Creds[ii].pass);
        Serial.print("[INFO]\tConnecting to ");
        Serial.print(WiFi_Creds[ii].ssid);
        while (WiFi.status() != WL_CONNECTED)
        {
          delay(500);
          Serial.print(".");
        }
        Serial.println("");
        Serial.print("[INFO]\t");   printWifiStatus();
        Serial.print("[INFO]\t");   Serial.println(WiFi.localIP());

        return;
      }
    }
  }
}

void mqttReconnect() {
  // Loop until we're reconnected
  for (int i = 0; i < 5; ++i) {
    if (!mqttClient.connected()) {
      Serial.println("[MQTT][INFO]\tAttempting MQTT connection...");
      // Attempt to connect
      if (mqttClient.connect("arduinoClient", MQTT_USERNAME, MQTT_PASSWORD, "endTopic", 1, true, "offline")) {
        Serial.println("[MQTT][INFO]\tConnected.");
        // Subscribe
        mqttClient.publish("outTopic", "online");
        mqttClient.subscribe("inTopic");
      } else {
        Serial.print("[MQTT][INFO]\tFailed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
  return;
}
