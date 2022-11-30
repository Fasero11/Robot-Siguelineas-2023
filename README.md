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


### CONEXIÓN DE CADA SENSOR & ACTUADOR

Tras desgranar del 3) y hacer testing, estos son los valores y conexiones de cada sensor/actuador:

- **Sensor ultrasonico **
- **Módulo seguimiento de línea **
- **ESP32**
- **Motores:  **

delantero izquierda:
delantero derecha:
trasero izquierda:
trasero derecha:







## ESTRUCTURA DE LA PRÁCTICA

### - ESP32:
  **PASOS A SEGUIR:** 
  
  1) Conectarse a la red WIFI "sensoresurjc"
  2) Conectarse al broker MQTT (Mosquitto) 
  3) Leer los mensajes del arduino 
  4) Publicar en el topic mensajes leidos en 3.

  1. y 2. se hacen en el septup. 3. y 4. en el loop

  **MENSAJES QUE MOSTRAR:**
  
  [Enlace a la wiki](https://gitlab.etsit.urjc.es/roberto.calvo/setr/-/wikis/P4FollowLine#62-mensajes)
  

### - ROBOT SIGUELÍNEAS: 

  **FUNCIONES:**
  
