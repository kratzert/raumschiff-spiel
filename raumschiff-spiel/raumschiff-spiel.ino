#include <LiquidCrystal.h>
#include <string.h>

#define RESETPIN A2
#define MOVEUP A1
#define MOVEDOWN A0
#define ROW_LENGTH 16
#define NOISE_PIN 13
#define COMET_APPEAR_CHANCE 3
#define COMET_RECENCY_DELAY 2
#define END_MESSAGE "Try again!"
#define RESTART_MESSAGE "Starting again"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

char upperRow[ROW_LENGTH + 1] = "                ";
char previousUpperRow[ROW_LENGTH + 1] = "                ";
char bottomRow[ROW_LENGTH + 1] = "                ";
char previousBottomRow[ROW_LENGTH + 1] = "                ";
char emptyRow[ROW_LENGTH + 1] = "                ";
char emptySpace[] = " ";
char comet[] = "*";
char spaceShip[] = ">";
bool isDead = false;
static bool isUpper = true;

void setup() {
  lcd.begin(16, 2);
  randomSeed(analogRead(NOISE_PIN));  // Set seed to random noise from empty pin
}

void loop() {

  if (analogRead(RESETPIN) == 1023) {
    resetGame();
    isUpper = true;
  }

  static unsigned long lastUpdateTime = 0; 
  if (millis() - lastUpdateTime >= 40) {
    lastUpdateTime = millis();

    checkForCollision();
    if (isDead) {
      displayEndScreen();
    } else {
      updateUniverse();
      navigateSpaceship();
      displayRows();  
    }
  }
}

void checkForCollision() {
  if ((isUpper && upperRow[1] == comet[0]) || 
      (!isUpper && bottomRow[1] == comet[0])) {
    isDead = true;
  }
}

void advanceState(char *nextUpper, char *nextBottom) {
  advanceRow(upperRow, nextUpper);
  advanceRow(bottomRow, nextBottom);
}

void navigateSpaceship() {
  if (analogRead(MOVEUP) == 1023 && !isUpper) {
    advanceRow(bottomRow, emptySpace);
    isUpper = true;
  } 
  if (analogRead(MOVEDOWN) == 1023 && isUpper) {
    advanceRow(upperRow, emptySpace);
    isUpper = false;
  }
  // Now add the spaceship to the correct row
  if (isUpper) {
    addSpaceShip(upperRow);
  } else {
    addSpaceShip(bottomRow);
  }
}

void updateUniverse() {
  static int recencyCounter = 0;
  static int noUpdateCounter = 0;
  if (noUpdateCounter < 10) {
    noUpdateCounter++;
  } else {
    if (recencyCounter == 0 && random(0, COMET_APPEAR_CHANCE) == 0) { 
      // Only create a comet if recencyCounter is 0 and random condition met
      if (random(0, 2) == 0) { // 50/50 chance for upper or lower row
        advanceState(comet, emptyRow); 
      } else {
        advanceState(emptyRow, comet);
      }
      recencyCounter = COMET_RECENCY_DELAY;
    } else {
        advanceState(emptyRow, emptyRow);
        recencyCounter = max(recencyCounter - 1, 0); 
    }
    noUpdateCounter = 0;
  }


}

void advanceRow(char *row, char *next) {
  memmove(row, row + 1, ROW_LENGTH - 1); // Shift characters to the left
  row[ROW_LENGTH - 1] = next[0]; // Add the new character
  row[ROW_LENGTH] = '\0'; // Ensure null termination
}

void addSpaceShip(char *row) {
  row[0] = spaceShip[0]; // Add the spaceship
  row[ROW_LENGTH] = '\0'; // Ensure null termination
}

void displayRows() {
  for (int i = 0; i < ROW_LENGTH; i++) {
    if (upperRow[i] != previousUpperRow[i]) {
      lcd.setCursor(i, 0);
      lcd.print(upperRow[i]);
      previousUpperRow[i] = upperRow[i];
    }
  }

  for (int i = 0; i < ROW_LENGTH; i++) {
    if (bottomRow[i] != previousBottomRow[i]) {
      lcd.setCursor(i, 1);
      lcd.print(bottomRow[i]);
      previousBottomRow[i] = bottomRow[i];
    }
  }
}

void displayEndScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(END_MESSAGE);
  delay(1000);
}

void resetGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(RESTART_MESSAGE);  // Show RESTART_MESSAGE for one second.
  delay(1000);
  strcpy(upperRow, emptyRow);
  strcpy(bottomRow, emptyRow);
  strcpy(previousUpperRow, emptyRow);
  strcpy(previousBottomRow, emptyRow);
  isDead = false;
  lcd.clear(); // Clear again to remove RESTART_MESSAGE from display.
}