/*
 * Generative Eye - Interactive Art for T-QT
 *
 * Abstract organic iris with radial fibers and pulsing pupil
 * Inspired by macro iris photography / sea anemone aesthetics
 *
 * Hardware: LilyGo T-QT Pro (ESP32-S3, GC9A01 128x128)
 */

#include <TFT_eSPI.h>
#include <SPI.h>
#include "OneButton.h"

// Display
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

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

// Colors
#define COL_BLACK       0x0000
#define COL_WHITE       0xFFFF
#define COL_CYAN_DARK   0x3B6C  // Dark cyan for fiber base
#define COL_CYAN_MID    0x5D12  // Medium cyan
#define COL_CYAN_LIGHT  0x7F18  // Light cyan/pale blue
#define COL_CYAN_PALE   0xAFBF  // Very pale cyan (almost white)
#define COL_ORANGE      0xFB80  // Orange for dots
#define COL_ORANGE_DARK 0xC240  // Darker orange ring
#define COL_RED_DARK    0x9000  // Dark red inner ring

// Fiber parameters
#define NUM_RINGS 12           // Number of concentric rings of fibers
#define BASE_FIBERS 16         // Fibers in innermost ring
#define MAX_FIBERS 400         // Maximum total fibers

// Fiber structure
struct Fiber {
  float angle;        // Base angle in radians
  float length;       // Length from center
  float phase;        // Animation phase offset
  int ring;           // Which ring this fiber belongs to
};

Fiber fibers[MAX_FIBERS];
int numFibers = 0;

// Pupil parameters
float pupilRadius = 18;
float pupilTargetRadius = 18;
float pupilPhase = 0;

// Animation timing
unsigned long lastFrameTime = 0;
float animTime = 0;

// Glitch parameters
unsigned long lastGlitchTime = 0;
#define GLITCH_INTERVAL 10000
bool isGlitching = false;
unsigned long glitchStartTime = 0;
#define GLITCH_DURATION 800

// Text display state
enum DisplayState {
  STATE_EYE,
  STATE_DIGITAL,
  STATE_HUMANITY,
  STATE_FADE_OUT
};
DisplayState currentState = STATE_EYE;
unsigned long stateStartTime = 0;
#define TEXT_DISPLAY_TIME 3000
#define FADE_DURATION 500

// Kinetic text
int textAnimFrame = 0;
unsigned long lastTextAnimTime = 0;

// Glitch characters
const char glitchChars[] = "01!@#$%^&*<>[]{}|/\\~`";
const int numGlitchChars = sizeof(glitchChars) - 1;

// Forward declarations
void onLeftClick();
void onRightClick();
void initFibers();
void drawEye();
void drawGlitch();
void drawDigital();
void drawHumanity();
void drawFadeOut();
void drawKineticText(const char* text, bool whiteBg);

void setup() {
  Serial.begin(115200);

  randomSeed(analogRead(4) * millis());

  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(COL_BLACK);

  // Create sprite for double buffering
  spr.createSprite(SCREEN_W, SCREEN_H);
  spr.setColorDepth(16);

  // Setup buttons
  btnLeft.attachClick(onLeftClick);
  btnRight.attachClick(onRightClick);

  // Initialize fibers
  initFibers();

  // Initialize timing
  lastFrameTime = millis();
  lastGlitchTime = millis();

  Serial.println("Generative Eye initialized");
}

void initFibers() {
  numFibers = 0;

  // Create concentric rings of fibers
  for (int ring = 0; ring < NUM_RINGS; ring++) {
    // More fibers in outer rings
    int fibersInRing = BASE_FIBERS + ring * 8;
    float ringRadius = 22 + ring * 4;  // Starting radius + spacing

    for (int i = 0; i < fibersInRing && numFibers < MAX_FIBERS; i++) {
      float baseAngle = (2.0 * PI * i) / fibersInRing;
      // Add slight randomness to angle
      baseAngle += ((random(100) - 50) / 50.0) * (PI / fibersInRing) * 0.3;

      fibers[numFibers].angle = baseAngle;
      fibers[numFibers].length = ringRadius + random(6) - 3;
      fibers[numFibers].phase = random(1000) / 1000.0 * 2 * PI;
      fibers[numFibers].ring = ring;
      numFibers++;
    }
  }

  Serial.printf("Created %d fibers\n", numFibers);
}

void loop() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastFrameTime) / 1000.0;
  lastFrameTime = currentTime;

  // Update animation time
  animTime += deltaTime;

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
          delay(25);
          break;
        }
      }

      // Update pupil pulsing
      pupilPhase += deltaTime * 0.8;
      pupilTargetRadius = 16 + sin(pupilPhase) * 4;
      pupilRadius += (pupilTargetRadius - pupilRadius) * 0.1;

      drawEye();
      delay(20);  // ~50fps
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
        lastGlitchTime = currentTime;
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
  }
}

void onRightClick() {
  if (currentState == STATE_EYE) {
    currentState = STATE_HUMANITY;
    stateStartTime = millis();
    textAnimFrame = 0;
    lastTextAnimTime = millis();
  }
}

