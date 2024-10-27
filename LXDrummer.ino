#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <MD_MAX72xx.h>
#include "909.h"  // 909 drum kit
#include <I2S.h>
#include <SPI.h>
#include <pgmspace.h>
#include <AsyncTimer.h>

// Pin definitions for the rotary encoder
#define ENCODER_CLK 5
#define ENCODER_DT 4
#define ENCODER_SW 0

// Pin definitions for the LED matrix display
#define MATRIX_HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MATRIX_CLK 14
#define MATRIX_DT 13
#define MATRIX_CS 16

#define CLICK_INTERVAL 350  // Time interval for detecting multiple clicks

// Initial settings
float volume = 0.5;
bool isPaused = false;
bool isPatternMode = false;
int bpm = 110;
const int BPM_MIN = 50;   // Minimum BPM
const int BPM_MAX = 200;  // Maximum BPM
int beatStep = 0;
int patternStep = 0;
int bpmTimer;
int bpmAdjustmentTime = 0;
bool bpmDisplayState = true;
AsyncTimer timer;

// Point structure for cursor position
struct Point {
  int x;
  int y;
  Point(int x, int y)
    : x(x), y(y) {}
};

MD_MAX72XX matrix = MD_MAX72XX(MATRIX_HARDWARE_TYPE, MATRIX_CS, 1);

Point cursor(0, 0);
bool cursorDisplayState = true;

// Encoder states
int currentEncoderState;
int lastEncoderState;
unsigned long lastButtonPressTime = 0;
int clickCount = 0;
unsigned long lastClickTime = 0;

// Tracks and pattern buffers
bool tracks[8][6][8] = { { { 0 } } };
bool temporaryPattern[6][8] = { { 0 } };
bool isTemporaryPatternEmpty = true;

// Function to adjust BPM and update timer interval
void adjustBpm(int change) {
  bpm += change;
  bpm = constrain(bpm, BPM_MIN, BPM_MAX);

  // Update timer interval based on new BPM
  timer.cancel(bpmTimer);
  bpmTimer = timer.setInterval(tick, (60000 / bpm) / 4);
  Serial.print("BPM set to: ");
  Serial.println(bpm);

  // Update progress bar on the last row
  displayProgressBar();
  bpmAdjustmentTime = millis();
  isPaused = false;
}

// Function to display a progress bar based on BPM
void displayProgressBar() {
  int progress = map(bpm, BPM_MIN, BPM_MAX, 0, 7);  // Map BPM to progress bar length (0-7)
  for (int x = 0; x < 8; x++) {
    matrix.setPoint(x, 7, false);  // Clear the progress bar
  }
  for (int x = 0; x < progress; x++) {
    matrix.setPoint(x, 7, true);  // Display the progress bar
  }
}

// Function to display tracks on the LED matrix
void displayTracks() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 6; x++) {
      matrix.setPoint(y, x, tracks[patternStep][x][y]);
    }
  }
}

// Function to handle a single click
void handleClick() {
  if (cursor.y >= 0 && cursor.y <= 5) {
    if (!tracks[patternStep][cursor.y][cursor.x]) playSound(cursor.y);
    tracks[patternStep][cursor.y][cursor.x] = !tracks[patternStep][cursor.y][cursor.x];
  } else if (cursor.y == 6) {
    if (patternStep != cursor.x) {
      matrix.setPoint(patternStep, 6, false);
      patternStep = cursor.x;
    }
  } else if (cursor.y == 7) {
    isPaused = !isPaused;
  }
}

// Function to handle a double click
void handleDoubleClick() {
  if (cursor.y == 7) {
    isPatternMode = !isPatternMode;
  } else if (cursor.y == 6) {
    if (!isTemporaryPatternEmpty) {
      for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
          tracks[cursor.x][i][j] = temporaryPattern[i][j];
        }
      }
    }
  }
}

// Function to handle a triple click
void handleTripleClick() {
  if (cursor.y == 6) {
    isTemporaryPatternEmpty = true;
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 8; j++) {
        temporaryPattern[i][j] = tracks[cursor.x][i][j];
        if (tracks[cursor.x][i][j]) {
          isTemporaryPatternEmpty = false;
        }
      }
    }
  }
}

// Function to clear the cursor display
void clearCursor() {
  matrix.setPoint(cursor.x, cursor.y, false);
}

// Function to display the cursor
void displayCursor() {
  if (cursor.y != 7) matrix.setPoint(cursor.x, cursor.y, true);
}

// Function to increment the cursor position
void incrementCursor() {
  clearCursor();
  if (cursor.x == 7 && cursor.y == 6) {
    cursor.x = 0;
    cursor.y = 7;
    return;
  } else if (cursor.x + 1 <= 7) {
    cursor.x++;
  } else {
    if (cursor.y + 1 <= 6) {
      cursor.y++;
      cursor.x = 0;
    }
  }
  displayCursor();
}

