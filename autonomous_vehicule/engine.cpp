#include <Arduino.h>
#include "engine.h"

void Engine::begin(
	int forwardPin,
	int backwardPin,
	int speedPin,
	int speedMin = 100,
	int speedMax = 255
){
	this->forwardPin = forwardPin;
	pinMode(this->forwardPin, OUTPUT);
	this->backwardPin = backwardPin;
	pinMode(this->backwardPin, OUTPUT);
	this->speedPin = speedPin;
	pinMode(this->speedPin, OUTPUT);

	this->stop();
	analogWrite(this->speedPin, 0);

	this->speedMin = speedMin;
	this->speedMax = speedMax;
}

void Engine::increment_tick(){
	this->tick++;
}

void Engine::set_speed_percent(unsigned int percent){
	this->speed = map(percent, 0, 100, this->speedMin, this->speedMax);
	analogWrite(this->speedPin, this->speed);
}

void Engine::forward(){
	digitalWrite(this->forwardPin, 1);
	digitalWrite(this->backwardPin, 0);
}

void Engine::backward(){
	digitalWrite(this->forwardPin, 0);
	digitalWrite(this->backwardPin, 1);

}

void Engine::stop(){
	digitalWrite(this->forwardPin, 0);
	digitalWrite(this->backwardPin, 0);
}
