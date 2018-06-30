#include <Adafruit_NeoPixel.h>
#include <TugOfWar.h>

#define D_PIN_RED 6
#define D_PIN_BLUE 7
#define A_PIN_RED 0
#define A_PIN_BLUE 1
#define NUM_PIXELS 120
#define ORIGINAL_GOAL_POST_DISTANCE 20

TugOfWar _redTeam;
TugOfWar _blueTeam;

const int _sampleWindow = 10; // Sample window width in mS (50 mS = 20Hz)

int _currentPos = 0;
int _acceleration = 1;
int _currentSpeed = 0;
int _maxSpeed = 5;

int _goalPostDistance = ORIGINAL_GOAL_POST_DISTANCE;

void setup() {
  Serial.begin(9600);

  uint32_t redColor = Adafruit_NeoPixel::Color(255,0,0);
  uint32_t blueColor = Adafruit_NeoPixel::Color(0,0,255);
  uint32_t neutralColor = Adafruit_NeoPixel::Color(255,255,255);
  _redTeam = TugOfWar(D_PIN_RED, A_PIN_RED, redColor, blueColor, neutralColor, NUM_PIXELS);
  _blueTeam = TugOfWar(D_PIN_BLUE, A_PIN_BLUE, blueColor, redColor, neutralColor, NUM_PIXELS);
}

void loop() {
  unsigned long startMillis = millis(); // Start of sample window

  _redTeam.ResetMicSample();
  _blueTeam.ResetMicSample();

  while (millis() - startMillis < _sampleWindow)
  {
    _redTeam.DoMicSample();
    _blueTeam.DoMicSample();
  }

  int redPeakToPeak = _redTeam.GetPeakToPeak();
  int bluePeakToPeak = _blueTeam.GetPeakToPeak();

  int teamDiff = redPeakToPeak - bluePeakToPeak;

  if(teamDiff > 0) //red is winning
  {
    _currentSpeed += _acceleration;
  }
  else if(teamDiff < 0) //blue is winning
  {
    _currentSpeed -= _acceleration;
  }
  else //tie
  {
    //TODO: shrink goal posts every time there's a tie
    
    if (_currentSpeed < 0)
      _currentSpeed += _acceleration;
    else if (_currentSpeed > 0)
      _currentSpeed -= _acceleration;
  }

  _currentSpeed = constrain(_currentSpeed, -_maxSpeed, _maxSpeed);
  _currentPos += _currentSpeed;
  _currentPos = constrain(_currentPos, 0, NUM_PIXELS - 1);

  _redTeam.Draw(_currentPos, _goalPostDistance);
  _blueTeam.Draw((NUM_PIXELS - 1) - _currentPos, _goalPostDistance);

  //Serial.println(_currentPos);

  //TODO: if goal post has been crossed, end game
  
  delay(50);
}

/*
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
*/
