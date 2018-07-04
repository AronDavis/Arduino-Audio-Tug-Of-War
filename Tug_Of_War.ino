#include <Adafruit_NeoPixel.h>
#include <TugOfWar.h>
#include <Paddle.h>
#include <MyColor.h>

#define D_PIN_RED 6
#define D_PIN_BLUE 5
#define A_PIN_RED 5
#define A_PIN_BLUE 0
#define NUM_PIXELS 120
#define ORIGINAL_GOAL_POST_DISTANCE 5

#define WAITING_MODE 0
#define TRANSITION_TO_GAME_MODE 1
#define GAME_MODE 2
#define TRANSITION_TO_END_MODE 3
#define END_MODE 4

TugOfWar _redTeam = TugOfWar(D_PIN_RED, A_PIN_RED, NUM_PIXELS);
TugOfWar _blueTeam = TugOfWar(D_PIN_BLUE, A_PIN_BLUE, NUM_PIXELS);

Paddle _redPaddle = Paddle(NUM_PIXELS);
Paddle _bluePaddle = Paddle(NUM_PIXELS);

MyColor _redColor = MyColor(255, 0, 0);
MyColor _blueColor = MyColor(0, 0, 255);
MyColor _neutralColor = MyColor(255, 255, 255);

MyColor _redBuffer [NUM_PIXELS] = {};
MyColor _blueBuffer [NUM_PIXELS] = {};

const int _sampleWindow = 10; // Sample window width in mS (50 mS = 20Hz)

const int _transitionToGameDuration = 2000;
const int _blinkSpeed = 200; 
const int _audioBarDuration = 10000;

int _currentPos;
int _acceleration;
int _currentSpeed;
int _maxSpeed;
int _goalPostDistance;

int _redWins = 0;
int _blueWins = 0;



int _mode = 0;

void setup() {
  Serial.begin(9600);
  
  _redTeam.SetColors(_redColor, _blueColor, _neutralColor);
  _blueTeam.SetColors(_blueColor, _redColor, _neutralColor);

  resetGame();
}

void loop() {

  switch(_mode)
  {
    case WAITING_MODE:
      waitingMode();
      break;
    case TRANSITION_TO_GAME_MODE:
      transitionToGameMode();
      break;
    case GAME_MODE:
      gameMode();
      break;
    case TRANSITION_TO_END_MODE:
      transitionToEndMode();
      break;
    case END_MODE:
      endScreenMode();
      break;
  }
}

void resetGame()
{
  _currentPos = NUM_PIXELS / 2;
  _acceleration = 1;
  _currentSpeed = 0;
  _maxSpeed = 1;
  _goalPostDistance = ORIGINAL_GOAL_POST_DISTANCE;
}

void switchMode(int mode)
{
  resetBuffers();
  _mode = mode;

  switch(_mode)
  {
    case WAITING_MODE:
      _redPaddle.Reset();
      _bluePaddle.Reset();
      break;
    case TRANSITION_TO_GAME_MODE:
      break;
    case GAME_MODE:
      resetGame();
      break;
    case TRANSITION_TO_END_MODE:
      break;
    case END_MODE:
      break;
  }
}

void waitingMode()
{
  //TODO: blink to start?
  
  resetBuffers();
  
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

  /*
  Serial.print(bluePeakToPeak);
  Serial.print(",");
  Serial.println(redPeakToPeak);
  */
  
  int redPos = _redPaddle.UpdatePos(redPeakToPeak);
  int bluePos = _bluePaddle.UpdatePos(bluePeakToPeak);
  
  _redPaddle.WriteToBuffer(_blueBuffer, _redColor, false);
  _bluePaddle.WriteToBuffer(_redBuffer, _blueColor, false);

  _redPaddle.WriteToBuffer(_redBuffer, _redColor, true);
  _bluePaddle.WriteToBuffer(_blueBuffer, _blueColor, true);
  
  drawBuffers();

  if(redPos == NUM_PIXELS - 1 && bluePos == NUM_PIXELS - 1)
  {
    switchMode(TRANSITION_TO_GAME_MODE);
  }
}

void transitionToGameMode()
{
  unsigned long startMillis = millis(); // Start of sample window

  while (millis() - startMillis < _transitionToGameDuration)
  {
    resetBuffers();
    _redPaddle.UpdateWave();
    _redPaddle.WriteToBuffer(_redBuffer, _neutralColor, true);
    _redPaddle.WriteToBuffer(_blueBuffer, _neutralColor, true);
    drawBuffers();
    delay(_sampleWindow);
  }

  //TODO: flash tug of war first

  switchMode(GAME_MODE);
}

