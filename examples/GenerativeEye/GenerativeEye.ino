/*
 * Generative Eye - Interactive Art for T-QT
 *
 * A living eye that watches, blinks, and glitches.
 * Press LEFT for "DIGITAL", RIGHT for "HUMANITY"
 *
 * Hardware: LilyGo T-QT Pro (ESP32-S3, GC9A01 128x128)
 */

#include <TFT_eSPI.h>
#include <SPI.h>
#include "OneButton.h"

// Display
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite eyeSprite = TFT_eSprite(&tft);

// Screen dimensions
#define SCREEN_W 128
#define SCREEN_H 128
#define CENTER_X 64
#define CENTER_Y 64

// Button pins
#define PIN_BTN_L 0
#define PIN_BTN_R 47

// Buttons
OneButton btnLeft(PIN_BTN_L, true, true);
OneButton btnRight(PIN_BTN_R, true, true);

// Colors - Grayscale palette for the eye
#define COL_BLACK      0x0000
#define COL_WHITE      0xFFFF
#define COL_DARK_GRAY  0x2104
#define COL_MID_GRAY   0x4208
#define COL_LIGHT_GRAY 0x8410
#define COL_SKIN       0xD69A  // Slightly warm gray for skin
#define COL_IRIS_DARK  0x2945  // Dark blue-gray
#define COL_IRIS_MID   0x4A69  // Medium blue-gray
#define COL_IRIS_LIGHT 0x6B4D  // Light blue-gray

// Eye parameters
float eyeX = 0;           // Current eye position (-1 to 1)
float eyeY = 0;
float targetX = 0;        // Target position
float targetY = 0;
float eyeSpeed = 0.08;    // Movement smoothing

// Pupil parameters
float pupilSize = 12;
float pupilTargetSize = 12;
float irisRadius = 28;

// Blink parameters
float blinkAmount = 0;    // 0 = open, 1 = closed
float blinkTarget = 0;
bool isBlinking = false;
unsigned long lastBlinkTime = 0;
unsigned long nextBlinkDelay = 2000;

// Look around timing
unsigned long lastLookTime = 0;
unsigned long nextLookDelay = 1500;

// Glitch parameters
unsigned long lastGlitchTime = 0;
#define GLITCH_INTERVAL 10000  // 10 seconds
bool isGlitching = false;
unsigned long glitchStartTime = 0;
#define GLITCH_DURATION 800    // Glitch lasts 800ms

// Text display state
enum DisplayState {
  STATE_EYE,
  STATE_DIGITAL,
  STATE_HUMANITY,
  STATE_FADE_OUT
};
DisplayState currentState = STATE_EYE;
unsigned long stateStartTime = 0;
#define TEXT_DISPLAY_TIME 3000  // 3 seconds
#define FADE_DURATION 500       // 500ms fade

// Kinetic text animation
int textAnimFrame = 0;
unsigned long lastTextAnimTime = 0;

// ASCII glitch characters
const char glitchChars[] = "01!@#$%^&*<>[]{}|/\\~`";
const int numGlitchChars = sizeof(glitchChars) - 1;

// Forward declarations
void onLeftClick();
void onRightClick();
void drawEye();
void drawGlitch();
void drawDigital();
void drawHumanity();
void updateEyeMovement();
void updateBlink();
void drawFadeOut();

void setup() {
  Serial.begin(115200);

  // Initialize random seed
  randomSeed(analogRead(4) * millis());

  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(COL_BLACK);

  // Create sprite for double buffering
  eyeSprite.createSprite(SCREEN_W, SCREEN_H);
  eyeSprite.setColorDepth(16);

  // Setup buttons
  btnLeft.attachClick(onLeftClick);
  btnRight.attachClick(onRightClick);

  // Initialize timing
  lastBlinkTime = millis();
  lastLookTime = millis();
  lastGlitchTime = millis();
  nextBlinkDelay = random(2000, 5000);
  nextLookDelay = random(1000, 3000);

  Serial.println("Generative Eye initialized");
}

