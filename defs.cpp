#include "defs.h"

Snake::Snake() {
  body[0].set(8, 8);
  body_size = 1;
}

bool Snake::move(Coordinate food) {
  directionLocked = false;
  // Calculate the new head position based on the current direction
  Coordinate new_head = body[0];
  uint8_t newRow = new_head.row();
  uint8_t newCol = new_head.col();

  // must check for game over before update because we will wrap around to 0
  // if we update from a wall
  if ((direction == UP && newRow == 0) ||
      (direction == DOWN && newRow == ROWS - 1) ||
      (direction == LEFT && newCol == 0) ||
      (direction == RIGHT && newCol == COLS - 1)) {
    return false;
  }


  switch (direction) {
    case UP:    newRow = newRow == 0 ? newRow : newRow - 1; break;
    case DOWN:  newRow = newRow == ROWS - 1 ? newRow : newRow + 1; break;
    case LEFT:  newCol = newCol == 0 ? newCol : newCol - 1; break;
    case RIGHT: newCol = newCol == COLS - 1 ? newCol : newCol + 1; break;
    default: return false; // If the direction is NONE, don't move
  }
    
  new_head.set(newRow, newCol);
  if (occupiesPosition(new_head)) {
    return false;
  }
  // Serial.println("(" + String(new_head.row()) + ", " + String(new_head.col()) + ")");



  // Shift all body elements to the right to make room for the new head
  for (int i = body_size; i > 0; i--) {
    body[i] = body[i - 1];
  }


  // Add the new head position at the front of the list
  body[0] = new_head;

  // if the head is now on food, leave the tail and increment our size.
  if (new_head.row() == food.row() && new_head.col() == food.col()) {
    body_size++;
    return true;
  } else {
    return false;
  }
}

bool Snake::occupiesPosition(const Coordinate& position) {
  for (int i = 0; i < body_size; i++) {
    if (body[i].row() == position.row() && body[i].col() == position.col()) {
      return true;
    }
  }
  return false;
}

bool isValidPosition(const Coordinate& position) {
  // Serial.println("(" + String(position.row()) + ", " + String(position.col()) + ")");
  return position.row() >= 0 && position.row() < ROWS && position.col() >= 0 && position.col() < COLS;
}

Coordinate generateFoodCoordinate(Snake& snake, Coordinate availableFoodPositions[]) {
  int count = 0;
  for (int row = 0; row < ROWS; ++row) {
    for (int col = 0; col < COLS; ++col) {
      Coordinate position;
      position.set(row, col);
      if (!snake.occupiesPosition(position)) {
        availableFoodPositions[count++] = position;
      }
    }
  }
  return availableFoodPositions[random(count)];
}

int coordToLEDIndex(const Coordinate coord) {
  return coord.row() * COLS + (coord.row() % 2 ? coord.col() : COLS - 1 - coord.col());
}

void updateBoard(Snake& snake, const Coordinate& food, CRGB LEDs[], bool gameOver) {
  for (int row = 0; row < ROWS; ++row) {
    for (int col = 0; col < COLS; ++col) {
      Coordinate currentPosition;
      currentPosition.set(row, col);
      int LEDIndex = coordToLEDIndex(currentPosition);
      if (snake.body[0].row() == currentPosition.row() && snake.body[0].col() == currentPosition.col()) {
        // Snake's head
        LEDs[LEDIndex] = gameOver ? CRGB(20, 0, 0) : CRGB(0, 20, 0);
      } else if (snake.occupiesPosition(currentPosition)) {
        // Snake's body
        LEDs[LEDIndex] = gameOver ? CRGB(3, 0, 0) : CRGB(0, 3, 0);
      } else if (food.row() == currentPosition.row() && food.col() == currentPosition.col()) {
        // Food
        LEDs[LEDIndex] = gameOver ? CRGB(0, 0, 0) : CRGB(0, 0, 20);
      } else {
        // Empty cell
        LEDs[LEDIndex] = CRGB(0, 0, 0);
      }
    }
  }
}

void printBoard(Snake& snake, const Coordinate& food) {
  for (int row = 0; row < ROWS; ++row) {
    for (int col = 0; col < COLS; ++col) {
      Coordinate currentPosition;
      currentPosition.set(row, col);

      if (snake.body[0].row() == currentPosition.row() && snake.body[0].col() == currentPosition.col()) {
        // Snake's head
        Serial.print("H ");
      } else if (snake.occupiesPosition(currentPosition)) {
        // Snake's body
        Serial.print("S ");
      } else if (food.row() == currentPosition.row() && food.col() == currentPosition.col()) {
        // Food
        Serial.print("F ");
      } else {
        // Empty cell
        Serial.print(". ");
      }
    }
    Serial.println(); // Move to the next row
  }
  Serial.println(); // Add an extra line to separate board states
}

void initializeButtons(byte* buttonPins) {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    // make sure to attach pullup resistors before enabling interrupts, otherwise
    // this triggers the interrupts a few times
    pinMode(buttonPins[i], INPUT_PULLUP);
    // turn on interrupts for specific pins
    PCMSK2 |= 1 << buttonPins[i];
  }
}
