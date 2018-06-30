#include "Arduino.h"
#include "TugOfWar.h"
#include "Adafruit_NeoPixel.h"

TugOfWar::TugOfWar()
{

}

TugOfWar::TugOfWar(int lightsDigitalPin, int micAnalogPin, uint32_t myColor, uint32_t enemyColor, uint32_t neutralColor, int numPixels)
{
	_lightsDigitalPin = _lightsDigitalPin;
	_micAnalogPin = _micAnalogPin;
	_myColor = myColor;
	_enemyColor = enemyColor;
	_neutralColor = neutralColor;
	_numPixels = numPixels;

	// Sample window width in mS (50 mS = 20Hz)
	_sampleWindow = 10; 
	_acceleration = 1;
	_currentSpeed = 0;
	_maxSpeed = 5;

	// Parameter 1 = number of pixels in strip
	// Parameter 2 = Arduino pin number (most are valid)
	// Parameter 3 = pixel type flags, add together as needed:
	//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
	//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
	//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
	//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
	//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
	_strip = Adafruit_NeoPixel(_numPixels, _lightsDigitalPin, NEO_GRB + NEO_KHZ800);
	_strip.begin();

	//Initialize all pixels to 'off'
	_strip.show();
}
void TugOfWar::ResetMicSample()
{
	_peakToPeak = 0;   // peak-to-peak level

	_signalMax = 0;
	_signalMin = 1024;
}

void TugOfWar::DoMicSample()
{
	unsigned int sample = analogRead(_micAnalogPin);

	// only handle valid readings
	if (sample < 1024)
	{
		if (sample > _signalMax)
		{
			// save just the max levels
			_signalMax = sample;
		}
		else if (sample < _signalMin)
		{
			// save just the min levels
			_signalMin = sample;
		}
	}
}

unsigned int TugOfWar::GetPeakToPeak()
{
	_peakToPeak = _signalMax - _signalMin;
}

void TugOfWar::Draw(int currentPos, int goalPostDistance)
{
	_strip.clear();

	for (int i = 0 ; i < currentPos; i++)
		_strip.setPixelColor(i, _myColor);

	_strip.setPixelColor(currentPos, _neutralColor);

	for (int i = currentPos + 1; i < _numPixels - 1; i++)
		_strip.setPixelColor(i, _enemyColor);

	int mid = _numPixels / 2;

	_strip.setPixelColor(mid + goalPostDistance, _neutralColor);
	_strip.setPixelColor(mid - goalPostDistance, _neutralColor);

	_strip.show();
}