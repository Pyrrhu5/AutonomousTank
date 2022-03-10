#include <Arduino.h>
#include "logger.h"
#include "engine.h"
#include "vehicule.h"
#include "obstacle_detector.h"
#include "brain.h"

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

// TODO: going to far when going straight
// TODO: brain decision sensibility should be relative to speed
// TODO: avoid doing twice the same decision (except straight)
// TODO: Vehicule acceleration and deceleration
// TODO: Vehicule pid control
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
	avoidance.begin(11, A3, A2, 0.35, 30);
	tank.begin(right, left, 0.0275, 13.75);
	Brain brain(tank, avoidance, 0.7);
	LOG("Ready", "INFO");
	brain.drive();
}

void loop() {
}

