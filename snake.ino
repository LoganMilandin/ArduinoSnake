#include <FastLED.h>
#include "defs.h"

CRGB LEDs[NUM_LEDS];
const byte buttonPins[] = {2, 3, 4, 5};
Snake snake;
// Square board[ROWS][COLS];
Coordinate availableFoodPositions[NUM_LEDS];
Coordinate food;
bool gameOver = false;
long frameDelay = 200;

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(LEDs, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.clear();
  FastLED.show();
  // enable pins 0-7 to accept pin change interrupts
  PCICR |= B00000100;
  Serial.begin(9600);
  // Serial.println("started");
  initializeButtons(buttonPins);
  // initializeBoard(board);

  
  // Coordinate initialHead;
  // initialHead.set(8, 8);
  // snake.init(initialHead);
  // Serial.println(snake.body_size);

  // randomSeed(analogRead(0));
  food = generateFoodCoordinate(snake, availableFoodPositions);
  // Serial.println("(" + String(food.row) + ", " + String(food.col) + ")");
}

void loop() {
  delay(frameDelay);
  if (gameOver) {
    return;
  }
  // Serial.println(gameOver);
  // Serial.println(snake.body_size);
  Coordinate prevHead = snake.body[0];
  bool ateFood = snake.move(food);
  if (snake.body[0].data == prevHead.data && snake.direction != NONE) {
    gameOver = true;
  }
  if (ateFood) {
    food = generateFoodCoordinate(snake, availableFoodPositions);
  }

  updateBoard(snake, food, LEDs, gameOver);
  FastLED.show();
}

ISR (PCINT2_vect) {
  // Serial.println("pressed");
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      if (debounceCheck(i)) {
        handleButtonPressed(buttonPins[i]);
      }
    }
  }
}

void handleButtonPressed(byte pin) {
  // If direction is already locked, do not process further direction changes
  if (snake.directionLocked) {
    return;
  }

  Direction newDirection = snake.direction;

  // Assign one of the directions to the newDirection variable based on the pressed button
  switch (pin) {
    case 2:
      if (snake.direction != DOWN) 
        newDirection = UP;
      break;
    case 3:
      if (snake.direction != UP)
        newDirection = DOWN;
      break;
    case 4:
      if (snake.direction != RIGHT)
        newDirection = LEFT;
      break;
    case 5:
      if (snake.direction != LEFT)
        newDirection = RIGHT;
      break;
  }

  if (newDirection != snake.direction) {
    snake.direction = newDirection;
    snake.directionLocked = true;
  }
}
