#include <Arduino.h>
#include <Paddle.h>
#include <MyColor.h>

Paddle::Paddle(int numPixels)
{
	_numPixels = numPixels;
	_currentPos = 0;
	_acceleration = 1;
	_currentSpeed = 0;
	_maxSpeed = 2;
	_paddleSize = 5;
	_edgePadding = _paddleSize / 2;

	_waveDelayIndex = 0;
	_waveDelay = 1;

	_waveIndex = 0;
	_waveLength = 20;
}

void Paddle::Reset()
{
	_currentPos = 0;
	_currentSpeed = 0;
}

void Paddle::UpdateWave()
{
	_waveDelayIndex++;
	_waveDelayIndex %= _waveDelay;

	if (_waveDelayIndex % _waveDelay == 0)
	{
		_waveIndex++;
		_waveIndex %= _waveLength;
	}
}

int Paddle::UpdatePos(int peakToPeak)
{
	UpdateWave();
	if (peakToPeak > 200)
		_currentSpeed += _acceleration;
	else
		_currentSpeed -= _acceleration;

	_currentSpeed = constrain(_currentSpeed, -_maxSpeed, _maxSpeed);
	_currentPos += _currentSpeed;
	_currentPos = constrain(_currentPos, 0, _numPixels - 1);

	if (_currentSpeed < 0 && _currentPos == 0)
		_currentSpeed += _acceleration;

	return _currentPos;
}

void Paddle::WriteToBuffer(MyColor buffer[], MyColor c, bool isMyTeam)
{
	bool isFilled = _currentPos == _numPixels - 1;
	int currentPos = _currentPos;

	if (currentPos < _edgePadding)
		currentPos = _edgePadding;
	else if (currentPos > (_numPixels - 1) - _edgePadding)
		currentPos = (_numPixels - 1) - _edgePadding;

	if (isMyTeam && isFilled)
	{
		for (int i = 0; i < currentPos - _edgePadding; i++)
		{
			int val = (i + _waveLength - _waveIndex) % _waveLength;

			buffer[i].Add(
				(int)(((double)val / _waveLength) * c.R()),
				(int)(((double)val / _waveLength) * c.G()),
				(int)(((double)val / _waveLength) * c.B())
			);
		}
	}

	for (int i = currentPos - _edgePadding; i <= currentPos + _edgePadding; i++)
	{
		if(isMyTeam && !isFilled)
			buffer[i].Set(c);
		else
			buffer[i].Add(c);
	}
}