void gameMode()
{  
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

  /*
  Serial.print(bluePeakToPeak);
  Serial.print(",");
  Serial.print(redPeakToPeak);
  Serial.print(",");
  Serial.println(teamDiff);
  */
  
  int deadZone = 200;
  int diffTolerance = 50;
  int mid = NUM_PIXELS / 2;
  
  if(teamDiff < diffTolerance && teamDiff > -diffTolerance) //tie
  {
    //TODO: shrink goal posts every time there's a tie
    if (_currentSpeed < 0)
      _currentSpeed += _acceleration;
    else if (_currentSpeed > 0)
      _currentSpeed -= _acceleration;
  } 
  else if(teamDiff > 0) //red is winning
  {
    _currentSpeed += _acceleration;
  }
  else if(teamDiff < 0) //blue is winning
  {
    _currentSpeed -= _acceleration;
  }

  _currentSpeed = constrain(_currentSpeed, -_maxSpeed, _maxSpeed);
  _currentPos += _currentSpeed;
  _currentPos = constrain(_currentPos, _goalPostDistance, (NUM_PIXELS - 1) - _goalPostDistance);

  resetBuffers();
  _redTeam.WriteToBuffer(_redBuffer, _currentPos, _goalPostDistance);
  _blueTeam.WriteToBuffer(_blueBuffer, (NUM_PIXELS - 1) - _currentPos, _goalPostDistance);
  drawBuffers();

  //TODO: maybe base goal posts from start and end instead of mid so we get consistency
  if(_currentPos >= (NUM_PIXELS - 1) - _goalPostDistance || _currentPos <= _goalPostDistance)
    switchMode(TRANSITION_TO_END_MODE);
}

void transitionToEndMode()
{
  unsigned long startMillis = millis(); // Start of sample window

  bool redWon = (_currentPos >= (NUM_PIXELS - 1) - _goalPostDistance);

  while (millis() - startMillis < _transitionToGameDuration)
  {
    resetBuffers();
    _redTeam.WriteToBuffer(_redBuffer, _currentPos, _goalPostDistance);
    _blueTeam.WriteToBuffer(_blueBuffer, (NUM_PIXELS - 1) - _currentPos, _goalPostDistance);
    drawBuffers();
    delay(_blinkSpeed);

    resetBuffers();
    drawBuffers();
    delay(_blinkSpeed);
  }
  
  switchMode(END_MODE);
}


void endScreenMode()
{ 
  unsigned long startMillis = millis();
 
  while (millis() - startMillis < _audioBarDuration)
  {
    _redTeam.ResetMicSample();
    _blueTeam.ResetMicSample();

    unsigned long startSampleMillis = millis();
    while (millis() - startSampleMillis < _sampleWindow)
    {
      _redTeam.DoMicSample();
      _blueTeam.DoMicSample();
    }
    
    int redPeakToPeak = _redTeam.GetPeakToPeak();
    int bluePeakToPeak = _blueTeam.GetPeakToPeak();
    
    // map to the max scale of the display
    int redLightsToShow = map(redPeakToPeak , 0, 600, 0, (NUM_PIXELS / 2) + 1);
    int blueLightsToShow = map(bluePeakToPeak , 0, 600, 0, (NUM_PIXELS / 2) + 1);
    
    resetBuffers();
    for(int i = 0; i < redLightsToShow; i++)
    {
      _redBuffer[i].Add(_redColor);
      _blueBuffer[(NUM_PIXELS - 1) - i].Add(_redColor);
    }
    
    for(int i = 0; i < blueLightsToShow; i++)
    {
      _blueBuffer[i].Add(_blueColor);
      _redBuffer[(NUM_PIXELS - 1) - i].Add(_blueColor);
    }
    
    drawBuffers(); 
  }

  switchMode(WAITING_MODE);
}

void resetBuffers()
{
  for(int i = 0; i < NUM_PIXELS; i++)
  {
    _redBuffer[i].Reset();
    _blueBuffer[i].Reset();
  }
}

void drawBuffers()
{
  _redTeam.DrawBuffer(_redBuffer);
  _blueTeam.DrawBuffer(_blueBuffer);
}
