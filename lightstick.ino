/*
  Lightstick (Arduino Nano)
  - Rainbow color cycle by default
  - Button press toggles to solid white
  - Press again toggles back to rainbow

  RGB pins: D3, D5, D6 (PWM)
  Button: D2 to GND using INPUT_PULLUP

  Set COMMON_ANODE to true for common anode RGB modules.
*/

const int R_PIN = 3;
const int G_PIN = 5;
const int B_PIN = 6;
const int BTN   = 2;

const bool COMMON_ANODE = true;

// Debounce
unsigned long lastPressMs = 0;
const unsigned long DEBOUNCE_MS = 180;

// Mode
bool whiteMode = false;
int lastBtn = HIGH;

// Rainbow speed
const unsigned long RAINBOW_STEP_MS = 15;
unsigned long lastRainbowMs = 0;
int hue = 0; // 0..359

void setRGB(int r, int g, int b) {
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);

  if (COMMON_ANODE) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }

  analogWrite(R_PIN, r);
  analogWrite(G_PIN, g);
  analogWrite(B_PIN, b);
}

// Convert Hue (0..359) to RGB (0..255), full saturation & value
void hueToRGB(int h, int &r, int &g, int &b) {
  // HSV with S=1, V=1
  float hf = (h % 360) / 60.0;   // 0..6
  int i = (int)hf;               // 0..5
  float f = hf - i;

  float p = 0.0;
  float q = 1.0 - f;
  float t = f;

  float rf=0, gf=0, bf=0;

  switch (i) {
    case 0: rf = 1; gf = t; bf = p; break; // red -> yellow
    case 1: rf = q; gf = 1; bf = p; break; // yellow -> green
    case 2: rf = p; gf = 1; bf = t; break; // green -> cyan
    case 3: rf = p; gf = q; bf = 1; break; // cyan -> blue
    case 4: rf = t; gf = p; bf = 1; break; // blue -> magenta
    default: rf = 1; gf = p; bf = q; break; // magenta -> red
  }

  r = (int)(rf * 255);
  g = (int)(gf * 255);
  b = (int)(bf * 255);
}

void setup() {
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  pinMode(BTN, INPUT_PULLUP);

  // start rainbow
  setRGB(0, 0, 0);
}

void loop() {
  unsigned long now = millis();

  // ---- Button press toggles mode (edge detect) ----
  int btn = digitalRead(BTN);
  if (lastBtn == HIGH && btn == LOW) {
    if (now - lastPressMs > DEBOUNCE_MS) {
      whiteMode = !whiteMode;
      lastPressMs = now;
    }
  }
  lastBtn = btn;

  // ---- Output ----
  if (whiteMode) {
    // Solid white
    setRGB(255, 255, 255);
  } else {
    // Rainbow cycle
    if (now - lastRainbowMs >= RAINBOW_STEP_MS) {
      lastRainbowMs = now;

      int r, g, b;
      hueToRGB(hue, r, g, b);
      setRGB(r, g, b);

      hue = (hue + 1) % 360;
    }
  }
}
