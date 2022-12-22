# SussusAmogus
Práctica 4 - Sistemas empotrados y de tiempo real. Robot siguelineas.

## ROBOT: ELEGOO SMART ROBOT CAR KIT V.4.0


### INSTRUCCIONES: 

1) En la carpeta subida, es importante leer el pdf que aparece dentro de **01 ReadMeFirst**, en él aparece las librerías (./ELEGOO Smart/02 Manual/02 Main) a importar para cuando usar el giroscopio, en nuestro caso tenemos: **MPU6050 gyro module**

2) **02 Manual & Main Code**: en apartado 1 tienes el manual por si quieres revisar algo.
En el apartado 2 tienes las librerías descrito en 1).
En el apartado 4 tienes un codigo de conxión del ESP + cámara.

3) **03 Tutorial & Code** tienes muchos ejemplos programados en arduino 
4) **04 Related chip information** son datasheets


### CONEXIONES SENSORES/ACTUADORES

Tras desgranar del 3) y hacer testing, estos son los valores y conexiones de cada sensor/actuador:

- **Sensor ultrasonico**
```
// ultrasonic sensor 
#define TRIG_PIN 13  
#define ECHO_PIN 12 
```

- **Módulo seguimiento de línea**

```
// infrared sensor ( de 0 a 1024 ) analog write
#define PIN_ITR20001-LEFT   A2
#define PIN_ITR20001-MIDDLE A1
#define PIN_ITR20001-RIGHT  A0
```

- **Motores:**
```
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
```

IMPORTANTE! Cada vez que se mande una velocidad el motor hay que mandar high


VERSION 2 WORKING: 

STD_VEL = 120

LOST_LINE = 120

IR_R = 650

IR_M = 450

IR_L = 750

Kp = 0.1

Kd = 20


Time obtained: 10.1

VERSION 2 WORKING: 

STD_VEL = 175

LOST_LINE = 150 

IR_R = 650

IR_M = 450

IR_L = 750

Kp = 1

Kd = 25

Time obtained: 8.6


VERSION 3 WORKING: 

STD_VEL = 175

LOST_LINE = 175

IR_R = 650

IR_M = 450

IR_L = 750

Kp = 1.5

Kd = 20

Time obtained: 8.1


## ESTRUCTURA DE LA PRÁCTICA

### - ESP32:
  **PASOS A SEGUIR:** 
  
  1) Conectarse a la red WIFI "sensoresurjc"
  2) Conectarse al broker MQTT (Mosquitto) 
  3) Leer los mensajes del arduino 
  4) Publicar en el topic mensajes leidos en 3.

  1. y 2. se hacen en el set-up. 3. y 4. en el loop

  **MENSAJES QUE MOSTRAR:**
  
  [Enlace a la wiki](https://gitlab.etsit.urjc.es/roberto.calvo/setr/-/wikis/P4FollowLine#62-mensajes)
  

### - ROBOT SIGUELÍNEAS: 

  **FUNCIONES:**
  
