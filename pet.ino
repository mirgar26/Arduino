const int RED_PIN   = 3;
const int GREEN_PIN = 5;
const int BLUE_PIN  = 6;
const int BUTTON_PIN = 2;

const bool COMMON_ANODE = true;

int mood = 60;
const int MOOD_MAX = 100;
const int MOOD_MIN = 0;

const int PET_INCREASE = 20;  // faster happy

unsigned long lastPetTime = 0;
unsigned long lastDecayTime = 0;

// Faster sadness
const unsigned long DECAY_DELAY = 1500; // start getting sad after 1.5s no touch
const unsigned long DECAY_RATE  = 300;  // lose mood every 0.3s
const int DECAY_AMOUNT = 2;             // lose 2 points each tick

int lastBtn = HIGH;

void setRGB(int r, int g, int b) {
  r = constrain(r,0,255);
  g = constrain(g,0,255);
  b = constrain(b,0,255);

  if (COMMON_ANODE) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }

  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void updateMoodLED() {
  if (mood >= 70) {
    setRGB(0, 255, 0);         // happy green
  } else if (mood >= 30) {
    setRGB(180, 0, 180);       // neutral purple
  } else {
    setRGB(0, 0, 255);         // sad blue
  }
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lastPetTime = millis();
  lastDecayTime = millis();
  updateMoodLED();
}

void loop() {
  unsigned long now = millis();

  // Button press event (only once per press)
  int btn = digitalRead(BUTTON_PIN);
  if (lastBtn == HIGH && btn == LOW) {
    mood = constrain(mood + PET_INCREASE, MOOD_MIN, MOOD_MAX);
    lastPetTime = now;
    updateMoodLED();
    delay(25); // tiny debounce
  }
  lastBtn = btn;

  // Mood decay after no interaction
  if (now - lastPetTime > DECAY_DELAY) {
    if (now - lastDecayTime > DECAY_RATE) {
      mood = constrain(mood - DECAY_AMOUNT, MOOD_MIN, MOOD_MAX);
      lastDecayTime = now;
      updateMoodLED();
    }
  }

  delay(5);
}
