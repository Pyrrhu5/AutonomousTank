#include <ArduinoSTL.h>
#include <Arduino.h>
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
		void begin(int servoPin, int echoPin, int triggerPin, float minDistance, int precision=45){
			// Servo Motor
			pinMode(servoPin, OUTPUT);
			this->servo.attach(servoPin);
			// Reset the servo in the middle position
			this->servo.write(90);

			// Ultrasonic sensor
			this->echoPin = echoPin;
			pinMode(this->echoPin, INPUT);
			this->triggerPin = triggerPin;
			pinMode(this->triggerPin, OUTPUT);
			digitalWrite(this->triggerPin, LOW);
			
			// Servo config
			this->minDistance = minDistance;
			this->precision = precision;
			this->nRotation = round((180/precision) + 1);
			// Results array initialization
			this->scanData = new AngleData[this->nRotation];
		}

		/* Get the distance of the nearest object in meters, max is 0.75m */
		float get_distance(){
			digitalWrite(this->triggerPin, LOW);
			delayMicroseconds(5);
			digitalWrite(this->triggerPin, HIGH);
			delayMicroseconds(15);
			digitalWrite(this->triggerPin, LOW);

			return (pulseIn(this->echoPin, HIGH)) * 0.0001657; // nice number to convert to m
		}

		/* Performs a 180deg scan by precision<unsigned int> degrees increments
			and return a pointer to the array of results from 0deg to 180deg.
			In meters.
		*/
		void scan(){
			int currentAngle = 0;
			/* struct AngleData scanData[nIter]; */
			/* float *scanData = new float[nIter]; */
			// The first rotation takes more time
			this->set_angle(currentAngle);
			// TODO dynamic speed: 24 ms per 60 deg
			delay(100);

			for (int i = 0; i < this->nRotation; ++i){
				AngleData data;
				this->scanData[i] = {
					currentAngle,
					this->angle_to_vehicule_angle(currentAngle),
					this->get_distance()
				};

				currentAngle += this->precision;
				this->set_angle(currentAngle);
				delay(75);
			}
			// Reset the servo straight ahead
			this->set_angle(90);

		}

		/* Performs a 180deg scan by precision<unsigned int> degrees increments
 			and order the result from the longest to the smallest distances.
 		*/
		int best_angle(){
			this->scan();
			std::sort(this->scanData, this->scanData + nRotation, this->scanData->compare);
		}

		/* Performs a 180deg scan by precision<unsigned int> degrees increments
 			and order the result from the longest to the smallest distances
			closest to the 90 deg (straight) position.
 		*/
		void acceptable_angle(){
			/* float *scanData = this->scan(precision); */
			this->scan();
			// Index of the 90 deg position, middle of the array
			int middleIndex = (this->nRotation - 1) / 2;

			// Calculates a weighted distance, the closer to middleIndex, the higher the weight
			// so an array[5] will be {1, 2, 3, 2, 1}*distance
			for (int i = this->nRotation; i >= 0; --i){
				// Pop the value if it's not in acceptable distance
				if (this->scanData[i].distance < this->minDistance){
					this->scanData[i].weightedDistance = -1;
					continue;
				}

				// Middle to beginning of the array
				if (i <= middleIndex){
					this->scanData[i].weightedDistance = (i + 1) * this->scanData[i].distance;
				} else {
					// end to middle of the array
					this->scanData[i].weightedDistance = (nRotation - i) * this->scanData[i].distance;
				}
			}
	
			std::sort(this->scanData, this->scanData + nRotation, this->scanData->compareWeighted);
		}

	private:
		/* Set the angle of the servo motor from 0 deg (left) to 180 (right) */
		void set_angle(int degree){
			this->servo.write(degree);
		}

		/* Convert the servo angle (0 to 180 deg) to vehicule angle
 			(-90 to 90) or -1
		*/
		int angle_to_vehicule_angle(int angle){
			if (angle == -1) return -1;
			return map(angle, 0, 180, -90, 90);
		}

};
