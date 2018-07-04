#include <Arduino.h>
#include <TugOfWar.h>
#include <MyColor.h>
#include <Adafruit_NeoPixel.h>

TugOfWar::TugOfWar(int lightsDigitalPin, int micAnalogPin, int numPixels)
{
	_lightsDigitalPin = lightsDigitalPin;
	_micAnalogPin = micAnalogPin;
	_numPixels = numPixels;
}

void TugOfWar::SetColors(MyColor myColor, MyColor enemyColor, MyColor neutralColor)
{
	_myColor = myColor;
	_enemyColor = enemyColor;
	_neutralColor = neutralColor;

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

	return _peakToPeak;
}

void TugOfWar::WriteToBuffer(MyColor buffer[], int currentPos, int goalPostDistance)
{
	for (int i = 0; i <= goalPostDistance; i++)
		buffer[i].Set(_neutralColor);

	for (int i = goalPostDistance + 1; i < currentPos; i++)
		buffer[i].Set(_myColor);

	buffer[currentPos].Set(_neutralColor);

	for (int i = currentPos + 1; i < (_numPixels - 1) - goalPostDistance; i++)
		buffer[i].Set(_enemyColor);

	for (int i = (_numPixels - 1) - goalPostDistance; i < _numPixels - 1; i++)
		buffer[i].Set(_neutralColor);
}

void TugOfWar::DrawBuffer(MyColor buffer[])
{
	for (int i = 0; i < _numPixels; i++)
	{
		MyColor c = buffer[i];
		_strip.setPixelColor(i, Adafruit_NeoPixel::Color(c.R(), c.G(), c.B()));
	}

	_strip.show();
}