#include <Arduino.h>
#include "logger.h"

void logger(const char* msg, const char* level){
	String buffer;

	long now = millis();
	char unit[4];
	// Hours
	sprintf(unit, "%02ld",(now / (1000 * 60UL * 60UL) % 24) );
	buffer += unit;
	buffer += ":";
	// minutes
	sprintf(unit, "%02ld",(now / (1000 * 60UL)) % 60);
	buffer += unit;
	buffer += ":";
	// seconds
	sprintf(unit, "%02ld",(now / 1000) % 60);
	buffer += unit;
	buffer += ":";
	// ms
	sprintf(unit, "%03ld",now % 1000);
	buffer += unit;

	buffer += " - ";
	buffer += level;
	buffer += " - ";
	buffer += msg;
	Serial.println(buffer);
}

void logger(String msg, const char* level){
	String buffer;

	long now = millis();
	char unit[4];
	// Hours
	sprintf(unit, "%02ld",(now / (1000 * 60UL * 60UL) % 24) );
	buffer += unit;
	buffer += ":";
	// minutes
	sprintf(unit, "%02ld",(now / (1000 * 60UL)) % 60);
	buffer += unit;
	buffer += ":";
	// seconds
	sprintf(unit, "%02ld",(now / 1000) % 60);
	buffer += unit;
	buffer += ":";
	// ms
	sprintf(unit, "%03ld",now % 1000);
	buffer += unit;

	buffer += " - ";
	buffer += level;
	buffer += " - ";
	buffer += msg;
	Serial.println(buffer);
}

