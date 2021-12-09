#include <Arduino.h>
#include "vehicule.cpp"

Engine right = Engine(12,10,6);
const int encoderRightPin = 3;
Engine left = Engine(9, 8, 5);
const int encoderLeftPin = 2;
Vehicule tank(right, left, 0.019, 30);

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

	tank.set_speed(100);
}

void loop() {
	tank.move(1);
	tank.stop();
	delay(5000);
}