void loop() {
  unsigned long currentTime = millis();

  // Always tick buttons
  btnLeft.tick();
  btnRight.tick();

  switch (currentState) {
    case STATE_EYE:
      // Check for glitch trigger
      if (currentTime - lastGlitchTime >= GLITCH_INTERVAL && !isGlitching) {
        isGlitching = true;
        glitchStartTime = currentTime;
      }

      // Update glitch state
      if (isGlitching) {
        if (currentTime - glitchStartTime >= GLITCH_DURATION) {
          isGlitching = false;
          lastGlitchTime = currentTime;
        } else {
          drawGlitch();
          delay(20);
          break;
        }
      }

      // Normal eye animation
      updateEyeMovement();
      updateBlink();
      drawEye();
      delay(16);  // ~60fps
      break;

    case STATE_DIGITAL:
      drawDigital();
      if (currentTime - stateStartTime >= TEXT_DISPLAY_TIME) {
        currentState = STATE_FADE_OUT;
        stateStartTime = currentTime;
      }
      delay(30);
      break;

    case STATE_HUMANITY:
      drawHumanity();
      if (currentTime - stateStartTime >= TEXT_DISPLAY_TIME) {
        currentState = STATE_FADE_OUT;
        stateStartTime = currentTime;
      }
      delay(30);
      break;

    case STATE_FADE_OUT:
      drawFadeOut();
      if (currentTime - stateStartTime >= FADE_DURATION) {
        currentState = STATE_EYE;
        lastGlitchTime = currentTime;  // Reset glitch timer
      }
      delay(20);
      break;
  }
}

void onLeftClick() {
  if (currentState == STATE_EYE) {
    currentState = STATE_DIGITAL;
    stateStartTime = millis();
    textAnimFrame = 0;
    lastTextAnimTime = millis();
    Serial.println("DIGITAL pressed");
  }
}

void onRightClick() {
  if (currentState == STATE_EYE) {
    currentState = STATE_HUMANITY;
    stateStartTime = millis();
    textAnimFrame = 0;
    lastTextAnimTime = millis();
    Serial.println("HUMANITY pressed");
  }
}

void updateEyeMovement() {
  unsigned long currentTime = millis();

  // Time for a new look direction?
  if (currentTime - lastLookTime >= nextLookDelay) {
    // Pick new target position
    targetX = (random(100) - 50) / 50.0 * 0.6;  // -0.6 to 0.6
    targetY = (random(100) - 50) / 50.0 * 0.5;  // -0.5 to 0.5

    // Occasionally look at center
    if (random(100) < 20) {
      targetX = 0;
      targetY = 0;
    }

    lastLookTime = currentTime;
    nextLookDelay = random(800, 2500);

    // Adjust pupil size randomly
    pupilTargetSize = random(10, 16);
  }

  // Smooth movement towards target
  eyeX += (targetX - eyeX) * eyeSpeed;
  eyeY += (targetY - eyeY) * eyeSpeed;

  // Smooth pupil size change
  pupilSize += (pupilTargetSize - pupilSize) * 0.1;
}

void updateBlink() {
  unsigned long currentTime = millis();

  // Time for a blink?
  if (!isBlinking && currentTime - lastBlinkTime >= nextBlinkDelay) {
    isBlinking = true;
    blinkTarget = 1.0;
    lastBlinkTime = currentTime;

    // Sometimes do a double blink
    if (random(100) < 15) {
      nextBlinkDelay = 150;  // Quick second blink
    } else {
      nextBlinkDelay = random(2500, 6000);
    }
  }

  // Animate blink
  if (isBlinking) {
    blinkAmount += (blinkTarget - blinkAmount) * 0.3;

    if (blinkTarget == 1.0 && blinkAmount > 0.9) {
      blinkTarget = 0;  // Start opening
    }

    if (blinkTarget == 0 && blinkAmount < 0.05) {
      blinkAmount = 0;
      isBlinking = false;
    }
  }
}

