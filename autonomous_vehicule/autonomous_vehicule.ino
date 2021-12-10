#include <Arduino.h>
#include "vehicule.cpp"
#include "obstacle_detector.cpp"
/* #include <Servo.h> */

Engine right;
const int encoderRightPin = 3;
Engine left;
const int encoderLeftPin = 2;
Vehicule tank;
ObstacleDetector avoidance;

/* Interrupts require static function outside of classes... */
void encoder_right(){
	right.set_rotation_tick();
}

void encoder_left(){
	left.set_rotation_tick();
}

void setup() {
	Serial.begin(9600);

	// Set the attached pins for the ticks sensor (tracking or hall sensor)
	// Being an interrupt, the called function cannot be dynamic (class method)
	pinMode(encoderRightPin, INPUT);
	pinMode(encoderLeftPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(encoderRightPin), encoder_right, FALLING);
	attachInterrupt(digitalPinToInterrupt(encoderLeftPin), encoder_left, FALLING);

	right.begin(12, 10, 6);
	left.begin(9, 8, 5);
	avoidance.begin(11, A3, A2);
	tank.begin(right, left, 0.019, 30);

	tank.set_speed(100);
}

void loop() {
	Serial.println(avoidance.get_distance());
	delay(2000);
}

