#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Arduino_FreeRTOS.h>

#define IR_THRESHOLD 100

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

int prev_time = 0;
bool detected_obstacle = false;
int left_ir;
int middle_ir;
int right_ir;
int message;
int is_line;

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

      if (left_ir < IR_THRESHOLD && middle_ir < IR_THRESHOLD && right_ir < IR_THRESHOLD){
        is_line = 0;
      } else {
        is_line = 1;
      }
        
//      Serial.print("LEFT: ");
//      Serial.print(left_ir);
//      Serial.print(" MIDDLE: ");
//      Serial.print(middle_ir);
//      Serial.print(" RIGHT: ");
//      Serial.println(right_ir);
      
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
      //Serial.println("CHECKING ULTRASOUND");
      xLastWaskeTime = xTaskGetTickCount();
      int distance_sensor = get_distance();
    
      //Serial.println(distance_sensor);
      if (distance_sensor < MAX_DIST_OBSTACLE){
        detected_obstacle = true;
      } else {
        detected_obstacle = false;  
      }
      xTaskDelayUntil(&xLastWaskeTime, 10);
  }
}

void command_motors(){
  TickType_t xLastWaskeTime, aux;
  while(1){
      //Serial.println("COMMANDING MOTORS");
      xLastWaskeTime = xTaskGetTickCount();
      digitalWrite(PIN_Motor_AIN_1, HIGH);
      digitalWrite(PIN_Motor_BIN_1, HIGH);
      if (is_line && !detected_obstacle){
        analogWrite(PIN_Motor_PWMA, 125);
        analogWrite(PIN_Motor_PWMB, 125);   
      } else {
        analogWrite(PIN_Motor_PWMA, 0);
        analogWrite(PIN_Motor_PWMB, 0);     
      }

      xTaskDelayUntil(&xLastWaskeTime, 10);
  }   
}

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

  xTaskCreate(is_obstacle, "is_obstacle", 100, NULL, 2, NULL);
  xTaskCreate(get_infrared, "get_infrared", 100, NULL, 1, NULL);
  xTaskCreate(send_message, "send_message", 500, NULL, 0, NULL);
  xTaskCreate(command_motors, "command_motors", 50, NULL, 3, NULL);

  Serial.begin(9600);

}

void loop() {
}