// Function to decrement the cursor position
void decrementCursor() {
  clearCursor();
  if (cursor.y == 7) {
    cursor.y--;
    cursor.x = 7;
    bpmDisplayState = true;
  } else if (cursor.x - 1 >= 0) {
    cursor.x--;
  } else {
    if (cursor.y - 1 >= 0) {
      cursor.y--;
      cursor.x = 7;
    }
  }
  displayCursor();
}

// Function to play a sound based on the track number
void playSound(int track) {
  switch (track) {
    case 0:
      BD16CNT = 0;
      break;
    case 1:
      SD16CNT = 0;
      break;
    case 2:
      HH16CNT = 0;
      break;
    case 3:
      CP16CNT = 0;
      break;
    case 4:
      HT16CNT = 0;
      break;
    case 5:
      OH16CNT = 0;
      break;
  }
}

// Function to process tracks and play sounds for the current step
void processTracks(int step) {
  if (tracks[patternStep][0][step]) playSound(0);
  if (tracks[patternStep][1][step]) playSound(1);
  if (tracks[patternStep][2][step]) playSound(2);
  if (tracks[patternStep][3][step]) playSound(3);
  if (tracks[patternStep][4][step]) playSound(4);
  if (tracks[patternStep][5][step]) playSound(5);
}

// Timer tick function
void tick() {
  displayTracks();
  matrix.setPoint(patternStep, 6, true);
  if (cursorDisplayState) {
    clearCursor();
    cursorDisplayState = false;
  } else {
    displayCursor();
    cursorDisplayState = true;
  }

  if (!isPaused) processTracks(beatStep);
  if (millis() - bpmAdjustmentTime > 500) {
    for (int i = 0; i < 8; i++) {
      matrix.setPoint(i, 7, false);
    }

    if (!isPaused) {
      beatStep++;
      if (beatStep == 8) {
        beatStep = 0;
        if (isPatternMode) {
          matrix.setPoint(patternStep, 6, false);
          patternStep++;
          if (patternStep == 8) patternStep = 0;
        }
      }
    }

    if (cursor.y == 7) {
      bpmDisplayState = !bpmDisplayState;
    }

    if (bpmDisplayState) matrix.setPoint(beatStep, 7, true);
  }
}

// ISR for sound timer
void ICACHE_RAM_ATTR soundTimerISR() {
  while (!i2s_is_full()) {
    int sample = SYNTH909() * volume;
    i2s_write_lr(sample, sample);
  }
  timer1_write(2000);
}

// Setup function
void setup() {
  Serial.begin(9600);
  i2s_begin();
  i2s_set_rate(44100);
  matrix.begin();
  matrix.clear();
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  lastEncoderState = digitalRead(ENCODER_CLK);
  bpmTimer = timer.setInterval(tick, (60000 / bpm) / 4);

  timer1_attachInterrupt(soundTimerISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(2000);
}

// Main loop function
void loop() {
  timer.handle();

  int buttonState = digitalRead(ENCODER_SW);
  currentEncoderState = digitalRead(ENCODER_CLK);

  // Adjust BPM only when the button is pressed
  if (buttonState == LOW) {
    if (millis() - lastButtonPressTime > 50) {
      if (millis() - lastClickTime < CLICK_INTERVAL) {
        clickCount++;
      } else {
        clickCount = 1;
      }
      lastClickTime = millis();
    }
    lastButtonPressTime = millis();

    if (currentEncoderState != lastEncoderState && currentEncoderState == 1) {
      if (cursor.y == 7) {
        if (digitalRead(ENCODER_DT) != currentEncoderState) {
          adjustBpm(10);  // Increase BPM when rotating clockwise
        } else {
          adjustBpm(-10);  // Decrease BPM when rotating counter-clockwise
        }
      } else {
        if (digitalRead(ENCODER_DT) != currentEncoderState) {
          handleClick();
          incrementCursor();
        } else {
          handleClick();
          decrementCursor();
        }
      }
      clickCount = 0;
    }
  } else {
    // Only update the cursor position when the button is not pressed
    if (currentEncoderState != lastEncoderState && currentEncoderState == 1) {
      if (digitalRead(ENCODER_DT) != currentEncoderState) {
        incrementCursor();
      } else {
        decrementCursor();
      }
    } else if (clickCount == 1 && (millis() - lastClickTime) > CLICK_INTERVAL) {
      handleClick();
      clickCount = 0;
    } else if (clickCount == 2 && (millis() - lastClickTime) > CLICK_INTERVAL) {
      handleDoubleClick();
      clickCount = 0;
    } else if (clickCount == 3 && (millis() - lastClickTime) > CLICK_INTERVAL) {
      handleTripleClick();
      clickCount = 0;
    }
  }
  lastEncoderState = currentEncoderState;  // Update the last state of the encoder
}
