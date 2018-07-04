#ifndef TugOfWar_h
#define TugOfWar_h

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <MyColor.h>

#ifdef __AVR__
#include <avr/power.h>
#endif



class TugOfWar
{
public:
	TugOfWar(int lightsDigitalPin, int micAnalogPin, int numPixels);
	void SetColors(MyColor myColor, MyColor enemyColor, MyColor neutralColor);
	void ResetMicSample();
	void DoMicSample();
	unsigned int GetPeakToPeak();
	void WriteToBuffer(MyColor buffer[], int currentPos, int goalPostDistance);
	void DrawBuffer(MyColor buffer[]);
private:
	int _lightsDigitalPin;
	int _micAnalogPin;
	MyColor _myColor;
	MyColor _enemyColor;
	MyColor _neutralColor;
	int _numPixels;
	Adafruit_NeoPixel _strip;

	unsigned int _peakToPeak;
	unsigned int _signalMax;
	unsigned int _signalMin;

	int _i;
};

#endif