void drawEye() {
  // Clear sprite
  eyeSprite.fillSprite(COL_BLACK);

  // Calculate eye offset based on look direction
  int offsetX = (int)(eyeX * 12);
  int offsetY = (int)(eyeY * 10);

  // Draw outer eye socket (skin tone circle)
  eyeSprite.fillCircle(CENTER_X, CENTER_Y, 62, COL_SKIN);

  // Draw eye white (sclera) with slight shadow
  eyeSprite.fillCircle(CENTER_X, CENTER_Y, 52, COL_LIGHT_GRAY);
  eyeSprite.fillCircle(CENTER_X, CENTER_Y + 2, 50, COL_WHITE);

  // Draw subtle blood vessels
  for (int i = 0; i < 5; i++) {
    int angle = random(360);
    float rad = angle * PI / 180.0;
    int x1 = CENTER_X + cos(rad) * 40;
    int y1 = CENTER_Y + sin(rad) * 40;
    int x2 = CENTER_X + cos(rad) * 50;
    int y2 = CENTER_Y + sin(rad) * 50;
    eyeSprite.drawLine(x1, y1, x2, y2, 0xFBCE);  // Very light pink
  }

  // Draw iris
  int irisX = CENTER_X + offsetX;
  int irisY = CENTER_Y + offsetY;

  // Iris outer ring
  eyeSprite.fillCircle(irisX, irisY, irisRadius, COL_IRIS_DARK);

  // Iris pattern - radial lines
  for (int a = 0; a < 360; a += 15) {
    float rad = a * PI / 180.0;
    int x1 = irisX + cos(rad) * 8;
    int y1 = irisY + sin(rad) * 8;
    int x2 = irisX + cos(rad) * (irisRadius - 2);
    int y2 = irisY + sin(rad) * (irisRadius - 2);
    uint16_t lineColor = (a % 30 == 0) ? COL_IRIS_LIGHT : COL_IRIS_MID;
    eyeSprite.drawLine(x1, y1, x2, y2, lineColor);
  }

  // Inner iris highlight
  eyeSprite.fillCircle(irisX, irisY, irisRadius - 6, COL_IRIS_MID);

  // Draw pupil
  int pupilR = (int)pupilSize;
  eyeSprite.fillCircle(irisX, irisY, pupilR, COL_BLACK);

  // Pupil highlight (reflection)
  eyeSprite.fillCircle(irisX - 5, irisY - 5, 4, COL_WHITE);
  eyeSprite.fillCircle(irisX + 7, irisY + 3, 2, COL_LIGHT_GRAY);

  // Draw eyelids based on blink
  if (blinkAmount > 0.01) {
    int lidHeight = (int)(blinkAmount * 55);

    // Upper eyelid
    eyeSprite.fillRect(0, 0, SCREEN_W, CENTER_Y - 52 + lidHeight, COL_SKIN);
    eyeSprite.fillCircle(CENTER_X, CENTER_Y - 52 + lidHeight, 52, COL_SKIN);

    // Lower eyelid
    eyeSprite.fillRect(0, CENTER_Y + 52 - lidHeight, SCREEN_W, SCREEN_H, COL_SKIN);
    eyeSprite.fillCircle(CENTER_X, CENTER_Y + 52 - lidHeight, 52, COL_SKIN);

    // Eyelid crease
    if (blinkAmount < 0.3) {
      eyeSprite.drawLine(CENTER_X - 45, CENTER_Y - 45, CENTER_X + 45, CENTER_Y - 45, COL_MID_GRAY);
    }
  } else {
    // Draw eyelid edges when open
    // Upper lid shadow
    for (int i = 0; i < 3; i++) {
      int y = CENTER_Y - 50 + i;
      int halfWidth = sqrt(52*52 - (y - CENTER_Y)*(y - CENTER_Y));
      eyeSprite.drawFastHLine(CENTER_X - halfWidth, y, halfWidth * 2, COL_SKIN);
    }
  }

  // Circular mask - make edges black (display is circular)
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      float dx = x - CENTER_X;
      float dy = y - CENTER_Y;
      float dist = sqrt(dx*dx + dy*dy);
      if (dist > 62) {
        eyeSprite.drawPixel(x, y, COL_BLACK);
      }
    }
  }

  // Push to display
  eyeSprite.pushSprite(0, 0);
}

