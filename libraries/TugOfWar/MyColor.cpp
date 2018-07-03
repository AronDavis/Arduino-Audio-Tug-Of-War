#include <Arduino.h>
#include <MyColor.h>


MyColor::MyColor()
{
	Reset();
}

MyColor::MyColor(char r, char g, char b)
{
	Set(r, g, b);
}

void MyColor::Reset()
{
	_r = 0;
	_g = 0;
	_b = 0;
}

void MyColor::Set(MyColor c)
{
	Set(c._r, c._g, c._b);
}

void MyColor::Set(char r, char g, char b)
{
	_r = r;
	_g = g;
	_b = b;
}

void MyColor::Add(MyColor c)
{
	Add(c._r, c._g, c._b);
}

void MyColor::Add(char r, char g, char b)
{
	_r += r;
	_g += g;
	_b += b;
}

unsigned int MyColor::R()
{
	return _cast(_r);
}

unsigned int MyColor::G()
{
	return _cast(_g);
}

unsigned int MyColor::B()
{
	return _cast(_b);
}

unsigned int MyColor::_cast(char c)
{
	return (unsigned int)((unsigned char)c);
}