#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#define IR_THRESHOLD 100

// ultrasonic sensor 
#define TRIG_PIN 13  
#define ECHO_PIN 12 

// infrared sensor ( de 0 a 1024 ) analog write
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
//#define MIN_DIST_OBSTACLE 0

// in milliseconds
#define THREE_HUNDRED_MS 300


//in microseconds
#define TEN_MS 10

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

ThreadController controller = ThreadController();
Thread distanceThread = Thread();
Thread IRThread = Thread();
Thread msgThread = Thread();

void send_message(){
    Serial.print(message);    
}

//return distance in cm
int get_distance(){

  float time;
  float distance;
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(TEN_MS);
  digitalWrite(TRIG_PIN, LOW);
  time=pulseIn(ECHO_PIN, HIGH);
  
  // conversion into cm 
  distance = time / 29 / 2; 
  // Serial.println(distance);

  return distance;
}

void get_infrared(){
  left_ir = analogRead(PIN_ITR20001_LEFT);
  middle_ir = analogRead(PIN_ITR20001_MIDDLE);
  right_ir = analogRead(PIN_ITR20001_RIGHT);
    
//  Serial.print("LEFT: ");
//  Serial.print(left_ir);
//  Serial.print(" MIDDLE: ");
//  Serial.print(middle_ir);
//  Serial.print(" RIGHT: ");
//  Serial.println(right_ir);    
}

void callback_dist_thread(){

  int distance_sensor = get_distance();

  //Serial.println(distance_sensor);
  if (distance_sensor < MAX_DIST_OBSTACLE){
    detected_obstacle = true;
  }
}

void setup() {
  // put your setup code here, to run once:

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //THREAD SETUP
  distanceThread.enabled = true;
  distanceThread.setInterval(300); // ms
  distanceThread.onRun(callback_dist_thread);

  IRThread.enabled = true;
  IRThread.setInterval(100); // ms
  IRThread.onRun(get_infrared);

  msgThread.enabled = true;
  msgThread.setInterval(1000); // ms
  msgThread.onRun(send_message);

  controller.add(&distanceThread);
  controller.add(&IRThread);
  controller.add(&msgThread); 

  Serial.begin(9600);

}

void loop() {
    
  if (left_ir < IR_THRESHOLD && middle_ir < IR_THRESHOLD && right_ir < IR_THRESHOLD){
    message = LINE_LOST;
  } else {
    message = 0;
  }
    
  if(detected_obstacle){
    controller.remove(&distanceThread);
    // Send message once every second.
    message = OBSTACLE_DETECTED;
    controller.add(&msgThread); 
  }

  controller.run();  
}
