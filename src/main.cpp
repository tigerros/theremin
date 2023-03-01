#include <Arduino.h>

const uint8_t PINA_PITCH_PHOTOTRANSISTOR = A0;
const uint8_t PIND_PIEZO = 10;
const uint8_t PIND_CALIBRATE_BTN = 12;
const uint8_t PIND_SWITCH_BTN = 8;
const uint8_t PIND_CALIBRATE_LED = 4;
const uint8_t PIND_ON_LED = 3;

const unsigned int CALIBRATION_TONE_PITCH = 100;
const unsigned long CALIBRATION_DURATION = 2500;
const unsigned long PLAY_DURATION = 20;
const unsigned int PITCH_MIN = 0;
const unsigned int PITCH_MAX = 4000;

long pitchLightMin = 1023;
long pitchLightMax = 0;
long pitchLight = 0;

bool wasCalibrateBtnPressed = false;
bool wasSwitchBtnPressed = false;
bool isCalibrating = false;
bool isOn = false;

// Calibrates the phototransistors
void calibrate();
// Plays a tone which has a frequency which corresponds to the amount of light falling on the phototransistors
void play();

void setup() {
    pinMode(PIND_CALIBRATE_LED, OUTPUT);
    pinMode(PIND_ON_LED, OUTPUT);
}

void loop() {
    bool isCalibrateBtnPressed = digitalRead(PIND_CALIBRATE_BTN);
    bool isSwitchBtnPressed = digitalRead(PIND_SWITCH_BTN);

    if (isCalibrateBtnPressed && isSwitchBtnPressed) {
        return;
    }

    // The calibrate button is not being pressed, but was. This means it was released, thus activating calibration
    if (!isCalibrateBtnPressed && wasCalibrateBtnPressed) {
        wasCalibrateBtnPressed = false;
        wasSwitchBtnPressed = false;
        isCalibrating = true;
        analogWrite(PIND_ON_LED, LOW);
        digitalWrite(PIND_CALIBRATE_LED, HIGH);

        calibrate();

        isCalibrating = false;
        digitalWrite(PIND_CALIBRATE_LED, LOW);

        return;
    }

    if (!isCalibrating) {
        if (!isSwitchBtnPressed && wasSwitchBtnPressed) {
            if (isOn) {
                isOn = false;
                noTone(PIND_PIEZO);
                digitalWrite(PIND_ON_LED, LOW);
            } else {
                isOn = true;
                digitalWrite(PIND_ON_LED, HIGH);
            }
        }

        if (isOn) {
            play();
        }
    }

    wasCalibrateBtnPressed = isCalibrateBtnPressed;
    wasSwitchBtnPressed = isSwitchBtnPressed;
}

void calibrate() {
    tone(PIND_PIEZO, CALIBRATION_TONE_PITCH, CALIBRATION_DURATION);
    unsigned long start = millis();

    while ((millis() - start) < CALIBRATION_DURATION) {
        pitchLight = analogRead(PINA_PITCH_PHOTOTRANSISTOR);

        if (pitchLight < pitchLightMin) {
            pitchLightMin = pitchLight;
        }

        if (pitchLight > pitchLightMax) {
            pitchLightMax = pitchLight;
        }
    }
}

void play() {
    pitchLight = analogRead(PINA_PITCH_PHOTOTRANSISTOR);
    long pitch = map(pitchLight, pitchLightMin, pitchLightMax, PITCH_MIN, PITCH_MAX);
    tone(PIND_PIEZO, pitch, PLAY_DURATION);
    delay(PLAY_DURATION / 2);
}