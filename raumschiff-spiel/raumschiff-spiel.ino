#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
String upperRow = "                ";
String bottomRow = "                ";
String emptyRow = "                ";
String comet = "*";
char cometAsChar = '*';
String emptySpace = " ";
String spaceShip = ">";
bool isDead = false;
String EndMsg = "Try again!";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  randomSeed(analogRead(13));  // Set seed to random noise from empty pin

}

void loop() {

  static bool isUpper = true;

  int val = analogRead(A2);
  Serial.println(val);
  if (val == 1023) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Starting again");
    delay(1000);
    upperRow = emptyRow;
    bottomRow = emptyRow;
    isUpper = true;
    isDead = false;
  }

  checkForCollision(isUpper);
  if (isDead) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(EndMsg);
    delay(1000);
  } else {
      // put your main code here, to run repeatedly:
    int randVal = random(0, 3);
    static int recencyCounter = 0;
    if (randVal == 0 && recencyCounter == 0) {
      advanceState(comet, emptySpace);
      recencyCounter = 2;
    } else if (randVal == 1 && recencyCounter == 0){
      advanceState(emptySpace, comet);
      recencyCounter = 2;
    } else {
      advanceState(emptySpace, emptySpace);
      recencyCounter = max(recencyCounter--, 0);
    }

    val = analogRead(A0);
    if (val == 1023 && isUpper) {
      advanceRow(upperRow, emptySpace);
      addSpaceShip(bottomRow);
      isUpper = false;
    } else if (isUpper) {
      addSpaceShip(upperRow);
    }

    val = analogRead(A1);
    if (val == 1023 && !isUpper) {
      advanceRow(bottomRow, emptySpace);
      addSpaceShip(upperRow);
      isUpper = true;
    } else if (!isUpper) {
      addSpaceShip(bottomRow);
    }
    displayRows();
    delay(500);

  }

  
}

void checkForCollision(bool isUpper) {
  if (isUpper) {
    if (upperRow.charAt(1) == cometAsChar) {
      isDead = true;
    }
  } else {
    if (bottomRow.charAt(1) == cometAsChar) {
      isDead = true;
    }
  }
}

void advanceState(String &nextUpper, String &nextBottom) {
  advanceRow(upperRow, nextUpper);
  advanceRow(bottomRow, nextBottom);
}

void advanceRow(String &row, String &next) {
  row.remove(0,1);
  row = row + next;
}

void addSpaceShip(String &row) {
  row.remove(0,1);
  row = spaceShip + row;
}

void displayRows() {
  lcd.setCursor(0, 0);
  lcd.print(upperRow);
  lcd.setCursor(0, 1);
  lcd.print(bottomRow);
}
