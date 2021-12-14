#include <Arduino.h>

/* 
	Class to represent one motor used in moving and steering
	the vehicule.

	The constructor has for arguments:
		forwardPin <int> : the digital pin to put HIGH to turn forward
		backwardPin <int>: the digital pin to put HIGH to turn backward
		speedPin <int>: the digital PWM pin to set the rotation speed
		speedMin <int>: the minimum speed the motor can turn at (from 0 to 255)
		speedMax <int>: the maximum speed the motor can turn at (from 0 to 255)
*/
class Engine {
	private: 
		// Pins definition
		int forwardPin;
		int backwardPin;
		int speedPin;
		int encoderPin;
		double lastTick = 0;
	public:
		int speed = 0;
		int speedMax;
		int speedMin;
		volatile unsigned int tick=0;

	/* Constructor: set up the pins and stop the motor */
	public:
		void begin(
			int forwardPin,
			int backwardPin,
			int speedPin,
			int speedMin = 100,
			int speedMax = 255
		){
			this->forwardPin = forwardPin;
			pinMode(this->forwardPin, OUTPUT);
			this->backwardPin = backwardPin;
			pinMode(this->backwardPin, OUTPUT);
			this->speedPin = speedPin;
			pinMode(this->speedPin, OUTPUT);

			// Stop the motor
			stop();
			analogWrite(this->speedPin, 0);

			this->speedMin = speedMin;
			this->speedMax = speedMax;
		}


		void increment_tick(){
			this->tick++;
		}

		/* Set the speed of rotation from a range of 0% to 100% */
		void set_speed_percent(unsigned int percent){
			this->speed = map(percent, 0, 100, this->speedMin, this->speedMax);
			analogWrite(this->speedPin, this->speed);
		}

		/* Turn the motor forward */
		void forward(){
			digitalWrite(this->forwardPin, 1);
			digitalWrite(this->backwardPin, 0);
		}

		/* Turn the motor backward */
		void backward(){
			digitalWrite(this->forwardPin, 0);
			digitalWrite(this->backwardPin, 1);

		}
		/* Stop the motor */
		void stop(){
			digitalWrite(this->forwardPin, 0);
			digitalWrite(this->backwardPin, 0);
		}
};
