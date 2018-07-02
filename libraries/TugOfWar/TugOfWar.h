#ifndef TugOfWar_h
#define TugOfWar_h

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif



class TugOfWar
{
public:
	TugOfWar(int lightsDigitalPin, int micAnalogPin, int numPixels);
	void SetColors(uint32_t myColor, uint32_t enemyColor, uint32_t neutralColor);
	void ResetMicSample();
	void DoMicSample();
	unsigned int GetPeakToPeak();
	void Draw(int currentPos, int goalPostDistance);
private:
	int _lightsDigitalPin;
	int _micAnalogPin;
	uint32_t _myColor;
	uint32_t _enemyColor;
	uint32_t _neutralColor;
	int _numPixels;
	Adafruit_NeoPixel _strip;

	unsigned int _peakToPeak;
	unsigned int _signalMax;
	unsigned int _signalMin;

	int _i;
};

#endif