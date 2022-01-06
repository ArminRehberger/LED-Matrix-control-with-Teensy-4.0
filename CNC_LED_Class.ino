/*
""" ############################################################################################ """
""" ############################################################################################ """
""" LED matrix (8*X) WS2812B control with Teensy4.0 """
""" V1_00 2022-01-06, Armin Rehberger """
""" ############################################################################################ """
""" ############################################################################################ """
*/

/*
// ###############################################################################################################
// ###############################################################################################################
##### Libary for LEDs driver: OctoWS2811
https://github.com/PaulStoffregen/OctoWS2811
#include <OctoWS2811.h>

##### Libary for color and shift the LEDs
LEDMatrix8.h
Integrated with "", so copy the files LEDMatrix8.cpp and LEDMatrix8.h in the same directory than the file CNC_LED_Class.ino
#include "LEDMatrix8.h"
*/

/*
// ###############################################################################################################
// ###############################################################################################################
Frequency calculation, example RGB LED strip with 300LEDs
800Khz = 1.25us
Per bit (high or low): 1.25us
Per LED: 3 Byte = 24 Bit = 30us
Reset: 50us
Per 300 LED: 30us * 300LED = 9000us + 50us = 9500us = 9.5ms
Frequency: F=1/t = 1/0.0095s = 105.26Hz

Frequency calculation, example RGB LED strip with 256LEDs
800Khz = 1.25us
Per bit (high or low): 1.25us
Per LED: 3 Byte = 24 Bit = 30us
Reset: 50us
Per 256 LED: 30us * 256LED = 7680us + 50us = 7730us = 7.73ms
Frequency: F=1/t = 1/0.00773s = 129.36Hz

Frequency calculation, example RGB LED strip with 125LEDs
800Khz = 1.25us
Per bit (high or low): 1.25us
Per LED: 3 Byte = 24 Bit = 30us
Reset: 50us
Per 125 LED: 30us * 125LED = 3750us + 50us = 3800us = 3.8ms
Frequency: F=1/t = 1/0.0038s = 263.16Hz
*/

/*
// ###############################################################################################################
// ###############################################################################################################
Current calculation
Used voltage: 5V
Per LED 60mA, when all three LEDs are on (color white)
300LEDs  = 18A
2400LEDs = 144A (300LED * 8 stripes)
256LEDs  = 15,36A
1024LEDs = 61.4A (256LED * 4 stripes)
2048LEDs = 122.8A (256LED * 8 stripes)
*/

// ###############################################################################################################
// ###############################################################################################################
// ##### Include
#include <OctoWS2811.h>
#include "LEDMatrix8.h"

// ###############################################################################################################
// ###############################################################################################################
// ##### Common definitions
#define DEBUGMODE true // Print diagnostic values to serial monitor

#define NONE 0
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

#define COLOR0 0
#define COLOR1 1
#define COLOR2 2

// ###############################################################################################################
// ###############################################################################################################
// ##### Global strings to write to the LED matrix
// All together max. 1773 characters

// Already defined letters in function "InitializeLetters"
// "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!?.:+-=# {}[]"
// { Haert, part 1
// } Haert, part 2
// [ Smily, part 1
// ] Smily, part 2

char StringToDraw[3][4000] = {
  "  [][][][]{}{}{}{}   ",
  "MERRY CHRISTMAS AND A HAPPY NEW YEAR {}{} ",
  "FROHE WEIHNACHTEN UND EIN GUTES NEUES JAHR [][] ",
  };

// ###############################################################################################################
// ###############################################################################################################
// ##### Global struct to draw the scenes
typedef struct
{
  bool ArrayIndexUsed;
  int StringNo;
  unsigned long DelaytimeMillis;
  int ColorSceneNo;
  int Color;
  int MaxIntense;
  int AnimationNo;
  int AmountNo;
} ParameterDrawScene;
ParameterDrawScene ParaDraw[100] = {};

// ###############################################################################################################
// ###############################################################################################################
// ##### Forward declaration functions
bool DrawScene(ParameterDrawScene*, const char*, int);
void DrawPixels(int, const byte[], unsigned long);

// ###############################################################################################################
// ###############################################################################################################
// ##### Initialize scenes
// LED-Matrix will start with scene 0, 1, ... After last scene, it starts again with scene 0