// Draw a fiber with gradient and tip dot
void drawFiber(float angle, float innerR, float outerR, float wobble) {
  // Apply subtle wobble animation
  float adjustedAngle = angle + sin(animTime * 2 + wobble) * 0.03;

  float cosA = cos(adjustedAngle);
  float sinA = sin(adjustedAngle);

  // Inner point (near pupil)
  int x1 = CENTER_X + (int)(cosA * innerR);
  int y1 = CENTER_Y + (int)(sinA * innerR);

  // Outer point (tip)
  int x2 = CENTER_X + (int)(cosA * outerR);
  int y2 = CENTER_Y + (int)(sinA * outerR);

  // Draw fiber line with gradient effect
  // We'll draw multiple segments for gradient
  int segments = 4;
  for (int s = 0; s < segments; s++) {
    float t1 = (float)s / segments;
    float t2 = (float)(s + 1) / segments;

    int sx1 = x1 + (x2 - x1) * t1;
    int sy1 = y1 + (y2 - y1) * t1;
    int sx2 = x1 + (x2 - x1) * t2;
    int sy2 = y1 + (y2 - y1) * t2;

    // Color gradient: darker near center, lighter at tips
    uint16_t col;
    if (t1 < 0.25) col = COL_CYAN_DARK;
    else if (t1 < 0.5) col = COL_CYAN_MID;
    else if (t1 < 0.75) col = COL_CYAN_LIGHT;
    else col = COL_CYAN_PALE;

    spr.drawLine(sx1, sy1, sx2, sy2, col);
  }

  // Draw orange dot at tip
  // Outer dark ring
  spr.fillCircle(x2, y2, 3, COL_ORANGE_DARK);
  // Orange middle
  spr.fillCircle(x2, y2, 2, COL_ORANGE);
  // Highlight
  spr.drawPixel(x2 - 1, y2 - 1, COL_CYAN_PALE);
}

void drawEye() {
  // Clear to black
  spr.fillSprite(COL_BLACK);

  // Draw all fibers
  for (int i = 0; i < numFibers; i++) {
    float innerR = pupilRadius + 2 + fibers[i].ring * 0.5;
    float outerR = fibers[i].length;

    // Skip if fiber would be inside pupil
    if (innerR >= outerR - 4) continue;

    drawFiber(fibers[i].angle, innerR, outerR, fibers[i].phase);
  }

  // Draw pupil (jagged edge)
  // First draw a slightly larger circle for the jagged edge effect
  int jaggedPoints = 32;
  int prevX = 0, prevY = 0;
  int firstX = 0, firstY = 0;

  for (int i = 0; i <= jaggedPoints; i++) {
    float angle = (2.0 * PI * i) / jaggedPoints;
    // Add noise to radius for jagged effect
    float noise = sin(angle * 8 + animTime * 3) * 2 + sin(angle * 13 + animTime * 2) * 1.5;
    float r = pupilRadius + noise;

    int x = CENTER_X + (int)(cos(angle) * r);
    int y = CENTER_Y + (int)(sin(angle) * r);

    if (i == 0) {
      firstX = x;
      firstY = y;
    } else {
      // Draw triangles to fill the jagged pupil
      spr.fillTriangle(CENTER_X, CENTER_Y, prevX, prevY, x, y, COL_BLACK);
    }

    prevX = x;
    prevY = y;
  }

  // Draw solid inner pupil
  spr.fillCircle(CENTER_X, CENTER_Y, (int)pupilRadius - 3, COL_BLACK);

  // Circular mask for display edge
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      float dx = x - CENTER_X;
      float dy = y - CENTER_Y;
      if (dx*dx + dy*dy > 62*62) {
        spr.drawPixel(x, y, COL_BLACK);
      }
    }
  }

  spr.pushSprite(0, 0);
}

