#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Arduino_FreeRTOS.h>

//#define MAX_IR_THRESHOLD 550
//#define MIN_IR_THRESHOLD 500
#define IR_THRESHOLD 100 // mi suelo da valores de unos 50 y la cinta de unos 500

// ultrasonic sensor 
#define TRIG_PIN 13  
#define ECHO_PIN 12 

// infrared sensor ( from 0 to 1024 ) analog write
#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0

// Enable/Disable motor control.
//  HIGH: motor control enabled
//  LOW: motor control disabled
#define PIN_Motor_STBY 3

// Group A Motors (Right Side)
// PIN_Motor_AIN_1: Digital output. HIGH: Forward, LOW: Backward
#define PIN_Motor_AIN_1 7
// PIN_Motor_PWMA: Analog output [0-255]. It provides speed.
#define PIN_Motor_PWMA 5

// Group B Motors (Left Side)
// PIN_Motor_BIN_1: Digital output. HIGH: Forward, LOW: Backward
#define PIN_Motor_BIN_1 8
// PIN_Motor_PWMB: Analog output [0-255]. It provides speed.
#define PIN_Motor_PWMB 6

//ultrasonic sensor 
#define MAX_DIST_OBSTACLE 8

#define START_LAP 1
#define END_LAP 2
#define OBSTACLE_DETECTED 3
#define LINE_LOST 4
#define PING 5
#define INIT_LINE_SEARCH 6
#define STOP_LINE_SEARCH 7
#define LINE_FOUND 8

#define NO_LINE 0
#define LINE_LEFT 1
#define LINE_MIDLEFT 2
#define LINE_MID 3
#define LINE_MIDRIGHT 4
#define LINE_RIGHT 5

bool detected_obstacle = false;
bool is_line = true;
bool ping = false;
long aux_time = 0;

int left_ir, middle_ir, right_ir, message, prev_time, line, count;

void send_message(){
  TickType_t xLastWaskeTime, aux;

  while(1){
      xLastWaskeTime = xTaskGetTickCount();

      message = 0;
      if (!is_line){
        message = LINE_LOST;
      }
      if(detected_obstacle){
        message = OBSTACLE_DETECTED;
      }
      if(ping){
        message = PING;
      }

      Serial.print(message);

      xTaskDelayUntil(&xLastWaskeTime, 20);
    }    
}

void get_infrared(){
  TickType_t xLastWaskeTime, aux;
  while(1){
    xLastWaskeTime = xTaskGetTickCount();
    left_ir = analogRead(PIN_ITR20001_LEFT);
    middle_ir = analogRead(PIN_ITR20001_MIDDLE);
    right_ir = analogRead(PIN_ITR20001_RIGHT);


    // TODOS ESTÁN FUERA 
    if (left_ir < IR_THRESHOLD && middle_ir < IR_THRESHOLD && right_ir < IR_THRESHOLD){
      is_line = false;
      line = NO_LINE;
    }
    if (left_ir < IR_THRESHOLD && middle_ir >= IR_THRESHOLD && right_ir >= IR_THRESHOLD){
      // RIGHT
      line = LINE_RIGHT;

    }
    if (left_ir < IR_THRESHOLD && middle_ir < IR_THRESHOLD && right_ir >= IR_THRESHOLD){
      // CENTER AND RIGHT
      line = LINE_MIDRIGHT;

    }
    if (left_ir >= IR_THRESHOLD && middle_ir >= IR_THRESHOLD && right_ir < IR_THRESHOLD){
      // LEFT
      line = LINE_LEFT;

    }
    if (left_ir >= IR_THRESHOLD && middle_ir < IR_THRESHOLD && right_ir < IR_THRESHOLD){
      // CENTER AND LEFT
      line = LINE_MIDLEFT;

    } 
    if(left_ir >= IR_THRESHOLD && middle_ir < IR_THRESHOLD && right_ir >= IR_THRESHOLD){
      // JUST THE MIDDLE TOUCH THE LINE
      line = LINE_MID;
    }
    
    xTaskDelayUntil(&xLastWaskeTime, 10);
  }    
}

