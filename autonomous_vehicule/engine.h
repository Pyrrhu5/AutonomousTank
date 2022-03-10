#ifndef ENGINE_H
#define ENGINE_H

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
class Engine{
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

	public:
		void begin(
			int forwardPin,
			int backwardPin,
			int speedPin,
			int speedMin = 100,
			int speedMax = 255
		);

		void increment_tick();
		/* Set the speed of rotation from a range of 0% to 100% */
		void set_speed_percent(unsigned int percent);
		/* Turn the motor forward */
		void forward();
		/* Turn the motor backward */
		void backward();
		/* Stop the motor */
		void stop();
};

#endif
