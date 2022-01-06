/*
""" ############################################################################################ """
""" ############################################################################################ """
""" LED matrix (8*X) WS2812B control with Teensy4.0 """
""" V1_00 2021-12-26, Armin Rehberger """
""" Libary cpp-file """
""" ############################################################################################ """
""" ############################################################################################ """
*/
#include <Arduino.h>
#include "LEDMatrix8.h"

// ###############################################################################################################
// ###############################################################################################################
// #####  Code of constructor in LEDMatrix8.h, because of template to pass dynamic char array.
/*
// LEDMatrix8::LEDMatrix8(const char inStringToDraw[][500], int inAmountOfPixelsDrawingArray)
{
}
*/

// ###############################################################################################################
// ###############################################################################################################
// ##### Initialize szene
void LEDMatrix8::InitSzene(int inInitOrRedraw, int inActiveStringNo)
{
  // ##### Local variables
  int level, line, column;
  int i, j, k;
  int AmountMatrixColumn;
  int AmountLetters;
  int StringNo;

  // ##### Create and clear the dynamic arrays during first call of this function
  if(inInitOrRedraw == INIT)
  {

    // ##### Amount of columns needed for the max string length
    AmountMatrixColumn = MaxStringlength * AMOUNTCOLUMNLETTER; // Matrix, needed columns for the max string length

    // ##### Three-dimensional array for X letters (level), each letter has 8 line and 6 column (line vertical, column horizontal)
    level = MaxStringlength;
    line = AMOUNTLINESLETTER;
    column = AMOUNTCOLUMNLETTER;
  
    drawingLetters=(byte***)malloc(level*sizeof(byte***));
    for(i=0;i<level;i++) 
    {
      drawingLetters[i]=(byte**)malloc(line*sizeof(byte*));
      for(j=0;j<line;j++) 
        drawingLetters[i][j]=(byte*)malloc(column*sizeof(byte));
    }
  
    for(i=0;i<level;i++) 
      for(j=0;j<line;j++) 
        for(k=0;k<column;k++) 
          drawingLetters[i][j][k] = 0;
  
    // ##### Two-dimensional array for LED-matrix, 8 lines and X column (line vertical, column horizontal)
    line = AMOUNTLINESLETTER;
    column = AmountMatrixColumn;
    drawingMatrix = (byte**)malloc(line*sizeof(byte*));
    for(i=0; i<line; i++)
    {
      drawingMatrix[i] = (byte*)malloc(column*sizeof(byte));
    }
    for(i=0; i<line; i++)
    {
      for(j=0; j<column; j++)
      {
        drawingMatrix[i][j] = 0;    
      }
    }
    
    // ##### One-dimensional array with all the pixels of the string
    line = AmountMatrixColumn * AMOUNTLINESLETTER;
    drawingArray = (byte*)malloc(line*sizeof(byte));
    for(i=0; i<line; i++)
    {
      drawingArray[i] = 0;
    }
  
    // ##### One-dimensional array to be drawen
    line = AmountOfPixelsDrawingArray;
    DrawPixelArray = (byte*)malloc(line*sizeof(byte));
    for(i=0; i<line; i++)
    {
      DrawPixelArray[i] = 0;
    }

    // ##### One-dimensional. This array can be used to set the color of each pixel
    line = AmountOfPixelsDrawingArray;
    ColorPixel = (int*)malloc(line*sizeof(int));
    for(i=0; i<line; i++)
    {
      ColorPixel[i] = 0;
    }
    
  } // if(inInitOrRedraw == INIT)

  // ##### Clear the dynamic arrays when calling this function with REDRAW
  if(inInitOrRedraw == REDRAW)
  {
    // ##### Amount of columns needed for the max string length
    AmountMatrixColumn = MaxStringlength * AMOUNTCOLUMNLETTER; // Matrix, needed columns for the max string length

    // ##### Three-dimensional array for X letters (level), each letter has 8 line and 6 column (line vertical, column horizontal)
    level = MaxStringlength;
    line = AMOUNTLINESLETTER;
    column = AMOUNTCOLUMNLETTER;
    for(i=0;i<level;i++) 
      for(j=0;j<line;j++) 
        for(k=0;k<column;k++) 
          drawingLetters[i][j][k] = 0;

    // ##### Two-dimensional array for LED-matrix, 8 lines and X column (line vertical, column horizontal)
    line = AMOUNTLINESLETTER;
    column = AmountMatrixColumn;
    for(i=0; i<line; i++)
    {
      for(j=0; j<column; j++)
      {
        drawingMatrix[i][j] = 0;    
      }
    }

    // ##### One-dimensional array with all the pixels of the string
    line = AmountMatrixColumn * AMOUNTLINESLETTER;
    for(i=0; i<line; i++)
    {
      drawingArray[i] = 0;
    }
  
    // ##### One-dimensional array to be drawen
    line = AmountOfPixelsDrawingArray;
    for(i=0; i<line; i++)
    {
      DrawPixelArray[i] = 0;
    }
  } // if(inInitOrRedraw == REDRAW)

  // ##### Transfer active string no.
  if(inActiveStringNo <= AmountOfStrings)
  {
    StringNo = inActiveStringNo;
  }
  else
  {
    StringNo = 0;
  }

  // ##### Amount of letters to be drawen
  AmountLetters = strlen(StringToDraw[StringNo]);
  AmountMatrixColumn = AmountLetters * AMOUNTCOLUMNLETTER; // Matrix, needed columns for the letters
  AmountOfPixelsCompleteString = AmountMatrixColumn * AMOUNTLINESLETTER; // Global variable "AmountOfPixelsCompleteString"

  // ##### Call function to initialize letters (Letters to a three dimensional array)
  InitializeLetters(drawingLetters, StringToDraw[StringNo], AmountLetters);

  // ##### Call function to write the letters (Three dimensional array) to a matrix (Two dimensional array)
  WriteLetterToDrawingMatrix(drawingLetters, drawingMatrix, AmountLetters);

  // ##### Call function to copy the matrix (Two dimensional array) to a drawing array (One dimensional array)
  DrawingMatrixToDrawingArray(drawingArray, drawingMatrix, AmountOfPixelsCompleteString);

  // ###### Copy the drawingArray to DrawPixelArray
  for(i=0; i<AmountOfPixelsDrawingArray; i++)
  {
    DrawPixelArray[i] = drawingArray[i];
  }

}

