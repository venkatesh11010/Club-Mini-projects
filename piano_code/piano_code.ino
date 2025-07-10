#include <Arduino.h>
#include "pitches.h"

#define BUZZER_PIN 33  // GPIO pin connected to passive buzzer

const uint8_t buttonPins[] = {23, 22, 21, 19, 18, 5, 4, 16};  // Update pins if needed
const int buttonTones[] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5
};
const int numTones = sizeof(buttonPins) / sizeof(buttonPins[0]);

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  for (uint8_t i = 0; i < numTones; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  for (uint8_t i = 0; i < numTones; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      playTone(buttonTones[i], 200);  // Play for 200ms when button is pressed
      break;  // Only one key at a time
    }
  }
}

// Manual tone generation using digitalWrite
void playTone(int frequency, int duration) {
  int period = 1000000L / frequency;
  int halfPeriod = period / 2;
  long cycles = (long)duration * 1000L / period;

  for (long i = 0; i < cycles; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(halfPeriod);
  }
}
