#include <Arduino_FreeRTOS.h>
#include "FastLED.h"

#define PIN_RBGLED 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

#define RXD2 33
#define TXD2 4

#define STD_VELOCITY 120
#define MAX_VELOCITY 255
#define MIN_VELOCITY 0
#define LOST_LINE_VEL 120
#define LOST_LINE_SECOND_VEL 0

#define IR_R_THRESHOLD 650
#define IR_M_THRESHOLD 450
#define IR_L_THRESHOLD 750

// ultrasonic sensor 
#define TRIG_PIN 13  
#define ECHO_PIN 12 

// infrared sensor (from 0 to 1024) analog write
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

#define BUILTIN_LED 13

// ultrasonic sensor 
#define MAX_DIST_OBSTACLE 10.0

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
long aux_time = 0;

int left_ir, middle_ir, right_ir, message, prev_time, line, count, right_vel,
 left_vel, line_lost_sent, obstacle_detected_sent, line_last_seen;

//.//.//.//.//.//. PID //.//.//.//.//.//.

const float Kp = 0.1;
const float Kd = 20;

float p_error = 0, d_error = 0, PD = 0;
float error = 0, previous_error = 0;

//.//.//.//.//.//.//.//.//.//.//.//.//.//

void send_message(){
    TickType_t xLastWaskeTime;

    while(1){
        xLastWaskeTime = xTaskGetTickCount();

        message = 0;
        if (!is_line && !line_lost_sent){
            Serial.print(LINE_LOST);
            Serial.print(INIT_LINE_SEARCH);
            line_lost_sent = 1;
        }

        if (line_lost_sent && is_line){
            line_lost_sent = 0;
            Serial.print(LINE_FOUND);
            Serial.print(STOP_LINE_SEARCH);
        }
      
        xTaskDelayUntil(&xLastWaskeTime, 10);
    }    
}

void get_infrared(){
    TickType_t xLastWaskeTime;
    while(1){
        xLastWaskeTime = xTaskGetTickCount();
        left_ir = analogRead(PIN_ITR20001_LEFT);
        middle_ir = analogRead(PIN_ITR20001_MIDDLE);
        right_ir = analogRead(PIN_ITR20001_RIGHT);

        is_line = true;

        // No sensors is on the line
        if (left_ir < IR_L_THRESHOLD && middle_ir < IR_M_THRESHOLD && right_ir < IR_R_THRESHOLD){
            is_line = false;
            line = NO_LINE;
            error = 0;
            previous_error = 0;
        }
        // CENTER + RIGHT
        else if (left_ir < IR_L_THRESHOLD && middle_ir >= IR_M_THRESHOLD && right_ir >= IR_R_THRESHOLD){
            error += 1;
            line = LINE_MIDRIGHT;
        }
        // RIGHT
        else if (left_ir < IR_L_THRESHOLD && middle_ir < IR_M_THRESHOLD && right_ir >= IR_R_THRESHOLD){
            error += 2;
            line_last_seen = LINE_RIGHT;
            line = LINE_RIGHT;
        }
        // CENTER + LEFT
        else if (left_ir >= IR_L_THRESHOLD && middle_ir >= IR_M_THRESHOLD && right_ir < IR_R_THRESHOLD){
            error += -1;
            line = LINE_MIDLEFT;
        }
        // LEFT
        else if (left_ir >= IR_L_THRESHOLD && middle_ir < IR_M_THRESHOLD && right_ir < IR_R_THRESHOLD){
            error += -2;
            line_last_seen = LINE_LEFT;
            line = LINE_LEFT;
        }
        // CENTER
        else {
            error = 0;
            previous_error = 0;
            line_last_seen = LINE_MID;
            line = LINE_MID;
        }
        xTaskDelayUntil(&xLastWaskeTime, 1);
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

    return distance;
}

void is_obstacle(){
    TickType_t xLastWaskeTime;
    while(1){

        xLastWaskeTime = xTaskGetTickCount();
        float distance_sensor = get_distance();
    
        if (distance_sensor < MAX_DIST_OBSTACLE && distance_sensor != 0){

            detected_obstacle = true;
            if(detected_obstacle && !obstacle_detected_sent){    
                Serial.print(OBSTACLE_DETECTED);
                Serial.print(END_LAP);
                obstacle_detected_sent = 1;
                left_vel = 0;
                right_vel = 0;
            }
        }

        xTaskDelayUntil(&xLastWaskeTime, 1);
    }
}

void command_motors(){
  
    TickType_t xLastWaskeTime;
    while(1){

        //Serial.println("COMMANDING MOTORS");
        xLastWaskeTime = xTaskGetTickCount();
        // set high each time we use them 
        digitalWrite(PIN_Motor_AIN_1, HIGH);
        digitalWrite(PIN_Motor_BIN_1, HIGH);
      
        p_error = error;
        d_error = error - previous_error;      

        PD = (Kp * p_error) + (Kd * d_error);

        right_vel = STD_VELOCITY - PD;
        left_vel = STD_VELOCITY + PD;

        if (right_vel > MAX_VELOCITY){
            right_vel = MAX_VELOCITY;
        }
        if (right_vel < MIN_VELOCITY){
            right_vel = MIN_VELOCITY;
        }
        if (left_vel > MAX_VELOCITY){
            left_vel = MAX_VELOCITY;
        }
        if (left_vel < MIN_VELOCITY){
            left_vel = MIN_VELOCITY;
        }

        if (line == NO_LINE && line_last_seen == LINE_LEFT){
            // TURN RIGHT
            left_vel = LOST_LINE_SECOND_VEL;
            right_vel = LOST_LINE_VEL;
        } else if (line == NO_LINE && line_last_seen == LINE_RIGHT){
          // TURN LEFT
            left_vel = LOST_LINE_VEL;
            right_vel = LOST_LINE_SECOND_VEL;
        }

        if (detected_obstacle){
            left_vel = 0;
            right_vel = 0;
        }

      // right 
      analogWrite(PIN_Motor_PWMA, right_vel);
      // left
      analogWrite(PIN_Motor_PWMB, left_vel); 

      previous_error = error;
        
      xTaskDelayUntil(&xLastWaskeTime, 1);
  }   
}

uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
    return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

void setup() {

    FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
    FastLED.setBrightness(0);
    FastLED.showColor(Color(0, 0, 0));
    FastLED.setBrightness(100);
    // Arduino UNO has one serial only.
    Serial.begin(9600); 

    // wait until ESP send info to the robot 
    // to say it is connected succesfully
    while(1){
        if (Serial.available()){
            Serial.println(Serial.read());
            break;
        }
    }
    FastLED.showColor(Color(0, 255, 0));

    pinMode(PIN_Motor_STBY, OUTPUT);
    pinMode(PIN_Motor_AIN_1, OUTPUT);
    pinMode(PIN_Motor_PWMA, OUTPUT);
    pinMode(PIN_Motor_BIN_1, OUTPUT);
    pinMode(PIN_Motor_PWMB, OUTPUT);
            
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Enables motor control
    digitalWrite(PIN_Motor_STBY, HIGH);

    xTaskCreate(is_obstacle, "is_obstacle", 100, NULL, 1, NULL);
    xTaskCreate(get_infrared, "get_infrared", 100, NULL, 2, NULL);
    xTaskCreate(send_message, "send_message", 100, NULL, 0, NULL);
    xTaskCreate(command_motors, "command_motors", 100, NULL, 2, NULL);

}

void loop() {
}