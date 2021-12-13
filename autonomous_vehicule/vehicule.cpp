#include <Arduino.h>
#ifndef Engine
	#include "engine.cpp"
#endif

class Vehicule{
	public:
		Engine *engines[2];
		float meterPerTick;
		float degreePerTick;
	public:
		unsigned int tickTarget=0;
		// for acceleration/deceleration
		// int target speed

	public: 
		void begin(Engine &engineRight, Engine &engineLeft, float meterPerTick, float degreePerTick){ 
			this->engines[0] = &engineRight;
			this->engines[1] = &engineLeft;
			this->meterPerTick = meterPerTick;
			this->degreePerTick = degreePerTick;
		}


		/* 
 			Set the speed of the vehicule in percentage 
  			<unsigned int> speed of its minimum to maximum speed
		*/
		void set_speed(unsigned int speed){
			for (int i=0; i<2; i++){
				this->engines[i]->set_speed_percent(speed);
			}
		}

		/*
 			Moves forward or backwards for a number of 
			<float> meters.
			The direction of travel is determined by the
			sign of meters (negative == backward).
			Does not stop after reaching the destination.
		*/
		void move(float meters){
			this->reset_tick();
			for (int i=0; i<2; i++){
				if (meters > 0) this->engines[i]->forward();
				else this->engines[i]->backward();
			}
			this->tickTarget = round(abs(meters)/this->meterPerTick);
		}

		/*
 			Turns right or left for a number of
			<float> degrees.
			The direction of turning is determined by the
			sign of degrees (negative == left)
			Does not stop after reaching the destination.
		*/
		void turn(int degree, float sharpness = 1){
			this->reset_tick();
			if (degree < 1){
				this->engines[0]->backward();
				this->engines[1]->forward();
			} else {
				this->engines[1]->backward();
				this->engines[0]->forward();
			}
			this->tickTarget = round(abs(degree)/this->degreePerTick);
		}

		/*
 			Stop the vehicule
		*/
		void stop(){
			for (int i=0; i<2; i++){
				this->engines[i]->stop();
			}
		}

		/*
 			Determin if the vehicule has reached its set destination.
			The destination is calculated by comparing the engines current
			tick with this->tickTarget.
			The tickTarget is set by turn and move methods
		*/
		bool has_reached_destination(){
			return this->engines[0]->tick >= this->tickTarget;
		}

		/* 
			Test function where the vehicule moves forward
			for a number of <unsigned int> ticks at
			<unsigned int> speed (in percent, optional) to determin
			the distance it covers.
			Used to determin this->meterPerTick given by
			the result: measurement/tick
		*/
		void test_forward(unsigned int tick, unsigned int speed=100){
			this->reset_tick();
			this->set_speed(speed);
			this->tickTarget = tick;
			for (int i=0; i<2; i++){
				this->engines[i]->forward();
			}
			while (!this->has_reached_destination()){;}
			this->stop();
		}

		void test_turn(unsigned int tick, unsigned int speed=100){
			this->reset_tick();
			this->set_speed(speed);
			this->tickTarget = tick;
			this->engines[0]->forward();
			this->engines[1]->backward();
			while (!this->has_reached_destination()){;}
			this->stop();
		}

	private:
		void reset_tick(){
			for (int i=0; i<2; i++){
				this->engines[i]->tick = 0;
			}
		}
};