// ArrayIndexUsed: Must be true, when the scene should be drawen
// StringNo: String no. in the StringToDraw array to be drawen
// DelaytimeMillis: Delaytime in ms to shift the pixel array e.g. 10, 20, 25, 50, 100, 2000, 0=fastest possible speed
// ColorSceneNo: COLOR0, whole matrix one times BLUE, then one times GREEN, then one times RED, ...YELLOW PINK ORANGE WHITE
// ColorSceneNo: COLOR1, whole matrix blue MinIntense..MaxIntense..MinIntense, green MinIntense..MaxIntense..MinIntense, red MinIntense..MaxIntense..MinIntense
// ColorSceneNo: COLOR2, whole matrix in value Color
// Color: Used for COLOR2. RED, GREEN, BLUE, YELLOW, PINK, ORANGE, WHITE, BACKGROUNDCOLUR
// MaxIntense: Used for Color1, MaxIntense 0..255. MinIntense = 1
// AnimationNo: NONE, LEFT, RIGHT, UP, DOWN
// AmountNo: Show string x times, e.g. LEFT 7 times

void InitValuesForScenes()
{
  // ##### Local variables
  int Index = 0;

  // ##### Scene 0, String no. 1, COLOR0
  ParaDraw[Index].ArrayIndexUsed = true;
  ParaDraw[Index].StringNo = 1;
  ParaDraw[Index].DelaytimeMillis = 20;
  ParaDraw[Index].ColorSceneNo = COLOR0;
  ParaDraw[Index].Color = PINK; // Just for COLOR2
  ParaDraw[Index].MaxIntense = 96; // Just for COLOR1
  ParaDraw[Index].AnimationNo = LEFT;
  ParaDraw[Index].AmountNo = 7;
  Index++;

  // ##### Scene 1, String no. 1, COLOR1
  ParaDraw[Index].ArrayIndexUsed = true;
  ParaDraw[Index].StringNo = 1;
  ParaDraw[Index].DelaytimeMillis = 20;
  ParaDraw[Index].ColorSceneNo = COLOR1;
  ParaDraw[Index].Color = PINK; // Just for COLOR2
  ParaDraw[Index].MaxIntense = 96; // Just for COLOR1
  ParaDraw[Index].AnimationNo = LEFT;
  ParaDraw[Index].AmountNo = 5;
  Index++;

  // ##### Scene 2, String no. 1, COLOR2
  ParaDraw[Index].ArrayIndexUsed = true;
  ParaDraw[Index].StringNo = 1;
  ParaDraw[Index].DelaytimeMillis = 20;
  ParaDraw[Index].ColorSceneNo = COLOR2;
  ParaDraw[Index].Color = BLUE; // Just for COLOR2
  ParaDraw[Index].MaxIntense = 96; // Just for COLOR1
  ParaDraw[Index].AnimationNo = LEFT;
  ParaDraw[Index].AmountNo = 2;
  Index++;

  // ##### Scene 3, String no. 2, COLOR2
  ParaDraw[Index].ArrayIndexUsed = true;
  ParaDraw[Index].StringNo = 2;
  ParaDraw[Index].DelaytimeMillis = 20;
  ParaDraw[Index].ColorSceneNo = COLOR2;
  ParaDraw[Index].Color = ORANGE; // Just for COLOR2
  ParaDraw[Index].MaxIntense = 96; // Just for COLOR1
  ParaDraw[Index].AnimationNo = RIGHT;
  ParaDraw[Index].AmountNo = 1;
  Index++;

  // ##### Scene 4, String no. 0, COLOR2
  ParaDraw[Index].ArrayIndexUsed = true;
  ParaDraw[Index].StringNo = 0;
  ParaDraw[Index].DelaytimeMillis = 20;
  ParaDraw[Index].ColorSceneNo = COLOR2;
  ParaDraw[Index].Color = YELLOW; // Just for COLOR2
  ParaDraw[Index].MaxIntense = 96; // Just for COLOR1
  ParaDraw[Index].AnimationNo = UP;
  ParaDraw[Index].AmountNo = 1;
  Index++;

  // ##### Scene 5, String no. 0, COLOR2
  ParaDraw[Index].ArrayIndexUsed = true;
  ParaDraw[Index].StringNo = 0;
  ParaDraw[Index].DelaytimeMillis = 20;
  ParaDraw[Index].ColorSceneNo = COLOR2;
  ParaDraw[Index].Color = PINK; // Just for COLOR2
  ParaDraw[Index].MaxIntense = 96; // Just for COLOR1
  ParaDraw[Index].AnimationNo = DOWN;
  ParaDraw[Index].AmountNo = 1;
  Index++;
 
  // ##### Scene 6, String no. 0, COLOR2
  ParaDraw[Index].ArrayIndexUsed = true;
  ParaDraw[Index].StringNo = 0;
  ParaDraw[Index].DelaytimeMillis = 20;
  ParaDraw[Index].ColorSceneNo = COLOR2;
  ParaDraw[Index].Color = WHITE; // Just for COLOR2
  ParaDraw[Index].MaxIntense = 96; // Just for COLOR1
  ParaDraw[Index].AnimationNo = NONE;
  ParaDraw[Index].AmountNo = 10;
  Index++;
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Create and configure an OctoWS2811 object
// The total number of pixels is "ledsPerStrip * numPins"
const int ledsPerStrip = 256; // LED-matrix 8x32
const int numPins = 4; // Amount of conected output pins, 1, 2, 3, 4, ... 8
byte pinList[numPins] = {7, 6, 5, 4}; // Connected output pin numbers. With Teensy 4.x, you can use any group of pins.
const int bytesPerLED = 3;  // RGB 3 bytes needed, RGBW 4 bytes needed
DMAMEM int displayMemory[ledsPerStrip * numPins * bytesPerLED / 4]; // Allocate DMA memory Teensy4 DMAChannel.h. Divide by 4 int -> byte
int drawingMemory[ledsPerStrip * numPins * bytesPerLED / 4];

const int config = WS2811_GRB | WS2811_800kHz; // LED Matrix WS2811_GRB, LED stripe WS2811_RGB
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

// ###############################################################################################################
// ###############################################################################################################
// ##### Create and configure an LED matrix object
int AmountOfStrings;    // Amount of strings in the StringToDraw char-array
int AmountOfIndex;      // Amount of index in the ParaDraw array
int ActiveIndex = 0;
const int AmountOfPixelsDrawingArray = ledsPerStrip * numPins;
LEDMatrix8 matrix(StringToDraw, AmountOfPixelsDrawingArray, ActiveIndex); // String array, amount of pixels, active string no.

// ###############################################################################################################
// ###############################################################################################################
// ##### Initialization setup()
void setup()
{

  // ##### Define LED on board
  pinMode(LED_BUILTIN, OUTPUT); // LED on board

  // ##### Serieller Monitor
  if (DEBUGMODE == true)
  {
    Serial.begin(9600);
    Serial.println("Startup");
  }

  // ##### Start OctoWS2811
  leds.begin();
  leds.show();

  // ##### Initialize values for scenes
  InitValuesForScenes();

  // ##### Get amount of strings in the StringToDraw char-array
  AmountOfStrings = sizeof StringToDraw / sizeof *StringToDraw;
  // ##### Get amount of index in the ParaDraw array
  AmountOfIndex = sizeof ParaDraw / sizeof *ParaDraw;

  // ##### Check if in first scene string no. != 0, because matrix is initialized with string no. 0
  // If so, initialize scene again
  if(ParaDraw[ActiveIndex].StringNo != 0)
  {
      if(ParaDraw[ActiveIndex].StringNo >= AmountOfStrings)
      {
        ParaDraw[ActiveIndex].StringNo = 0;
      }
      matrix.InitSzene(REDRAW, ParaDraw[ActiveIndex].StringNo);
  }

} // void setup()

// ###############################################################################################################
// ###############################################################################################################
// ##### Main program loop()
void loop()
{
  // ##### Local variables
  static bool ledon = false;
  static int SceneNo = 0;
  int StringNoOld, StringNoNew;
  bool SceneDone;

  // ##### Draw scene
  switch(SceneNo)
  {
    // ##### Draw scene
    case 0:
      if(ParaDraw[ActiveIndex].ArrayIndexUsed == true)
      {
        // Call function draw scene
        SceneDone = DrawScene(&ParaDraw[ActiveIndex], StringToDraw[ParaDraw[ActiveIndex].StringNo], AmountOfPixelsDrawingArray);
        if(SceneDone == true)
        {
          SceneNo++;
        }        
      }
      else
      {
        SceneNo++;
      }
      break;

    // ##### Increment index, check if string no. has changed
    case 1:
      StringNoOld = ParaDraw[ActiveIndex].StringNo;
      ActiveIndex++;
      if(ActiveIndex > AmountOfIndex-1)
      {
        ActiveIndex = 0;
      }
      StringNoNew = ParaDraw[ActiveIndex].StringNo;
      if(StringNoOld == StringNoNew)
      {
        SceneNo = 0;
      }
      else
      {
        SceneNo++;
      }
      break;

    // ##### Initialize scene, if string no. has changed
    case 2:
      if(ParaDraw[ActiveIndex].StringNo >= AmountOfStrings)
      {
        ParaDraw[ActiveIndex].StringNo = 0;
      }
      matrix.InitSzene(REDRAW, ParaDraw[ActiveIndex].StringNo);
      SceneNo = 0;
      break;

    default:
    break;   
  }

  // ##### Toggle LED on board
  ledon = ! ledon;
  digitalWrite(LED_BUILTIN, ledon);
} // void loop()

// ###############################################################################################################
// ###############################################################################################################
// ##### Draw scene
bool DrawScene(ParameterDrawScene *inParaDraw, const char* inStringToDraw, int inAmountOfPixelsDrawingArray)
{
  // ##### Local variables
  bool ReturnDone = false;
  static int StrlenActiveString;
  static int ColumnsActiveString;
  static int CounterActiveScene = 0;
  static int AmountActiveScene = 0;

  StrlenActiveString = strlen(inStringToDraw);
  ColumnsActiveString = StrlenActiveString * AMOUNTCOLUMNLETTER;

  // ##### Color scene
  switch(inParaDraw->ColorSceneNo)
  {
    case COLOR0:
      matrix.ColorScene0(ColumnsActiveString); // ColorScene 0, whole matrix inparameter times BLUE, then inparameter times GREEN, then inparameter times RED, ...YELLOW PINK ORANGE WHITE
    break;
    case COLOR1:
      matrix.ColorScene1(inParaDraw->MaxIntense); // ColorScene 1, whole matrix blue 1..inParaDraw->MaxIntense..1, green 1..inParaDraw->MaxIntense..1, red 1..inParaDraw->MaxIntense..1
    break;
    case COLOR2:
      matrix.ColorScene2(inParaDraw->Color); // ColorScene 2, whole matrix inParaDraw->Color
    break;
    default:
      matrix.ColorScene2(inParaDraw->Color); // ColorScene 2, whole matrix inParaDraw->Color
    break;   
  }

  // ##### Draw scene
  DrawPixels(inAmountOfPixelsDrawingArray, matrix.DrawPixelArray, inParaDraw->DelaytimeMillis);

  // ##### Animation scene (shift left, right, up, down)
  switch(inParaDraw->AnimationNo)
  {
    case NONE:
      AmountActiveScene = AMOUNTLINESLETTER * inParaDraw->AmountNo;
    break;
    case LEFT:
      matrix.ShiftDrawingArrayLeft();
      AmountActiveScene = ColumnsActiveString * inParaDraw->AmountNo;
    break;
    case RIGHT:
      matrix.ShiftDrawingArrayRight();
      AmountActiveScene = ColumnsActiveString * inParaDraw->AmountNo;
    break;
    case UP:
      matrix.ShiftDrawingArrayUp();
      AmountActiveScene = AMOUNTLINESLETTER * inParaDraw->AmountNo;
    break;
    case DOWN:
      matrix.ShiftDrawingArrayDown();
      AmountActiveScene = AMOUNTLINESLETTER * inParaDraw->AmountNo;
    break;
    default:
      matrix.ShiftDrawingArrayLeft();
      AmountActiveScene = ColumnsActiveString * inParaDraw->AmountNo;
    break;   
  }

  // ##### Scene done
  CounterActiveScene++;
  if(CounterActiveScene >= AmountActiveScene) // Show string x times
  {
    DrawPixels(inAmountOfPixelsDrawingArray, matrix.DrawPixelArray, 1000); // Draw scene and wait 1000ms
    CounterActiveScene = 0;
    ReturnDone = true;
  }

  return ReturnDone;
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Draw the pixels
void DrawPixels(int inAmountOfPixelsDrawingArray, const byte inDrawPixelArray[], unsigned long inDelaytimeMillis)
{
  // ##### Local variables
  static unsigned long previousMillis = 0;
  static unsigned long currentMillis = 0;
  bool LEDbusy;
  int ColorPixel;
  int i;

  // ##### Set the pixels color
  for (i=0; i < inAmountOfPixelsDrawingArray; i++)
  {
    if(inDrawPixelArray[i] == 0) // 0 = pixel for background
    {
      ColorPixel = BACKGROUNDCOLUR;
    }
    else if(inDrawPixelArray[i] == 1) // 1 = pixel for letter / number
    {
      ColorPixel = matrix.ColorPixel[i];
    }
    else if(inDrawPixelArray[i] == 2) // 2 = pixel for underline
    {
      //ColorPixel = RED;
      ColorPixel = BACKGROUNDCOLUR;
      //ColorPixel = matrix.ColorPixel[i];
    }
    else
    {
      ColorPixel = BACKGROUNDCOLUR;
    }
    leds.setPixel(i, ColorPixel); // Set the pixels color
  }

  // ##### Draw the pixels
  leds.show();

  // ##### Delaytime in ms
  if(inDelaytimeMillis != 0)
  {
    currentMillis = millis();
    previousMillis = currentMillis;
    do
    {
      currentMillis = millis();
    } while (currentMillis - previousMillis < inDelaytimeMillis);    
  }
  // ##### Fastest possible speed
  else
  {
    do
    {
      LEDbusy = leds.busy();
    }while (LEDbusy == true);    
  }
}// void DrawPixels
