#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

// ultrasonic sensor 
#define TRIG_PIN 13  
#define ECHO_PIN 12 

// infrared sensor ( de 0 a 1024 ) analog write
#define PIN_ITR20001-LEFT   A2
#define PIN_ITR20001-MIDDLE A1
#define PIN_ITR20001-RIGHT  A0

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
#define MIN_DIST_OBSTACLE 0

// in milliseconds
#define THREE_HUNDRED_MS 300


//in microseconds
#define TEN_MS 10

bool detected_obstacle = false;

ThreadController controller = ThreadController();
Thread distanceThread = Thread();


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
  //Serial.println(distance);

  return distance;
}


void callback_dist_thread(){

  int distance_sensor = 0;

  distance_sensor = get_distance();

  //Serial.println(distance_sensor);
  if (MIN_DIST_OBSTACLE < distance_sensor && distance_sensor < MAX_DIST_OBSTACLE){

    detected_obstacle = true;
  }
}

void setup() {
  // put your setup code here, to run once:

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //THREAD SETUP
  distanceThread.enabled = true;
  distanceThread.setInterval(THREE_HUNDRED_MS);
  distanceThread.onRun(callback_dist_thread);

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  controller.add(&distanceThread);

  // service state phase b)
  if(detected_obstacle){

    //we do not need this thread any more
    controller.remove(&distanceThread);
  }

  controller.run();  
}