// ###############################################################################################################
// ###############################################################################################################
// ##### Write letter to drawing matrix
void LEDMatrix8::WriteLetterToDrawingMatrix(byte ***inDrawingLetters, byte **inDrawingMatrix, int inStringlen)
{
  // int inDrawingLetters[X][8][6]; // Three-dimensional array for all letters (level), each letter has 8 line and 6 column (line vertical, column horizontal)
  // int inDrawingMatrix[8][x]; // Two dimensional array for all letters, 8 lines and X columns (line vertical, column horizontal)
  // ##### Local variables
  int line;
  int column;
  int level;
  int offsetcolumn;

  // Write the letters
  offsetcolumn = 0;
  for (level=0; level<inStringlen; level++)
  {
    for(line = 0; line<AMOUNTLINESLETTER; line++)
    {
      for(column = 0; column<AMOUNTCOLUMNLETTER; column++)
      {
        inDrawingMatrix[line][column + offsetcolumn] = inDrawingLetters[level][line][column];
      }
    }
    offsetcolumn = offsetcolumn + AMOUNTCOLUMNLETTER;
  }
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Drawing matrix (Two dimensional array) to drawing array (One dimensional array)
void LEDMatrix8::DrawingMatrixToDrawingArray(byte inDrawingArray[], byte **inDrawingMatrix, int inAmountOfPixelsCompleteString)
{
  // ##### Local variables
  int i;
  int line = 0;
  int column = 0;
  bool SetCountDirection = false;
  bool CountPositive = true;
  
  for(i = 0; i<inAmountOfPixelsCompleteString; i++)
  {

    // Set count direction
    if(SetCountDirection == true)
    {
      SetCountDirection = false;
      if(line >= 8)
      {
        CountPositive = false;
        line = 7;
      }
      else
      {
        CountPositive = true;
        line = 0;       
      }
    }

    // Set pixel
    inDrawingArray[i] = inDrawingMatrix[line][column];

    // Count positive
    if(CountPositive == true)
    {
      line++;
      if(line>=8)
      {
        SetCountDirection = true;
        column++;
      }
    }

    // Count negative
    if(CountPositive == false)
    {
      line--;
      if(line<=-1)
      {
        SetCountDirection = true;
        column++;
      }
    }
    
  } // for(i = 0; i<inAmountOfPixelsCompleteString; i++)
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Shift the drawing array right by one column
// Shift drawing array right (One dimensional array) by 15, 13, 11, ...1, 15, 13, 11, ...1 positions
void LEDMatrix8::ShiftDrawingArrayRight()
{
  // ##### Local variables
  int drawingArraySafe[8];
  int i;
  int j;
  int countArrayNumber;
  
  // ##### Safe the last 8 positions
  for(i = AmountOfPixelsCompleteString-1, j=0; i>=AmountOfPixelsCompleteString-8; i--, j++)
  {
    drawingArraySafe[j] = drawingArray[i];
  }

  // ##### Shift array by 15, 13, 11, ...1, 15, 13, 11, ...1 positions
  countArrayNumber = 15;
  for(i = AmountOfPixelsCompleteString-1; i>=0; i--)
  {
    drawingArray[i] = drawingArray[i-countArrayNumber];
    countArrayNumber -=2;
    if(countArrayNumber <1)
    {
      countArrayNumber = 15;
    }
  }

  // ##### Restore the first 8 positions
  for(i=0; i<8; i++)
  {
    drawingArray[i] = drawingArraySafe[i];
  }

  // ###### Copy the drawingArray to DrawPixelArray
  for(i=0; i<AmountOfPixelsDrawingArray; i++)
  {
    DrawPixelArray[i] = drawingArray[i];
  }

}

// ###############################################################################################################
// ###############################################################################################################
// ##### Shift the drawing array left by one column
// Shift drawing array left (One dimensional array) by 1, 3, 5, 7, 9, ...15, 1, 3, 5, 7, 9, ...15 positions
void LEDMatrix8::ShiftDrawingArrayLeft()
{
  // ##### Local variables
  int drawingArraySafe[8];
  int i;
  int j;
  int countArrayNumber;
  
  // ##### Safe the first 8 positions
  for(i=0; i<8; i++)
  {
    drawingArraySafe[i] = drawingArray[i];
  }

  // ##### Shift array by 1, 3, 5, 7, 9, ...15, 1, 3, 5, 7, 9, ...15 positions
  countArrayNumber = 1;
  for(i = 8; i<AmountOfPixelsCompleteString; i++)
  {
    drawingArray[i-countArrayNumber] = drawingArray[i];
    countArrayNumber +=2;
    if(countArrayNumber >15)
    {
      countArrayNumber = 1;
    }
  }

  // ##### Restore the last 8 positions
  for(i = AmountOfPixelsCompleteString-1, j=0; i>=AmountOfPixelsCompleteString-8; i--, j++)
  {
    drawingArray[i] = drawingArraySafe[j];
  }

  // ###### Copy the drawingArray to DrawPixelArray
  for(i=0; i<AmountOfPixelsDrawingArray; i++)
  {
    DrawPixelArray[i] = drawingArray[i];
  }
  
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Shift the drawing array up by one line
// Shift drawing array up (One dimensional array)
void LEDMatrix8::ShiftDrawingArrayUp()
{
  // ##### Local variables
  int i, j;
  int SaveUpperPosition;
  bool CountPositive = true;
  int FirstArrayIndexLine;
  int LastArrayIndexLine;

  // ##### Go through each column
  FirstArrayIndexLine = 7;
  LastArrayIndexLine = 1;
  for(i = 0; i<AmountOfPixelsCompleteString / 8; i++)
  {
    // ##### Count positive
    //FirstArrayIndexLine = 7;
    //LastArrayIndexLine = 1;
    if(CountPositive == true)
    {
      // Save the upper position
      SaveUpperPosition = drawingArray[LastArrayIndexLine-1]; // Save position 0
  
      // Shift column 1 times up
      for(j=LastArrayIndexLine; j<=FirstArrayIndexLine; j++)
      {
        drawingArray[j-1] = drawingArray[j]; // Position 0 <- 1 / 1 <- 2 / ... 6 <- 7
      }
  
     // Restore the upper position to the lowest position
     drawingArray[FirstArrayIndexLine] = SaveUpperPosition; // Restore pos. 0 to pos. 7

    // Set index for next row
    FirstArrayIndexLine = FirstArrayIndexLine + 1;
    LastArrayIndexLine = LastArrayIndexLine + 13 ;
    }

    // ##### Count negative
    //FirstArrayIndexLine = 8;
    //LastArrayIndexLine = 14;
    if(CountPositive == false)
    {
      // Save the upper position
      SaveUpperPosition = drawingArray[LastArrayIndexLine+1]; // Save position 15
  
      // Shift column 1 times up
      for(j=LastArrayIndexLine; j>=FirstArrayIndexLine; j--)
      {
        drawingArray[j+1] = drawingArray[j]; // Position 15 <- 14 / 14 <- 13 / ... 9 <- 8
      }
  
     // Restore the upper position to the lowest position
     drawingArray[FirstArrayIndexLine] = SaveUpperPosition; // Restore pos. 15 to pos. 8

    // Set index for next row
    FirstArrayIndexLine = FirstArrayIndexLine + 15;
    LastArrayIndexLine = LastArrayIndexLine + 3 ;
    }

    // Toggle count direction
    CountPositive = ! CountPositive;
  }

  // ###### Copy the drawingArray to DrawPixelArray
  for(i=0; i<AmountOfPixelsDrawingArray; i++)
  {
    DrawPixelArray[i] = drawingArray[i];
  }
  
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Shift the drawing array down by one line
// Shift drawing array down (One dimensional array)
void LEDMatrix8::ShiftDrawingArrayDown()
{
  // ##### Local variables
  int i, j;
  int SaveUpperPosition;
  bool CountPositive = true;
  int FirstArrayIndexLine;
  int LastArrayIndexLine;

  // ##### Go through each column
  FirstArrayIndexLine = 0;
  LastArrayIndexLine = 6;
  for(i = 0; i<AmountOfPixelsCompleteString / 8; i++)
  {
    // ##### Count positive
    //FirstArrayIndexLine = 0;
    //LastArrayIndexLine = 6;
    if(CountPositive == true)
    {
      // Save the down position
      SaveUpperPosition = drawingArray[LastArrayIndexLine+1]; // Save position 7
  
      // Shift column 1 times down
      for(j=LastArrayIndexLine; j>=FirstArrayIndexLine; j--)
      {
        drawingArray[j+1] = drawingArray[j]; // Position 7 <- 6 / 6 <- 5 / ... 1 <- 0
      }
  
     // Restore the upper position to the lowest position
     drawingArray[FirstArrayIndexLine] = SaveUpperPosition; // Restore pos. 7 to pos. 0

    // Set index for next row
    FirstArrayIndexLine = FirstArrayIndexLine + 15;
    LastArrayIndexLine = LastArrayIndexLine + 3 ;
    }

    // ##### Count negative
    //FirstArrayIndexLine = 15;
    //LastArrayIndexLine = 9;
    if(CountPositive == false)
    {
      // Save the down position
      SaveUpperPosition = drawingArray[LastArrayIndexLine-1]; // Save position 8
  
      // Shift column 1 times down
      for(j=LastArrayIndexLine; j<=FirstArrayIndexLine; j++)
      {
        drawingArray[j-1] = drawingArray[j]; // Position 8 <- 9 / 9 <- 10 / ... 14 <- 15
      }
  
     // Restore the upper position to the lowest position
     drawingArray[FirstArrayIndexLine] = SaveUpperPosition; // Restore pos. 8 to pos. 15

    // Set index for next row
    FirstArrayIndexLine = FirstArrayIndexLine + 1;
    LastArrayIndexLine = LastArrayIndexLine + 13 ;
    }

    // Toggle count direction
    CountPositive = ! CountPositive;
  }

  // ###### Copy the drawingArray to DrawPixelArray
  for(i=0; i<AmountOfPixelsDrawingArray; i++)
  {
    DrawPixelArray[i] = drawingArray[i];
  }
  
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Color scene 0
// Whole matrix inCounterSwitchScene times BLUE, then inCounterSwitchScene times GREEN, then inCounterSwitchScene times RED, ...YELLOW PINK ORANGE WHITE
void LEDMatrix8::ColorScene0(int inCounterSwitchScene)
{
  // ##### Local variables
  static int CounterSwitchScene = inCounterSwitchScene;
  static int SceneNo = 0;
  int MemoryColorPixel = BACKGROUNDCOLUR;
  int CounterPixelNo = 0;
  int i, j;

  // ##### Calculate the pixels color
  CounterSwitchScene++;
  if (CounterSwitchScene >= inCounterSwitchScene)
  {
    CounterSwitchScene = 0;

    // ##### Switch color
    switch(SceneNo)
    {
      case 0:
        MemoryColorPixel = BLUE;
        SceneNo++;
      break;
  
      case 1:
        MemoryColorPixel = GREEN;
        SceneNo++;
      break;
  
      case 2:
        MemoryColorPixel = RED;
        SceneNo++;
      break;
  
      case 3:
        MemoryColorPixel = YELLOW;
        SceneNo++;
      break;
  
      case 4:
        MemoryColorPixel = PINK;
        SceneNo++;
      break;
  
      case 5:
        MemoryColorPixel = ORANGE;
        SceneNo++;
      break;
  
      case 6:
        MemoryColorPixel = WHITE;
        SceneNo = 0;
      break;
  
      default:
        MemoryColorPixel = BACKGROUNDCOLUR;
      break;   
    }

    // ##### Write the color to array ColorPixel[]
    for (i=0; i < AmountOfColumnMatrix; i++)
    {
      for(j=0; j<AMOUNTLINESLETTER; j++)
      {
        ColorPixel[CounterPixelNo] = MemoryColorPixel;
        CounterPixelNo++;
      }
    }
  }
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Color scene 1
// Whole matrix blue 1..inMaxIntense..1, green 1..inMaxIntense..1, red 1..inMaxIntense..1
// In value max intense, max. 255
void LEDMatrix8::ColorScene1(int inMaxIntense)
{
  // ##### Local variables
  static int SceneNo = 0;
  int MemoryColorPixel;
  static int Blue = 0;
  static int Green = 0;
  static int Red = 0;
  int CounterPixelNo = 0;
  int i, j;

  // ##### Max. value intense = 255
  if(inMaxIntense >255)
    inMaxIntense = 255;

  // ##### Switch color
  switch(SceneNo)
  {
    case 0:
      Red = 0;
      Blue++;
      if(Blue >=inMaxIntense)
        SceneNo++;
    break;

    case 1:
      Blue--;
      if(Blue <=1)
        SceneNo++;
    break;

    case 2:
      Blue = 0;
      Green++;
      if(Green >=inMaxIntense)
        SceneNo++;
    break;

    case 3:
      Green--;
      if(Green <=1)
        SceneNo++;
    break;

    case 4:
      Green = 0;
      Red++;
      if(Red >=inMaxIntense)
        SceneNo++;
    break;

    case 5:
      Red--;
      if(Red <=1)
        SceneNo = 0;
    break;

    default:
    break;   
  }

  // ##### Transfer color
  MemoryColorPixel = Blue;
  MemoryColorPixel = MemoryColorPixel | (Green<<8);
  MemoryColorPixel = MemoryColorPixel | (Red<<16);

  // ##### Write the color to array ColorPixel[]
  for (i=0; i < AmountOfColumnMatrix; i++)
  {
    for(j=0; j<AMOUNTLINESLETTER; j++)
    {
      ColorPixel[CounterPixelNo] = MemoryColorPixel;
      CounterPixelNo++;
    }
  }
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Color scene 2
// Whole matrix inValueColor
void LEDMatrix8::ColorScene2(int inValueColor)
{
  // ##### Local variables
  int CounterPixelNo = 0;
  int i, j;

  // ##### Write the color to array ColorPixel[]
  for (i=0; i < AmountOfColumnMatrix; i++)
  {
    for(j=0; j<AMOUNTLINESLETTER; j++)
    {
      ColorPixel[CounterPixelNo] = inValueColor;
      CounterPixelNo++;
    }
  }
  
}

// ###############################################################################################################
// ###############################################################################################################
// ##### Initialize letters
// int inDrawingLetters[X][8][6]; // Three-dimensional array for all letters (level), each letter has 8 line and 6 column (line vertical, column horizontal)
void LEDMatrix8::InitializeLetters(byte ***inDrawingLetters, const char* inStringToDraw, int inStringlen)
{
  // ##### Local variables
  char Letter[2];
  int i;
  int level;

  level = 0;
  for (i=0; i<inStringlen; i++)
  {
    Letter[0] = inStringToDraw[i];

    // ##### Write letters to three-dimensional array "inDrawingLetters"
    switch (Letter[0])
    {
      case 'A':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;      
      level++;
      break;

      case 'B':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;  
      level++;
      break;

      case 'C':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0; 
      level++;
      break;

      case 'D':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'E':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'F':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'G':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'H':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'I':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 1;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'J':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'K':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 1;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 1;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'L':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'M':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 1;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 1;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'N':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 1;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'O':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'P':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'Q':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 1;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'R':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 1;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'S':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'T':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 1;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'U':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'V':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 1;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 1;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'W':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 1;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 1;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'X':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 1;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 1;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'Y':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 1;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case 'Z':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 1;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '1':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 1;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 1;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '2':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 1;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 1;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 1;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '3':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '4':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 1;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 1;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 1;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 1;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '5':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '6':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '7':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 1;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '8':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '9':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '0':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 1;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '!':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '?':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '.':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case ':':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '+':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 1;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '-':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '=':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 1;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 1;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case '#':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 1;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 1;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 1;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 1;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 1;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 1;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      case ' ':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      // Haert, part 1
      case '{':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 1;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 1;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 1;
      inDrawingLetters[level][1][4] = 1;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 1;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 1;
      inDrawingLetters[level][3][5] = 1;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 1;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 1;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 1;
      inDrawingLetters[level][5][5] = 1;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 1;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      // Haert, part 2
      case '}':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 1;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 1;
      inDrawingLetters[level][1][1] = 1;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 1;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 1;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 1;
      inDrawingLetters[level][3][1] = 1;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 1;
      inDrawingLetters[level][4][1] = 1;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      // Smily, part 1
      case '[':
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 1;
      inDrawingLetters[level][0][5] = 1;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 1;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 1;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 1;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 1;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 1;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 1;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 1;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 1;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 1;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 0; // Line 7
      inDrawingLetters[level][7][1] = 0;
      inDrawingLetters[level][7][2] = 0;
      inDrawingLetters[level][7][3] = 0;
      inDrawingLetters[level][7][4] = 1;
      inDrawingLetters[level][7][5] = 1;
      level++;
      break;

      // Smily, part 2
      case ']':
      // line, column, level
      inDrawingLetters[level][0][0] = 1;
      inDrawingLetters[level][0][1] = 1;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 1;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 1;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 1;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 1;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 1;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 1;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 1;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 1;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 1;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 1; // Line 7
      inDrawingLetters[level][7][1] = 1;
      inDrawingLetters[level][7][2] = 0;
      inDrawingLetters[level][7][3] = 0;
      inDrawingLetters[level][7][4] = 0;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;

      default:
      // line, column, level
      inDrawingLetters[level][0][0] = 0;
      inDrawingLetters[level][0][1] = 0;
      inDrawingLetters[level][0][2] = 0;
      inDrawingLetters[level][0][3] = 0;
      inDrawingLetters[level][0][4] = 0;
      inDrawingLetters[level][0][5] = 0;
      inDrawingLetters[level][1][0] = 0;
      inDrawingLetters[level][1][1] = 0;
      inDrawingLetters[level][1][2] = 0;
      inDrawingLetters[level][1][3] = 0;
      inDrawingLetters[level][1][4] = 0;
      inDrawingLetters[level][1][5] = 0;
      inDrawingLetters[level][2][0] = 0;
      inDrawingLetters[level][2][1] = 0;
      inDrawingLetters[level][2][2] = 0;
      inDrawingLetters[level][2][3] = 0;
      inDrawingLetters[level][2][4] = 0;
      inDrawingLetters[level][2][5] = 0;
      inDrawingLetters[level][3][0] = 0;
      inDrawingLetters[level][3][1] = 0;
      inDrawingLetters[level][3][2] = 0;
      inDrawingLetters[level][3][3] = 0;
      inDrawingLetters[level][3][4] = 0;
      inDrawingLetters[level][3][5] = 0;
      inDrawingLetters[level][4][0] = 0;
      inDrawingLetters[level][4][1] = 0;
      inDrawingLetters[level][4][2] = 0;
      inDrawingLetters[level][4][3] = 0;
      inDrawingLetters[level][4][4] = 0;
      inDrawingLetters[level][4][5] = 0;
      inDrawingLetters[level][5][0] = 0;
      inDrawingLetters[level][5][1] = 0;
      inDrawingLetters[level][5][2] = 0;
      inDrawingLetters[level][5][3] = 0;
      inDrawingLetters[level][5][4] = 0;
      inDrawingLetters[level][5][5] = 0;
      inDrawingLetters[level][6][0] = 0;
      inDrawingLetters[level][6][1] = 0;
      inDrawingLetters[level][6][2] = 0;
      inDrawingLetters[level][6][3] = 0;
      inDrawingLetters[level][6][4] = 0;
      inDrawingLetters[level][6][5] = 0;
      inDrawingLetters[level][7][0] = 2; // Line 7
      inDrawingLetters[level][7][1] = 2;
      inDrawingLetters[level][7][2] = 2;
      inDrawingLetters[level][7][3] = 2;
      inDrawingLetters[level][7][4] = 2;
      inDrawingLetters[level][7][5] = 0;
      level++;
      break;     
    }
  }
}
