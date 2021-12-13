#include <Arduino.h>
#include <Servo.h>

class ObstacleDetector {
	private:
		int echoPin;
		int triggerPin;
		Servo servo;
		float minDistance;
		float maxDistance;

	public:
		void begin(int servoPin, int echoPin, int triggerPin, float minDistance, float maxDistance){
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
			this->minDistance = minDistance;
			this->maxDistance = maxDistance;
		}

	private:
		/* Set the angle of the servo motor from 0 deg (left) to 180 (right) */
		void set_angle(int degree){
			this->servo.write(degree);
		}

	public:
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
		float *scan(unsigned int precision=45){
			int nIter = (180/precision) + 1;
			int currentAngle = 0;
			float *scanData = new float[nIter];
			// The first rotation takes more time
			this->set_angle(currentAngle);
			delay(100);

			for (int i = 0; i < nIter; ++i){
				this->set_angle(currentAngle);
				delay(75);
				scanData[i] = this->get_distance();
				currentAngle += precision;
			}
			this->set_angle(90);

			return scanData;
		}


		/* Performs a 180deg scan by precision<unsigned int> degrees increments
			and output the angle which have the most free distance, and is the 
			closest to 90deg (front)
 		*/
		int best_angle(unsigned int precision=45){
			float *scanData = this->scan(precision);
			int nElement = (180/precision) + 1;
			float maxDist = 0;
			int bestAngle;

			// find the max distance scanned
			for (int i = 0; i < nElement; i++) {
				if(scanData[i] > maxDist){
					maxDist = scanData[i];
					// recalculate the angle
					bestAngle = i*precision;
				// for the angle before 90, the lowest value is the best
				// but it's iterated after a higher value
				// given scan() returning {0, 45, 90, 135, 180}
				} else if (scanData[i] == maxDist && (i*precision) < 90){
					// recalculate the angle
					bestAngle = i * precision;
				}
			}

			return bestAngle;
		}

		/* Performs a 180deg scan by precision<unsigned int> degrees increments
			and output the angle which is the closest to 90 deg and is above 
			the minimum distance.
			Returns -1 if no acceptable angle has been found;
 		*/
		int acceptable_angle(unsigned int precision = 45){
			float *scanData = this->scan(precision);
			int arraySize = (180 / precision) + 1;
			int middleIndex = (arraySize - 1) / 2;

			// If there is room to move straight, let's go straight
			if (scanData[middleIndex] > this->minDistance) return 90;

			int bestIndex = -1;
			// Minimum acceptable index from 0 to middle
			for (int i = (middleIndex - 1); i >= 0; i--){
				if (scanData[i] > this->minDistance) {
					bestIndex = i;
				}
			}
			// Minimum acceptable index from middle to arraySize
			for (int i = (middleIndex + 1); i < arraySize; i++){
				// Take the index only if higher than the previous bestIndex
				if (scanData[i] > this->minDistance && (arraySize - 1 - i) > bestIndex){
					return i * precision;
				}
			}
			// Minimum acceptable angle is from 0 to middle
			// or none
			if (bestIndex != -1) return bestIndex * precision;
			else return -1;
		}

		/* Convert the servo angle (0 to 180 deg) to vehicule angle
 			(-90 to 90) or -1
		*/
		int angle_to_vehicule_angle(int angle){
			if (angle == -1) return -1;
			return map(angle, 0, 180, -90, 90);
		}

};
