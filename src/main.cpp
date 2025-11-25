#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Ultrasonic.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Ultrasonic ultrasonic1(12, 13);
Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(9600);

  dht.begin();
  pinMode(DHTPIN, INPUT);

  Wire.begin(21, 22); 
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);  
}

void loop() {      

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');

    cmd.trim(); 

    if (cmd == "data") {          
      
      if(mpu.getMotionInterruptStatus()) {
        /* Get new sensor events with the readings */
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        Serial.println(
          String(dht.readTemperature()) + ':' +
          String(dht.readHumidity())    + ':' +
          String(ultrasonic1.read())    + ":" +
          String(a.acceleration.x)      + "," +
          String(a.acceleration.y)
        );
      }

      delay(10);      
    }
  }
}