void drawGlitch() {
  unsigned long elapsed = millis() - glitchStartTime;
  float progress = (float)elapsed / GLITCH_DURATION;

  // Intensity varies during glitch
  float intensity = sin(progress * PI) * 0.8;  // Peak in middle

  // Random glitch type
  int glitchType = random(4);

  eyeSprite.fillSprite(COL_BLACK);

  switch (glitchType) {
    case 0: {
      // ASCII character scatter
      eyeSprite.setTextColor(COL_WHITE);
      eyeSprite.setTextSize(1);
      int numChars = (int)(intensity * 40) + 5;
      for (int i = 0; i < numChars; i++) {
        int x = random(SCREEN_W);
        int y = random(SCREEN_H);
        char c = glitchChars[random(numGlitchChars)];
        eyeSprite.drawChar(x, y, c, COL_WHITE, COL_BLACK, 1);
      }
      break;
    }

    case 1: {
      // Horizontal scan lines with ASCII
      for (int y = 0; y < SCREEN_H; y += 8) {
        if (random(100) < intensity * 100) {
          eyeSprite.drawFastHLine(0, y, SCREEN_W, COL_WHITE);
          // Add some ASCII on the line
          for (int x = 0; x < SCREEN_W; x += 12) {
            if (random(100) < 50) {
              char c = glitchChars[random(numGlitchChars)];
              eyeSprite.drawChar(x, y - 4, c, COL_BLACK, COL_WHITE, 1);
            }
          }
        }
      }
      break;
    }

    case 2: {
      // Binary rain
      eyeSprite.setTextColor(COL_WHITE);
      for (int x = 0; x < SCREEN_W; x += 10) {
        int startY = random(SCREEN_H);
        int len = random(3, 8);
        for (int i = 0; i < len; i++) {
          int y = (startY + i * 10) % SCREEN_H;
          char c = random(2) ? '0' : '1';
          uint16_t col = (i == len - 1) ? COL_WHITE : COL_MID_GRAY;
          eyeSprite.drawChar(x, y, c, col, COL_BLACK, 1);
        }
      }
      break;
    }

    case 3: {
      // Block distortion with symbols
      int numBlocks = (int)(intensity * 8) + 2;
      for (int i = 0; i < numBlocks; i++) {
        int bx = random(SCREEN_W - 30);
        int by = random(SCREEN_H - 15);
        int bw = random(15, 40);
        int bh = random(8, 20);
        uint16_t col = random(2) ? COL_WHITE : COL_MID_GRAY;
        eyeSprite.fillRect(bx, by, bw, bh, col);
        // Add symbol in block
        char c = glitchChars[random(numGlitchChars)];
        uint16_t textCol = (col == COL_WHITE) ? COL_BLACK : COL_WHITE;
        eyeSprite.drawChar(bx + bw/2 - 3, by + bh/2 - 4, c, textCol, col, 1);
      }
      break;
    }
  }

  // Always add some noise dots
  int numDots = (int)(intensity * 100);
  for (int i = 0; i < numDots; i++) {
    int x = random(SCREEN_W);
    int y = random(SCREEN_H);
    eyeSprite.drawPixel(x, y, random(2) ? COL_WHITE : COL_MID_GRAY);
  }

  // Circular mask
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      float dx = x - CENTER_X;
      float dy = y - CENTER_Y;
      if (sqrt(dx*dx + dy*dy) > 62) {
        eyeSprite.drawPixel(x, y, COL_BLACK);
      }
    }
  }

  eyeSprite.pushSprite(0, 0);
}

