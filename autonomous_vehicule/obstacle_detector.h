#ifndef OBSTACLE_DETECTOR_H
#define OBSTACLE_DETECTOR_H
#include <Servo.h>

// Type to hold scan data
struct AngleData {
	int servoAngle;
	int vehiculeAngle;
	float distance;
	// Distance * coef of distance from the middle index
	// the closest to 90, the higher the weight should be
	float weightedDistance;
	// Comparaison lambdas to sort the array
	static bool compare(const AngleData& a, const AngleData& b){
		return a.distance > b.distance;
	}
	static bool compareWeighted(const AngleData& a, const AngleData& b){ 
		return a.weightedDistance > b.weightedDistance;
	}
};

class ObstacleDetector {
	private:
		int echoPin;
		int triggerPin;
		Servo servo;
		int servoRPM;
		// Minimum distance for a direction to be acceptable
		float minDistance;
		// Distance in degrees between two scan points
		int precision;
		// Number of rotation in a scan
		int nRotation;
	public:
		// Array of results from a scan
		AngleData* scanData;
	public:
		void begin(
			int servoPin,
			int echoPin,
			int triggerPin,
			float minDistance,
			int precision=45,
			int servoRPM=120
		);
		/* Get the distance of the nearest object in meters, max is 0.75m */
		float get_distance();
		/* 
 			Performs a 180deg scan by precision<unsigned int> degrees increments
			and return a pointer to the array of results from 0deg to 180deg.
			In meters.
		*/
		void scan();
		/* 
 			Performs a 180deg scan by precision<unsigned int> degrees increments
 			and order the result from the longest to the smallest distances.
 		*/
		int best_angle();
		/* 
 			Performs a 180deg scan by precision<unsigned int> degrees increments
 			and order the result from the longest to the smallest distances
			closest to the 90 deg (straight) position.
 		*/
		void acceptable_angle();
	private:
		/* Set the angle of the servo motor from 0 deg (left) to 180 (right) */
		void set_angle(int degree);
		/* 
  			Convert the servo angle (0 to 180 deg) to vehicule angle
 			(-90 to 90) or -1
		*/
		int angle_to_vehicule_angle(int angle);
};

#endif
