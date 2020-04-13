//////////////////////////////////////////////////////
/*
 * Libraries
 */
//////////////////////////////////////////////////////
#include <Arduino_FreeRTOS.h>
#include <Servo.h>

//////////////////////////////////////////////////////
/*
 * defines arduino pins numbers
 */
//////////////////////////////////////////////////////
const int trigPin = 12;
const int echoPin = 11;
const int servoPin = 9;

//////////////////////////////////////////////////////
/*
 * defines variables
 */
//////////////////////////////////////////////////////
long duration;  
int distance; //Formula: Distance = (Speed of sound × duration)/2
Servo myservo;  //Servo object 
TaskHandle_t xHandle ;

//////////////////////////////////////////////////////
/*
 * defines tasks prototype
 */
//////////////////////////////////////////////////////
void TaskUltrasonicSensor( void *pvParameters );
void TaskServoMotor( void *pvParameters );

//////////////////////////////////////////////////////
/*
 * setup
 */
//////////////////////////////////////////////////////
void setup(){

  //Serial.begin(9600);

  //Servo motor is attached to pin 9
  myservo.attach(servoPin);  
  myservo.write(0); //initialize servo motor
  
  // Now set up two Tasks to run independently.
  xTaskCreate(
    TaskUltrasonicSensor
    ,  "Ultrasonic Sensor"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &xHandle );

  xTaskCreate(
    TaskServoMotor
    ,  "Servo Motor"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );
 
}

//////////////////////////////////////////////////////
/*
 * loop
 */
//////////////////////////////////////////////////////
void loop(){}

//////////////////////////////////////////////////////
/*
 * Task Ultrasonic Sensor
 */
//////////////////////////////////////////////////////
void TaskUltrasonicSensor(void *pvParameters){
  (void) pvParameters;

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  for (;;) // A Task shall never return or exit.
  {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;

    /*
    Serial.print("Distance from the object = ");
    Serial.print(distance);
    Serial.println(" cm");
    */
    
    if(distance > 0 && distance<10){
      //suspend the task itself
      vTaskSuspend (NULL);    
    }else {
      //Do nothing
      myservo.write(0);
    }
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

//////////////////////////////////////////////////////
/*
 * Task Servo Motor
 */
//////////////////////////////////////////////////////
void TaskServoMotor(void *pvParameters){
  (void) pvParameters;

  //Serial.println(" Execute...");

  BaseType_t xYieldRequired;

  //push the pump
  myservo.write(90);
 
  //Resume the suspended task
  xYieldRequired = xTaskResumeFromISR(xHandle) ;
  if( xYieldRequired == pdTRUE ){
    taskYIELD();
  }
  
  vTaskDelay( 1000 / portTICK_PERIOD_MS ); 

}
