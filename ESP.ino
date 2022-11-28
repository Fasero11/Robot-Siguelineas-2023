#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

char ssid[] = "sensoresurjc";               // your network SSID (name)
char pass[] = "Goox0sie_WZCGGh25680000";    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;                // the WiFi radio's status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.147.53.2";
int        port     = 21883;
const char topic[]  = "SETR/2022/3/";

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.// SETUP //.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  while (true) {

    if (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());

      delay(1000);
    }
    else
      break;
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.// LOOP //.//.//.//.//.//.//.//./
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

void loop() {
  
    // call poll() regularly to allow the library to send MQTT keep alives which
    // avoids being disconnected by the broker
    mqttClient.poll();

    String jsonInfo = "{TEST}";
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();

     sleep(1);
 }

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.// SHOW NETWORK INFORMATION //.//.//.//.//
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
