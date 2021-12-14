#include <Arduino.h>
#ifndef Brain
	#include "brain.cpp"
#endif

Engine right;
const int encoderRightPin = 3;
Engine left;
const int encoderLeftPin = 2;
Vehicule tank;
ObstacleDetector avoidance;

/* Interrupts require static function outside of classes... */
void encoder_right(){
	right.increment_tick();
}

void encoder_left(){
	left.increment_tick();
}

void setup() {
	Serial.begin(9600);

	// Set the attached pins for the ticks sensor (tracking or hall sensor)
	// Being an interrupt, the called function cannot be dynamic (class method)
	pinMode(encoderRightPin, INPUT);
	pinMode(encoderLeftPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(encoderRightPin), encoder_right, RISING);
	attachInterrupt(digitalPinToInterrupt(encoderLeftPin), encoder_left, RISING);

	right.begin(12, 10, 6);
	left.begin(9, 8, 5);
	avoidance.begin(11, A3, A2, 0.25);
	tank.begin(right, left, 0.0275, 13.75);
	Brain brain(tank, avoidance, 0.1);
	brain.drive();
}

void loop() {
}

