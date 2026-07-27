#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

const short matrix_pin = 6;
const short pinX = A4;
const short pinY = A3;
const short rows = 8;
const short cols = 8;
const short numLeds = rows * cols;

// Геометрія матриці (1, 2, 3 або 4)
const short MATRIX_TYPE = 1;

short cur_X = 0;
short cur_Y = 0;

Adafruit_NeoPixel matrix(numLeds, matrix_pin, NEO_GRB + NEO_KHZ800);

// 0 = прохід, 1 = стіна, 2 = фініш, 3 = старт
short maze[rows][cols] = {
    {3, 0, 0, 0, 1, 1, 1, 0},
    {0, 1, 0, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 1, 0},
    {1, 1, 0, 1, 1, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0},
    {1, 0, 1, 0, 1, 1, 1, 0},
    {1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 1, 1, 1, 2},
};

unsigned long last_time = 0;

short getPixelIndex(short x, short y)
{
  switch (MATRIX_TYPE)
  {
  case 1:
    return y * cols + x;
  case 2:
    return (y % 2 == 0) ? (y * cols + x) : (y * cols + (cols - 1 - x));
  case 3:
    return x * rows + y;
  case 4:
    return (x % 2 == 0) ? (x * rows + y) : (x * rows + (rows - 1 - y));
  default:
    return y * cols + x;
  }
}

void setup()
{
  matrix.begin();
  matrix.setBrightness(20);
  matrix.clear();
  matrix.show();
}

void loop()
{
  if (millis() - last_time >= 150)
  {
    last_time = millis();

    // 1. Стабілізоване зчитування X
    analogRead(pinX);
    delayMicroseconds(10);
    int valX = analogRead(pinX);

    // 2. Стабілізоване зчитування Y
    analogRead(pinY);
    delayMicroseconds(10);
    int valY = analogRead(pinY);

    // Розрахунок сили відхилення від центра
    int devX = abs(valX - 512);
    int devY = abs(valY - 512);

    short moveX = 0;
    short moveY = 0;

    // Вибір однієї домінуючої осі з розширеним діапазоном
    if (devX > devY && devX > 150)
    {
      if (valX < 350)
        moveX = -1;
      else if (valX > 670)
        moveX = 1;
    }
    else if (devY >= devX && devY > 150)
    {
      if (valY < 350)
        moveY = -1;
      else if (valY > 670)
        moveY = 1;
    }

    // Рух по X
    if (moveX != 0)
    {
      short targetX = cur_X + moveX;
      if (targetX >= 0 && targetX < cols)
      {
        if (maze[cur_Y][targetX] != 1)
        {
          cur_X = targetX;
        }
      }
    }

    // Рух по Y
    if (moveY != 0)
    {
      short targetY = cur_Y + moveY;
      if (targetY >= 0 && targetY < rows)
      {
        if (maze[targetY][cur_X] != 1)
        {
          cur_Y = targetY;
        }
      }
    }

    matrix.clear();

    // Отрисовка стін і фінішу
    for (int r = 0; r < rows; r++)
    {
      for (int c = 0; c < cols; c++)
      {
        int pixelIndex = getPixelIndex(c, r);

        if (maze[r][c] == 1)
        {
          matrix.setPixelColor(pixelIndex, matrix.Color(255, 0, 0)); // Стіна
        }
        else if (maze[r][c] == 2)
        {
          matrix.setPixelColor(pixelIndex, matrix.Color(0, 255, 0)); // Фініш
        }
      }
    }

    // Малювання гравця
    matrix.setPixelColor(getPixelIndex(cur_X, cur_Y), matrix.Color(255, 255, 255));

    matrix.show();
  }
}