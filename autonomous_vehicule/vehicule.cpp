#include <Arduino.h>
#include "vehicule.h"

void Vehicule::begin(Engine &engineRight, Engine &engineLeft, float meterPerTick, float degreePerTick){ 
	this->engines[0] = &engineRight;
	this->engines[1] = &engineLeft;
	this->meterPerTick = meterPerTick;
	this->degreePerTick = degreePerTick;
}


void Vehicule::set_speed(unsigned int speed){
	for (int i=0; i<2; i++){
		this->engines[i]->set_speed_percent(speed);
	}
}

void Vehicule::move(float meters){
	this->reset_tick();
	for (int i=0; i<2; i++){
		if (meters > 0) this->engines[i]->forward();
		else this->engines[i]->backward();
	}
	this->tickTarget = round(abs(meters)/this->meterPerTick);
}

void Vehicule::turn(int degree, float sharpness = 1){
	this->reset_tick();
	if (degree < 1){
		this->engines[0]->backward();
		this->engines[1]->forward();
	} else {
		this->engines[1]->backward();
		this->engines[0]->forward();
	}
	this->tickTarget = round(abs(degree)/this->degreePerTick);
}

void Vehicule::stop(){
	for (int i=0; i<2; i++){
		this->engines[i]->stop();
	}
}

bool Vehicule::has_reached_destination(){
	return this->engines[0]->tick >= this->tickTarget;
}

void Vehicule::test_forward(unsigned int tick, unsigned int speed=100){
	this->reset_tick();
	this->set_speed(speed);
	this->tickTarget = tick;
	for (int i=0; i<2; i++){
		this->engines[i]->forward();
	}
	while (!this->has_reached_destination()){;}
	this->stop();
}

void Vehicule::test_turn(unsigned int tick, unsigned int speed=100){
	this->reset_tick();
	this->set_speed(speed);
	this->tickTarget = tick;
	this->engines[0]->forward();
	this->engines[1]->backward();
	while (!this->has_reached_destination()){;}
	this->stop();
}

void Vehicule::reset_tick(){
	for (int i=0; i<2; i++){
		this->engines[i]->tick = 0;
	}
}
