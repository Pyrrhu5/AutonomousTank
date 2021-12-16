
#ifndef VEHICULE_H
#define VEHICULE_H
#include "engine.h"

class Vehicule{
	public:
		Engine *engines[2];
		float meterPerTick;
		float degreePerTick;
	public:
		unsigned int tickTarget=0;

	public:
		void begin(
			Engine &engineRight,
			Engine &engineLeft,
			float meterPerTick,
			float degreePerTick
		);

		/* 
			Set the speed of the vehicule in percentage 
			<unsigned int> speed of its minimum to maximum speed
		*/
		void set_speed(unsigned int speed);
		/*
			Moves forward or backwards for a number of 
			<float> meters.
			The direction of travel is determined by the
			sign of meters (negative == backward).
			Does not stop after reaching the destination.
		*/
		void move(float meters);
		/*
			Turns right or left for a number of
			<float> degrees.
			The direction of turning is determined by the
			sign of degrees (negative == left)
			Does not stop after reaching the destination.
		*/
		void turn(int degree, float sharpness = 1);
		/* Stop the vehicule */
		void stop();
		/*
			Determin if the vehicule has reached its set destination.
			The destination is calculated by comparing the engines current
			tick with this->tickTarget.
			The tickTarget is set by turn and move methods
		*/
		bool has_reached_destination();
		/* 
			Test function where the vehicule moves forward
			for a number of <unsigned int> ticks at
			<unsigned int> speed (in percent, optional) to determin
			the distance it covers.
			Used to determin this->meterPerTick given by
			the result: measurement/tick
		*/
		void test_forward(unsigned int tick, unsigned int speed=100);
		void test_turn(unsigned int tick, unsigned int speed=100);
	private:
		void reset_tick();
};
#endif
