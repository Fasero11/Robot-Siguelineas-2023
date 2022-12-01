#include <WiFi.h>
#include <ArduinoMqttClient.h>

char ssid[] = "sensoresurjc";               // your network SSID (name)
char pass[] = "Goox0sie_WZCGGh25680000";    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;                // the WiFi radio's status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Define specific pins for Serial2.
#define RXD2 33
#define TXD2 4

String sendBuff;

const char broker[] = "192.147.53.2";
int        port     = 21883;
const char topic[]  = "SETR/2022/3/";

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//. JSON MESSAGES //.//.//.//.//.//.//.
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

void send_start_lap() {
    String jsonInfo = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"START_LAP\"}";  
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();      
}

void send_end_lap(long time) {
    String jsonInfo = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"END_LAP\",\"time\":\""+String(time)+"\"}";  
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();  
}

void send_obstacle_detected() {
    String jsonInfo = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"OBSTACLE_DETECTED\"}";  
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();    
}

void send_line_lost() {
    String jsonInfo = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"LINE_LOST\"}";  
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();    
}

void send_ping(long time) {
    String jsonInfo = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"PING\",\"time\":\""+String(time)+"\"}";  
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();      
}

void send_init_line_search() {
    String jsonInfo = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"INIT_LINE_SEARCH\"}";  
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();    
}

void send_stop_line_search() {
    String jsonInfo = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"STOP_LINE_SEARCH\"}";  
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();    
}

void send_line_found() {
    String jsonInfo = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"LINE_FOUND\"}";  
    mqttClient.beginMessage(topic);
    mqttClient.print(jsonInfo);
    mqttClient.endMessage();    
}

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

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
  // Regular serial connection to show traces for debug porpuses
  Serial.begin(9600);
  
  // Serial port to communicate with Arduino UNO
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

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

    // READ MESSAGES FROM ARDUINO UNO //
    if (Serial2.available()) {
    
        char c = Serial2.read();
        sendBuff += c;
        
        if (c == '}')  {            
        Serial.print("Received data in serial com: ");
        Serial.println(sendBuff);
        
        sendBuff = "";
        }
    } 
   //.//.//.//.//.//.//.//.//.//.

    // call poll() regularly to allow the library to send MQTT keep alives which
    // avoids being disconnected by the broker
    mqttClient.poll();
    
    send_start_lap(); // TEST   
    sleep(1);         // TEST
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
