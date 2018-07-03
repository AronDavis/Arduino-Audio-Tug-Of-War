#ifndef Paddle_h
#define Paddle_h

#include <Arduino.h>
#include <MyColor.h>


class Paddle
{
public:
	Paddle(int numPixels);
	void Reset();
	void UpdateWave();
	int UpdatePos(int peakToPeak);
	void WriteToBuffer(MyColor buffer[], MyColor c, bool isMyTeam);
private:
	int _waveDelayIndex;
	int _waveDelay;
	int _waveIndex;
	int _waveLength;

	int _numPixels;
	int _currentPos;
	int _acceleration;
	int _currentSpeed;
	int _maxSpeed;
	int _paddleSize;
	int _edgePadding;
};

#endif