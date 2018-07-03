#ifndef MyColor_h
#define MyColor_h

class MyColor
{
public:
	MyColor();
	MyColor(char r, char g, char b);
	void Reset();
	void Set(MyColor c);
	void Set(char r, char g, char b);
	void Add(MyColor c);
	void Add(char r, char g, char b);
	unsigned int R();
	unsigned int G();
	unsigned int B();
private:
	char _r;
	char _g;
	char _b;
	unsigned int _cast(char c);
};

#endif