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
			int direction = this->avoidance->best_angle();
			direction = this->avoidance->angle_to_vehicule_angle(direction);
						
			while (true){
				// turn back, all direction blocked
				if (direction == -1){
					this->vehicule->move(-1 * (this->decisionSensibility / 2));
					while (!this->vehicule->has_reached_destination()){}
					int direction = this->avoidance->best_angle();
					direction = this->avoidance->angle_to_vehicule_angle(direction);
					continue;
				// turn, front blocked
				} else if (direction != 0){ 
					this->vehicule->turn(direction);
				// continue straight
				} else {
					this->vehicule->move(this->decisionSensibility);
				}
	
				while (!this->vehicule->has_reached_destination()){}
				direction = this->avoidance->acceptable_angle();
				direction = this->avoidance->angle_to_vehicule_angle(direction);
			}
		}
};
