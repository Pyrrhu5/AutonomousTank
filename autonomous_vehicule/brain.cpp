#include <Arduino.h>
#include "logger.h"
#include "brain.h"


Brain::Brain(Vehicule &vehicule, ObstacleDetector &avoidance, float decisionSensibility){
	this->vehicule = &vehicule;
	this->avoidance = &avoidance;
	this->decisionSensibility = decisionSensibility;
}

void Brain::drive(int speed=100){
	this->vehicule->set_speed(speed);

	// At start, choose the best direction
	this->avoidance->best_angle();
	AngleData *choice = &this->avoidance->scanData[0];
				
	while (true){
		if (choice->vehiculeAngle == 0 && choice->distance < decisionSensibility){
			LOG("Decision to move forward for " + String(choice->distance/2) + "m", "DECISION");
			this->vehicule->move(choice->distance/2.0);
		} else if (choice->vehiculeAngle == 0){
			LOG("Decision to move forward for " + String(decisionSensibility) + "m (overwritten by decisionSensibility)", "DECISION");
			this->vehicule->move(decisionSensibility);
		} else {
			LOG("Decision to turn to " + String(choice->vehiculeAngle) + " degrees", "DECISION");
			this->vehicule->turn(choice->vehiculeAngle);
		}
		while (!this->vehicule->has_reached_destination()){}
		this->avoidance->acceptable_angle();
		choice = &this->avoidance->scanData[0];
	}
}
