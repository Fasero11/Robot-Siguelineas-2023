#include <WiFi.h>
#include <ArduinoMqttClient.h>

char ssid[] = "sensoresurjc";               // your network SSID (name)
char pass[] = "Goox0sie_WZCGGh25680000";    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;                // the WiFi radio's status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.147.53.2";
int        port     = 21883;
const char topic[]  = "SETR/2022/3/";

void initWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  printWifiData();
}


//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.// SETUP //.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  initWiFi();

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
    Serial.println("TEST");
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
}

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.