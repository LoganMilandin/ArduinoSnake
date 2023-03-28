#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 9
#define NUM_LEDS 256
#define ROWS 16
#define COLS 16
#define NUM_BUTTONS 4

enum Direction {
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Coordinate {
  uint8_t data; // 4 bits for row, 4 bits for col

  uint8_t row() const { return (data >> 4) & 0x0F; }
  uint8_t col() const { return data & 0x0F; }

  void set(uint8_t newRow, uint8_t newCol) {
    data = (newRow << 4) | (newCol & 0x0F);
  }
};

class Snake {
  public:
    Direction direction = NONE;
    bool directionLocked = false; // New field // needed to avoid possibility of doubling back in the case of a rapid double press from the user
    Coordinate body[NUM_LEDS]; // at most, player can fill whole grid
    int body_size;

    Snake();

    // move and return true if we ate food, false otherwise
    bool move(Coordinate food);

    // return true if given coordinate is part of snake (head or body)
    bool occupiesPosition(const Coordinate& position);
};

bool isValidPosition(const Coordinate& position);
Coordinate generateFoodCoordinate(Snake& snake, Coordinate availableFoodPositions[]);
int coordToLEDIndex(const Coordinate coord);
void updateBoard(Snake& snake, const Coordinate& food, CRGB LEDs[], bool gameOver);
void printBoard(Snake& snake, const Coordinate& food);

void initializeButtons(byte* buttonPins);


