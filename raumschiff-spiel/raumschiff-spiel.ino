#include <LiquidCrystal.h>

const int ROW_LENGTH = 15;
const int COMET_APPEAR_CHANCE = 3;
const int COMET_RECENCY_DELAY = 2;
const int BUTTON_PRESSED_THRESHOLD = 1000; // Adjust if needed
const int MAX_UPDATE_INTERVAL = 32;
const int LEVEL_MULTIPLIER = 2;
const int LEVEL_UP_THRESHOLD = 10;
const int NO_UPDATE_LIMIT = 10;
const char* END_MESSAGE = "Try again!";
const char* RESTART_MESSAGE = "Starting again";

enum PinAssignments {
    RESETPIN = A2,
    MOVEUP = A1,
    MOVEDOWN = A0,
    NOISE_PIN = 13,
    LCD_RS = 12,
    LCD_ENABLE = 11,
    LCD_D4 = 5,
    LCD_D5 = 4,
    LCD_D6 = 3,
    LCD_D7 = 2
};

LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

struct Row {
    char data[ROW_LENGTH + 1];

    Row() { clear(); }

    void clear() {
        memset(data, ' ', ROW_LENGTH);
        data[ROW_LENGTH] = '\0';
    }

    void shiftLeft() {
        memmove(data, data + 1, ROW_LENGTH - 1);
    }

    void add(char c) {
        data[ROW_LENGTH - 1] = c;
    }

    char get(int index) const {
        return data[index];
    }

    void set(int index, char value) {
        data[index] = value;
    }
};

Row upperRow;
Row previousUpperRow;
Row bottomRow;
Row previousBottomRow;
const char emptySpace = ' ';
const char comet = '*';
const char spaceShip = '>';
bool isDead = false;
bool isUpper = true;
int level = 1;
int passedComets = 0;
int noUpdateCounter = 0;

void setup() {
    initializeGame();
}

void loop() {
    if (analogRead(RESETPIN) > BUTTON_PRESSED_THRESHOLD) {
        resetGame();
        isUpper = true;
    }

    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime >= (MAX_UPDATE_INTERVAL - LEVEL_MULTIPLIER * level)) {
        lastUpdateTime = millis();

        if (noUpdateCounter == NO_UPDATE_LIMIT){
          checkForCollision();
        }

        if (isDead) {
            displayEndScreen();
        } else {
            updateUniverse();
            navigateSpaceship();
            displayRows(upperRow, previousUpperRow, bottomRow, previousBottomRow);
        }
    }
}

void initializeGame() {
    lcd.begin(16, 2);
    randomSeed(analogRead(NOISE_PIN));
    pinMode(RESETPIN, INPUT);
    pinMode(MOVEUP, INPUT);
    pinMode(MOVEDOWN, INPUT);
    resetGame();
}

void checkForCollision() {
    if ((isUpper && upperRow.get(1) == comet) ||
        (!isUpper && bottomRow.get(1) == comet)) {
        isDead = true;
    }
}

void advanceState(char nextUpper, char nextBottom) {
    if (upperRow.get(0) == comet || bottomRow.get(0) == comet) {
        passedComets++;
        if (passedComets >= LEVEL_UP_THRESHOLD) {
            level++;
            passedComets = 0;
        }
        if (level == 10) {
          level = 1;
        }
    }
    updateRow(upperRow, nextUpper);
    updateRow(bottomRow, nextBottom);
}

void navigateSpaceship() {
    if (analogRead(MOVEUP) > BUTTON_PRESSED_THRESHOLD && !isUpper) {
        updateRow(bottomRow, emptySpace);
        isUpper = true;
    }
    if (analogRead(MOVEDOWN) > BUTTON_PRESSED_THRESHOLD && isUpper) {
        updateRow(upperRow, emptySpace);
        isUpper = false;
    }

    if (isUpper) {
        upperRow.set(0, spaceShip);
    } else {
        bottomRow.set(0, spaceShip);
    }
}

void updateUniverse() {
    static int recencyCounter = 0;

    if (noUpdateCounter < NO_UPDATE_LIMIT) {
        noUpdateCounter++;
    } else {
        if (recencyCounter == 0 && random(0, COMET_APPEAR_CHANCE) == 0) {
            if (random(0, 2) == 0) {
                advanceState(comet, emptySpace);
            } else {
                advanceState(emptySpace, comet);
            }
            recencyCounter = COMET_RECENCY_DELAY;
        } else {
            advanceState(emptySpace, emptySpace);
            recencyCounter = max(recencyCounter - 1, 0);
        }
        noUpdateCounter = 0;
    }
}

void updateRow(Row& row, char nextChar) {
    row.shiftLeft();
    row.add(nextChar);
}

void displayRows(Row& upperRow, Row& previousUpperRow, Row& bottomRow, Row& previousBottomRow) {
    for (int i = 0; i < ROW_LENGTH; i++) {
        if (upperRow.get(i) != previousUpperRow.get(i)) {
            lcd.setCursor(i, 0);
            lcd.print(upperRow.get(i));
            previousUpperRow.set(i, upperRow.get(i));
        }
    }
    lcd.setCursor(15, 0);
    lcd.print(level);

    for (int i = 0; i < ROW_LENGTH; i++) {
        if (bottomRow.get(i) != previousBottomRow.get(i)) {
            lcd.setCursor(i, 1);
            lcd.print(bottomRow.get(i));
            previousBottomRow.set(i, bottomRow.get(i));
        }
    }
    lcd.setCursor(15, 1);
    lcd.print(passedComets);
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
    lcd.print(RESTART_MESSAGE);
    delay(1000);
    upperRow.clear();
    bottomRow.clear();
    previousUpperRow.clear();
    previousBottomRow.clear();
    isDead = false;
    level = 1;
    passedComets = 0;
    lcd.clear();
}