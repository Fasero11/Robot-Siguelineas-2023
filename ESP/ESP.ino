#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include <WiFi.h>


#define WLAN_SSID "sensoresurjc"                // your network SSID (name)
#define WLAN_PASS "Goox0sie_WZCGGh25680000"     // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;                    // the WiFi radio's status

#define MQTT_SERVER "193.147.53.2"
#define MQTT_SERVERPORT 21883

WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT);

// Setup a feed called 'test' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish topic = Adafruit_MQTT_Publish(&mqtt, "/SETR/2022/3/");

// Define specific pins for Serial2.
#define RXD2 33
#define TXD2 4

#define START_LAP 1
#define END_LAP 2
#define OBSTACLE_DETECTED 3
#define LINE_LOST 4
#define PING 5
#define INIT_LINE_SEARCH 6
#define STOP_LINE_SEARCH 7
#define LINE_FOUND 8

String sendBuff;

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//. JSON MESSAGES //.//.//.//.//.//.//.
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

void send_json(){
    
    switch (sendBuff.toInt()){
      case START_LAP:
        Serial.println("Sending START_LAP");
        send_start_lap();
        break;

      case END_LAP:
        Serial.println("Sending END_LAP");
        send_end_lap();
        break;

      case OBSTACLE_DETECTED:
        Serial.println("Sending OBSTACLE_DETECTED");
        send_obstacle_detected();
        break;

      case LINE_LOST:
        Serial.println("Sending LINE_LOST");
        send_line_lost();
        break;

      case PING:
        Serial.println("Sending PING");
        send_ping();
        break;
        
      case INIT_LINE_SEARCH:
        Serial.println("Sending INIT_LINE_SEARCH");
        send_init_line_search();
        break;

      case STOP_LINE_SEARCH:
        Serial.println("Sending STOP_LINE_SEARCH");
        send_stop_line_search();
        break;

      case LINE_FOUND:
        Serial.println("Sending LINE_FOUND");
        send_line_found();
        break;

      default:
        Serial.println("Error: Invalid value.");
        break;
    }
}

void send_start_lap() {
  
    char jsoninfo[] = "{\"team_name\":\"Robot-Maniac\",\"id\":\"3\",\"action\":\"START_LAP\"}";
    topic.publish(jsoninfo);        
}

void send_end_lap() {
    long time_ms = millis();
    char jsoninfo[100];
    sprintf(jsoninfo, "{\"team_name\":\"Robot-Maniac\",\"id\":\"3\",\"action\":\"END_LAP\",\"time\":\" %ld\"}", time_ms);
    topic.publish(jsoninfo);  
}

void send_obstacle_detected() {
    char jsoninfo[] = "{\"team_name\":\"Robot-Maniac\",\"id\":\"3\",\"action\":\"OBSTACLE_DETECTED\"}";
    topic.publish(jsoninfo);    
}

void send_line_lost() {
    char jsoninfo[] = "{\"team_name\":\"Robot Maniac\",\"id\":\"3\",\"action\":\"LINE_LOST\"}";
    topic.publish(jsoninfo);    
}

void send_ping() {
    long time_ms = millis();
    char jsoninfo[100];
    sprintf(jsoninfo, "{\"team_name\":\"Robot-Maniac\",\"id\":\"3\",\"action\":\"PING\",\"time\":\" %ld\"}", time_ms);
    topic.publish(jsoninfo);        
}

void send_init_line_search() {
    char jsoninfo[] = "{\"team_name\":\"Robot-Maniac\",\"id\":\"3\",\"action\":\"INIT_LINE_SEARCH\"}";
    topic.publish(jsoninfo);     
}

void send_stop_line_search() {
    char jsoninfo[] = "{\"team_name\":\"Robot-Maniac\",\"id\":\"3\",\"action\":\"STOP_LINE_SEARCH\"}";
    topic.publish(jsoninfo);    
}

void send_line_found() {
    char jsoninfo[] = "{\"team_name\":\"Robot-Maniac\",\"id\":\"3\",\"action\":\"LINE_FOUND\"}";
    topic.publish(jsoninfo);       
}

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
}

void initWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
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
}

//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.
//.//.//.//.//.//.//.// LOOP //.//.//.//.//.//.//.//./
//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.//.

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
    MQTT_connect();
    
    // READ MESSAGES FROM ARDUINO UNO //
    if (Serial2.available()) {
    
        char c = Serial2.read();
        sendBuff += c;

        Serial.print("Received data in serial com: ");
        Serial.print(sendBuff);
        //Serial.print(" SIZE: ");
        //Serial.println(sizeof(sendBuff));

        send_json();

        sendBuff = "";
    }
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
