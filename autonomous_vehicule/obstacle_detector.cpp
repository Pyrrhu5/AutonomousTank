#include "obstacle_detector.h"
#include <ArduinoSTL.h>
#include <Arduino.h>

void ObstacleDetector::begin(
	int servoPin,
	int echoPin,
	int triggerPin,
	float minDistance,
	int precision=45,
	int servoRPM=120
){
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
	this->servoRPM = servoRPM;
	// Results array initialization
	this->scanData = new AngleData[this->nRotation];
}

float ObstacleDetector::get_distance(){
	digitalWrite(this->triggerPin, LOW);
	delayMicroseconds(5);
	digitalWrite(this->triggerPin, HIGH);
	delayMicroseconds(15);
	digitalWrite(this->triggerPin, LOW);

	return (pulseIn(this->echoPin, HIGH)) * 0.0001657; // nice number to convert to m
}

void ObstacleDetector::scan(){
	int currentAngle = 0;
	this->set_angle(currentAngle);

	for (int i = 0; i < this->nRotation; ++i){
		AngleData data;
		this->scanData[i] = {
			currentAngle,
			this->angle_to_vehicule_angle(currentAngle),
			this->get_distance()
		};

		currentAngle += this->precision;
		this->set_angle(currentAngle);
	}

}

int ObstacleDetector::best_angle(){
	this->scan();
	std::sort(this->scanData, this->scanData + nRotation, this->scanData->compare);
}

void ObstacleDetector::acceptable_angle(){
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
		if (i < middleIndex){
			this->scanData[i].weightedDistance = (i + 1) * this->scanData[i].distance;
		} else if (i == middleIndex){
			// going straight is weighted very high
			this->scanData[i].weightedDistance = (i + 2) * this->scanData[i].distance;
		} else {
			// end to middle of the array
			this->scanData[i].weightedDistance = (nRotation - i) * this->scanData[i].distance;
		}
	}

	std::sort(this->scanData, this->scanData + nRotation, this->scanData->compareWeighted);
}

void ObstacleDetector::set_angle(int degree){
	// Calculate the time for the servo to reach its desired angle
	// calculate the degrees to travel
	float timeToWait = abs(this->servo.read() - degree);
	// calculate the time in seconds to do the journey
	// 1 RPM = 6 degree/second
	timeToWait = timeToWait / (this->servoRPM * 6);
	// converts to ms
	timeToWait = timeToWait * 1000;

	this->servo.write(degree);
	delay(timeToWait);
}

int ObstacleDetector::angle_to_vehicule_angle(int angle){
	if (angle == -1) return -1;
	return map(angle, 0, 180, -90, 90);
}

