/* #include <Vector.h> */
#include <Arduino.h>
#include <Servo.h>

class ObstacleDetector {
	private:
		int echoPin;
		int triggerPin;
		Servo servo;

	public:
		void begin(int servoPin, int echoPin, int triggerPin){
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
		}

		void set_angle(int degree){
			this->servo.write(degree);
		}


		/* Get the distance of the nearest object in meters, max is 0.75m */
		float get_distance(){
			digitalWrite(this->triggerPin, LOW);
			delayMicroseconds(5);
			digitalWrite(this->triggerPin, HIGH);
			delayMicroseconds(15);
			digitalWrite(this->triggerPin, LOW);

			return (pulseIn(this->echoPin, HIGH)) * 0.0001657;
		}


		/* Performs a 180deg scan by precision<unsigned int> degrees increments
			And return a pointer to the array of results from 0deg to 180deg 
		*/
		float *scan(unsigned int precision=45){
			int nIter = (180/precision) + 1;
			int currentAngle = 0;
			float *scanData = new float[nIter];

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

};
