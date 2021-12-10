#include <Arduino.h>
#include <Servo.h>

class ObstacleDetector {
	private:
		int echoPin;
		int triggerPin;
		Servo servo;
		static const int nAnglesToCheck = 5;
		static const int anglesToCheck[nAnglesToCheck] = {90, 135, 180, 0, 45};
	public:
		float angleDistances[nAnglesToCheck];

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

		void scan(){

			for (int i=0; i<nAnglesToCheck; i++){
				this->set_angle(this->anglesToCheck[i]);
				this->angleDistances[i] = this->get_distance();
				delay(50);
			}
			this->set_angle(90);
		}

};
