/*
""" ############################################################################################ """
""" ############################################################################################ """
""" LED matrix (8*X) WS2812B control with Teensy4.0 """
""" V1_00 2021-12-26, Armin Rehberger """
""" Libary h-file """
""" ############################################################################################ """
""" ############################################################################################ """
*/
#ifndef LEDMatrix8_h
#define LEDMatrix8_h

#include <Arduino.h>

#define AMOUNTLINESLETTER 8   // Matrix height
#define AMOUNTCOLUMNLETTER 6  // Column per letter
#define INIT 0                // Value for initialize the scene
#define REDRAW 1              // Value for redraw the scene

// Colours
/*
#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
#define BACKGROUNDCOLUR  0x000001
*/
// Colours with less intense...
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE  0x101010
#define BACKGROUNDCOLUR  0x000001

class LEDMatrix8
{
public:

	// ##### Constructor (ctor)
  //LEDMatrix8(const char inStringToDraw[][500], int inAmountOfPixelsDrawingArray= 0);
  // Code of constructor here in LEDMatrix8.h, because of template to pass dynamic char array.
  template< typename T, int X, int Y > // Template for LEDMatrix8
  	LEDMatrix8(T (&inStringToDraw)[X][Y], int inAmountOfPixelsDrawingArray, int inActiveStringNo)
    {
      // ##### Local variables
      int AmountLetters;
      int MinAmountDrawLetters;
      int StringNo;
      int i, j;

      // ##### Copy values
      AmountOfPixelsDrawingArray = inAmountOfPixelsDrawingArray; // ledsPerStrip * numPins e.g. 256*4 = 1024
      AmountOfColumnMatrix = AmountOfPixelsDrawingArray / AMOUNTLINESLETTER; // e.g. 1024/8 = 128 column

      // ##### Get amount of strings in the inStringToDraw char-array
      AmountOfStrings = X;

      // ##### Transfer active string no.
      if(inActiveStringNo <= AmountOfStrings)
      {
        StringNo = inActiveStringNo;
      }
      else
      {
        StringNo = 0;
      }
  
      // ##### Check if string is to short for the matrix.
      // If so, add spaces " "
      MinAmountDrawLetters = (int)(((inAmountOfPixelsDrawingArray / AMOUNTLINESLETTER) / AMOUNTCOLUMNLETTER) +1);

      for(i=0; i<AmountOfStrings; i++)
      {
        AmountLetters = strlen(inStringToDraw[i]);
        if(AmountLetters < MinAmountDrawLetters)
        {
          for(j=AmountLetters; j<MinAmountDrawLetters; j++)
          {
            inStringToDraw[i][j] = ' ';
            inStringToDraw[i][j+1] = '\0';
          }
        }    
      }
  
      // ##### Fill Dynamic array with the stringlength, determine max. stringlength
      StringlenStringToDraw = (int*)malloc(AmountOfStrings*sizeof(int));
      MaxStringlength = 0;
      for(i=0; i<AmountOfStrings; i++)
      {
        StringlenStringToDraw[i] = strlen(inStringToDraw[i]);
        if(StringlenStringToDraw[i] > MaxStringlength)
        {
          MaxStringlength = StringlenStringToDraw[i]; // Global variable "MaxStringlength"
        }
      }    

      // ##### Create a copy of the input strings
      StringToDraw = (char**)malloc(AmountOfStrings*sizeof(char*));
      for(i=0; i<AmountOfStrings; i++)
      {
        StringToDraw[i] = (char*)malloc((MaxStringlength+1)*sizeof(char));
      }

      for(i=0; i<AmountOfStrings; i++)
      {
        for(j=0; j<(MaxStringlength+1); j++)
        {
          StringToDraw[i][j] = '\0';    
        }
      }

      for(i=0; i<AmountOfStrings; i++)
      {
        strcpy(StringToDraw[i], inStringToDraw[i]);
      }

      // ##### Initialize szene
      InitSzene(INIT, StringNo);

    }

	// ##### Copy-Constructor (copy ctor)
	// NA

	// ##### Destructor (dtor)
	// NA

	// ##### Assignment operator
	// NA

	// ##### Public member methods
	void InitSzene(int, int);
	void ShiftDrawingArrayRight();
	void ShiftDrawingArrayLeft();
	void ShiftDrawingArrayUp();
	void ShiftDrawingArrayDown();
  void ColorScene0(int);
  void ColorScene1(int);
  void ColorScene2(int);

  // ##### Public member variables (Attribute)
  byte *DrawPixelArray; // One-dimensional array to be drawen. Needed to set the color of each pixel. Method setPixel in OctoWS2811
  int *ColorPixel;    // One-dimensional. This array can be used to set the color of each pixel

private:
	// ##### Private member methods
	void InitializeLetters(byte***, const char*, int);
	void WriteLetterToDrawingMatrix(byte***, byte**, int);
	void DrawingMatrixToDrawingArray(byte[], byte**, int);

	// ##### Private member variables
  int AmountOfPixelsDrawingArray;
  int AmountOfColumnMatrix;
  int AmountOfStrings;
  int MaxStringlength;
  int AmountOfPixelsCompleteString;

	// ##### Private dynamic allocated arrays
	byte ***drawingLetters;  // Three-dimensional array for all letters (level), each letter has 8 line and 6 column (line vertical, column horizontal)
	byte **drawingMatrix;    // Two dimensional array for all letters, 8 lines and X columns (line vertical, column horizontal)
	byte *drawingArray;      // One-dimensional array with all the pixels of the string
	int *StringlenStringToDraw; // One-dimensional array with the stringlength of the strings to be drawen
  char **StringToDraw;        // Two dimensional char array, copy of the input strings

};
#endif // LEDMatrix8_h
