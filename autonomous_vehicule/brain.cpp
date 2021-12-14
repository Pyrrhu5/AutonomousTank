#ifndef Vehicule
	#include "vehicule.cpp"
#endif
#ifndef ObstacleDetector
	#include "obstacle_detector.cpp"
#endif


class Brain{

	private:
		Vehicule *vehicule;
		ObstacleDetector *avoidance;
		float decisionSensibility;

	public:
		Brain(Vehicule &vehicule, ObstacleDetector &avoidance, float decisionSensibility){
			this->vehicule = &vehicule;
			this->avoidance = &avoidance;
			this->decisionSensibility = decisionSensibility;
		}

		// TODO Needs to drive only for available meters, then take a decision
		// TODO Cache previous decisions to not repete them
		/* Continously drives, straight if possible, while avoiding obstacles */
		void drive(int speed=100){
			this->vehicule->set_speed(speed);

			// At start, choose the best direction
			this->avoidance->best_angle();
			AngleData *choice = &this->avoidance->scanData[0];
						
			while (true){
				if (choice->vehiculeAngle == 0 && choice->distance < decisionSensibility){
					Serial.println("Decision to move forward for " + String(choice->distance/2) + "m");
					this->vehicule->move(choice->distance/2.0);
				} else if (choice->vehiculeAngle == 0){
					Serial.println("Decision to move forward for " + String(decisionSensibility) + "m (overwritten by decisionSensibility)");
					this->vehicule->move(decisionSensibility);
				} else {
					Serial.println("Decision to turn to " + String(choice->vehiculeAngle) + " degrees");
					this->vehicule->turn(choice->vehiculeAngle);
				}
				while (!this->vehicule->has_reached_destination()){}
				this->avoidance->acceptable_angle();
				choice = &this->avoidance->scanData[0];
			}
		}
};