void drawKineticText(const char* text, bool whiteBg) {
  unsigned long currentTime = millis();

  // Update animation frame
  if (currentTime - lastTextAnimTime > 50) {
    textAnimFrame++;
    lastTextAnimTime = currentTime;
  }

  // Background
  uint16_t bgColor = whiteBg ? COL_WHITE : COL_BLACK;
  uint16_t textColor = whiteBg ? COL_BLACK : COL_WHITE;

  eyeSprite.fillSprite(bgColor);

  int len = strlen(text);

  // Calculate text positioning
  // Using font 2 (16 pixel height), each char ~12 pixels wide
  int charWidth = 10;
  int charHeight = 16;
  int totalWidth = len * charWidth;
  int startX = (SCREEN_W - totalWidth) / 2;
  int startY = (SCREEN_H - charHeight) / 2;

  // Draw each character with kinetic effect
  for (int i = 0; i < len; i++) {
    // Calculate character offset based on animation
    float phase = (textAnimFrame * 0.15 + i * 0.5);
    int offsetY = (int)(sin(phase) * 3);
    int offsetX = (int)(cos(phase * 0.7) * 2);

    // Occasionally glitch a character
    char c = text[i];
    if (random(100) < 5) {
      // Replace with glitch char briefly
      c = glitchChars[random(numGlitchChars)];
    }

    int x = startX + i * charWidth + offsetX;
    int y = startY + offsetY;

    // Draw character shadow for depth
    if (!whiteBg) {
      eyeSprite.drawChar(x + 1, y + 1, c, COL_DARK_GRAY, bgColor, 2);
    }

    // Draw main character
    eyeSprite.drawChar(x, y, c, textColor, bgColor, 2);
  }

  // Add subtle scan lines for texture
  for (int y = 0; y < SCREEN_H; y += 4) {
    uint16_t lineColor = whiteBg ? 0xE71C : 0x1082;  // Subtle variation
    eyeSprite.drawFastHLine(0, y, SCREEN_W, lineColor);
  }

  // Circular mask
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      float dx = x - CENTER_X;
      float dy = y - CENTER_Y;
      if (sqrt(dx*dx + dy*dy) > 62) {
        eyeSprite.drawPixel(x, y, COL_BLACK);
      }
    }
  }

  eyeSprite.pushSprite(0, 0);
}

void drawDigital() {
  drawKineticText("DIGITAL", true);  // White background
}

void drawHumanity() {
  drawKineticText("HUMANITY", false);  // Black background
}

void drawFadeOut() {
  unsigned long elapsed = millis() - stateStartTime;
  float progress = (float)elapsed / FADE_DURATION;

  if (progress > 1.0) progress = 1.0;

  // Calculate fade amount (0-31 for 5-bit per channel adjustment)
  int fadeLevel = (int)(progress * 20);

  eyeSprite.fillSprite(COL_BLACK);

  // Draw fading rectangles
  for (int i = fadeLevel; i < 20; i++) {
    int size = 64 - i * 3;
    if (size > 0) {
      uint16_t gray = ((20 - i) * 8) << 11 | ((20 - i) * 16) << 5 | ((20 - i) * 8);
      eyeSprite.fillRect(CENTER_X - size, CENTER_Y - size, size * 2, size * 2, gray);
    }
  }

  // Circular mask
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      float dx = x - CENTER_X;
      float dy = y - CENTER_Y;
      if (sqrt(dx*dx + dy*dy) > 62) {
        eyeSprite.drawPixel(x, y, COL_BLACK);
      }
    }
  }

  eyeSprite.pushSprite(0, 0);
}