void drawGlitch() {
  unsigned long elapsed = millis() - glitchStartTime;
  float progress = (float)elapsed / GLITCH_DURATION;
  float intensity = sin(progress * PI) * 0.8;

  spr.fillSprite(COL_BLACK);

  int glitchType = random(4);

  switch (glitchType) {
    case 0: {
      // Scattered ASCII
      spr.setTextColor(COL_WHITE);
      int numChars = (int)(intensity * 40) + 5;
      for (int i = 0; i < numChars; i++) {
        int x = random(SCREEN_W);
        int y = random(SCREEN_H);
        char c = glitchChars[random(numGlitchChars)];
        spr.drawChar(x, y, c, COL_WHITE, COL_BLACK, 1);
      }
      break;
    }

    case 1: {
      // Horizontal lines with ASCII
      for (int y = 0; y < SCREEN_H; y += 8) {
        if (random(100) < intensity * 100) {
          spr.drawFastHLine(0, y, SCREEN_W, COL_WHITE);
          for (int x = 0; x < SCREEN_W; x += 12) {
            if (random(100) < 50) {
              char c = glitchChars[random(numGlitchChars)];
              spr.drawChar(x, y - 4, c, COL_BLACK, COL_WHITE, 1);
            }
          }
        }
      }
      break;
    }

    case 2: {
      // Binary rain
      for (int x = 0; x < SCREEN_W; x += 10) {
        int startY = random(SCREEN_H);
        int len = random(3, 8);
        for (int i = 0; i < len; i++) {
          int y = (startY + i * 10) % SCREEN_H;
          char c = random(2) ? '0' : '1';
          uint16_t col = (i == len - 1) ? COL_WHITE : COL_CYAN_MID;
          spr.drawChar(x, y, c, col, COL_BLACK, 1);
        }
      }
      break;
    }

    case 3: {
      // Block distortion
      int numBlocks = (int)(intensity * 8) + 2;
      for (int i = 0; i < numBlocks; i++) {
        int bx = random(SCREEN_W - 30);
        int by = random(SCREEN_H - 15);
        int bw = random(15, 40);
        int bh = random(8, 20);
        uint16_t col = random(2) ? COL_WHITE : COL_CYAN_MID;
        spr.fillRect(bx, by, bw, bh, col);
        char c = glitchChars[random(numGlitchChars)];
        uint16_t textCol = (col == COL_WHITE) ? COL_BLACK : COL_WHITE;
        spr.drawChar(bx + bw/2 - 3, by + bh/2 - 4, c, textCol, col, 1);
      }
      break;
    }
  }

  // Noise dots
  int numDots = (int)(intensity * 100);
  for (int i = 0; i < numDots; i++) {
    spr.drawPixel(random(SCREEN_W), random(SCREEN_H), random(2) ? COL_WHITE : COL_CYAN_MID);
  }

  // Circular mask
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      float dx = x - CENTER_X;
      float dy = y - CENTER_Y;
      if (dx*dx + dy*dy > 62*62) {
        spr.drawPixel(x, y, COL_BLACK);
      }
    }
  }

  spr.pushSprite(0, 0);
}

void drawKineticText(const char* text, bool whiteBg) {
  unsigned long currentTime = millis();

  if (currentTime - lastTextAnimTime > 50) {
    textAnimFrame++;
    lastTextAnimTime = currentTime;
  }

  uint16_t bgColor = whiteBg ? COL_WHITE : COL_BLACK;
  uint16_t textColor = whiteBg ? COL_BLACK : COL_WHITE;

  spr.fillSprite(bgColor);

  int len = strlen(text);
  int charWidth = 10;
  int charHeight = 16;
  int totalWidth = len * charWidth;
  int startX = (SCREEN_W - totalWidth) / 2;
  int startY = (SCREEN_H - charHeight) / 2;

  for (int i = 0; i < len; i++) {
    float phase = (textAnimFrame * 0.15 + i * 0.5);
    int offsetY = (int)(sin(phase) * 3);
    int offsetX = (int)(cos(phase * 0.7) * 2);

    char c = text[i];
    if (random(100) < 5) {
      c = glitchChars[random(numGlitchChars)];
    }

    int x = startX + i * charWidth + offsetX;
    int y = startY + offsetY;

    if (!whiteBg) {
      spr.drawChar(x + 1, y + 1, c, COL_CYAN_DARK, bgColor, 2);
    }

    spr.drawChar(x, y, c, textColor, bgColor, 2);
  }

  // Scan lines
  for (int y = 0; y < SCREEN_H; y += 4) {
    uint16_t lineColor = whiteBg ? 0xE71C : 0x1082;
    spr.drawFastHLine(0, y, SCREEN_W, lineColor);
  }

  // Circular mask
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      float dx = x - CENTER_X;
      float dy = y - CENTER_Y;
      if (dx*dx + dy*dy > 62*62) {
        spr.drawPixel(x, y, COL_BLACK);
      }
    }
  }

  spr.pushSprite(0, 0);
}

void drawDigital() {
  drawKineticText("DIGITAL", true);
}

void drawHumanity() {
  drawKineticText("HUMANITY", false);
}

void drawFadeOut() {
  unsigned long elapsed = millis() - stateStartTime;
  float progress = (float)elapsed / FADE_DURATION;
  if (progress > 1.0) progress = 1.0;

  spr.fillSprite(COL_BLACK);

  // Fade effect with shrinking circles
  for (int i = 0; i < 20; i++) {
    int size = (int)((1.0 - progress) * (64 - i * 3));
    if (size > 0) {
      uint8_t gray = (uint8_t)((20 - i) * 12 * (1.0 - progress));
      uint16_t col = (gray >> 3) << 11 | (gray >> 2) << 5 | (gray >> 3);
      spr.fillCircle(CENTER_X, CENTER_Y, size, col);
    }
  }

  // Circular mask
  for (int y = 0; y < SCREEN_H; y++) {
    for (int x = 0; x < SCREEN_W; x++) {
      float dx = x - CENTER_X;
      float dy = y - CENTER_Y;
      if (dx*dx + dy*dy > 62*62) {
        spr.drawPixel(x, y, COL_BLACK);
      }
    }
  }

  spr.pushSprite(0, 0);
}
