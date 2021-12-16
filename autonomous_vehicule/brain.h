#ifndef BRAIN_H
#define BRAIN_H

#include "vehicule.h"
#include "obstacle_detector.h"

class Brain{
	private:
		Vehicule *vehicule;
		ObstacleDetector *avoidance;
		float decisionSensibility;

	public:
		Brain(Vehicule &vehicule, ObstacleDetector &avoidance, float decisionSensibility);
		/* Continously drives, straight if possible, while avoiding obstacles */
		void drive(int speed=100);

};
#endif