//return distance in cm
int get_distance(){

  float time;
  float distance;
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  time=pulseIn(ECHO_PIN, HIGH);

  // conversion into cm 
  distance = time / 29 / 2; 
  //Serial.println(distance);

  return distance;
}

void is_obstacle(){
  TickType_t xLastWaskeTime, aux;
  while(1){

    xLastWaskeTime = xTaskGetTickCount();
    int distance_sensor = get_distance();

    if (distance_sensor < MAX_DIST_OBSTACLE){
      detected_obstacle = true;
      //Serial.println("obstacle");
    } else {
      detected_obstacle = false;  
    }
    xTaskDelayUntil(&xLastWaskeTime, 10);
  }
}

void command_motors(){
  TickType_t xLastWaskeTime, aux;
  while(1){

      xLastWaskeTime = xTaskGetTickCount();
      // set high each time we use them 
      digitalWrite(PIN_Motor_AIN_1, HIGH);
      digitalWrite(PIN_Motor_BIN_1, HIGH);

      if (line == LINE_MID && !detected_obstacle){
        // right 
        analogWrite(PIN_Motor_PWMA, 50);
        // left
        analogWrite(PIN_Motor_PWMB, 50); 

      }
      if (line == NO_LINE || detected_obstacle){

        analogWrite(PIN_Motor_PWMA, 0);
        analogWrite(PIN_Motor_PWMB, 0);     
      }

      if (line == LINE_MIDRIGHT && !detected_obstacle){ // gira izq + mid 
        // right 
        analogWrite(PIN_Motor_PWMA, 20);
        // left
        analogWrite(PIN_Motor_PWMB, 40); 

      }
      if (line == LINE_RIGHT && !detected_obstacle){ // gira izq
        // right 
        analogWrite(PIN_Motor_PWMA, 0);
        // left
        analogWrite(PIN_Motor_PWMB, 40);

      }
      if (line == LINE_MIDLEFT && !detected_obstacle){ //  gira derecha + mid
        // right 
        analogWrite(PIN_Motor_PWMA, 40);
        // left
        analogWrite(PIN_Motor_PWMB, 20);

      }
      if (line == LINE_LEFT && !detected_obstacle){ // gira derecha

        // right 
        analogWrite(PIN_Motor_PWMA, 40);
        // left
        analogWrite(PIN_Motor_PWMB, 0);

      }
      xTaskDelayUntil(&xLastWaskeTime, 10);
  }   
}

/*void send_ping(){
  TickType_t xLastWaskeTime, aux;
   while(1){
      xLastWaskeTime = xTaskGetTickCount();
      if (millis() - aux_time > 1000){
        aux_time = millis();
        count++;
        ping = false;
      }
        
      if (count == 4){
        ping = true;
        count = 0;
      }
      // repeat each second
      ping = true;
      xTaskDelayUntil(&xLastWaskeTime, 10);
    }
}*/

void setup() {
  // put your setup code here, to run once:

  pinMode(PIN_Motor_STBY, OUTPUT);
  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(PIN_Motor_STBY, HIGH); // Enables motor control

  // communicate arduino with ESP to start lap

  xTaskCreate(is_obstacle, "is_obstacle", 100, NULL, 3, NULL);
  xTaskCreate(get_infrared, "get_infrared", 100, NULL, 1, NULL);
  xTaskCreate(send_message, "send_message", 100, NULL, 0, NULL);
  xTaskCreate(command_motors, "command_motors", 100, NULL, 4, NULL);

  // state message sent each 4 seconds, time field should represent time
  // since lap started . Should prioritize follow line behaviour to 
  // sending PING messages 
  //xTaskCreate(send_ping, "send_ping", 512, NULL, 2, NULL);

  Serial.begin(9600);
  //Serial.setTimeout(50);

}

void loop() {

  // intento de comunicación del esp al arduino 
  /*if (Serial.available())
  {
      char data = Serial.read();
      if (data == 'C'){
        
        Serial.println(data);
      }
  }*/
}
