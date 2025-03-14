#include <Arduino.h>

// Define LED Pins
#define RED_LED    25
#define YELLOW_LED 26
#define GREEN_LED  27

// Define Buzzer Pin
#define BUZZER     33

// Define Button Pin
#define BUTTON     17

// Timing Constants (milliseconds)
#define RED_TIME           10000  // 10s RED
#define YELLOW_TIME        2000   // 2s YELLOW
#define RED_YELLOW_TIME    2000   // 2s RED-YELLOW
#define MIN_GREEN_TIME     5000   // 5s minimum GREEN

#define BUZZER_GREEN_ON    500    // 500ms ON in GREEN
#define BUZZER_GREEN_OFF   1500   // 1500ms OFF in GREEN
#define BUZZER_RED_ON      250    // 250ms ON in RED
#define BUZZER_RED_OFF     250    // 250ms OFF in RED

// Traffic Light States
enum TrafficState { RED, RED_YELLOW, GREEN, YELLOW };
TrafficState state = RED;

// Function Prototypes
void IRAM_ATTR buttonISR();
void setTrafficState(TrafficState newState);
void handleTrafficState();
void handleBuzzer();

// Timing Variables
unsigned long lastStateChange = 0;
unsigned long greenStartTime = 0;
unsigned long yellowStartTime = 0;
bool buttonPressed = false;
bool requestChangeToYellow = false;  // Ensures green stays for 5s after button press

// Buzzer Timing
unsigned long lastBuzzerToggle = 0;
bool buzzerState = false;

void setup() {
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);  // Internal pull-up

    attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, FALLING);
    
    setTrafficState(RED);  // Start with RED (teacher's requirement)
}

void loop() {
    handleTrafficState();
    handleBuzzer();
}

// Interrupt Service Routine for Button Press
void IRAM_ATTR buttonISR() {
    buttonPressed = true;
}

// Change Traffic Light State
void setTrafficState(TrafficState newState) {
    state = newState;
    lastStateChange = millis();
    
    digitalWrite(RED_LED,    (state == RED || state == RED_YELLOW));
    digitalWrite(YELLOW_LED, (state == YELLOW || state == RED_YELLOW));
    digitalWrite(GREEN_LED,  (state == GREEN));

    if (state == GREEN) {
        greenStartTime = millis();  // Track when green starts
        buttonPressed = false;
        requestChangeToYellow = false;  // Reset request
    }

    if (state == YELLOW) {
        yellowStartTime = millis();  // Track yellow start time for delay
    }
}

// Handle Traffic Light Transitions
void handleTrafficState() {
    unsigned long elapsed = millis() - lastStateChange;

    switch (state) {
        case RED:
            if (elapsed >= RED_TIME) {
                setTrafficState(RED_YELLOW);
            }
            break;

        case RED_YELLOW:
            if (elapsed >= RED_YELLOW_TIME) {
                setTrafficState(GREEN);
            }
            break;

        case GREEN:
            // If button is pressed, start a countdown
            if (buttonPressed && !requestChangeToYellow) {
                requestChangeToYellow = true;  // Mark request
                yellowStartTime = millis();  // Start 5s countdown
            }

            // Ensure exactly 5s wait after button press before switching to yellow
            if (requestChangeToYellow && millis() - yellowStartTime >= MIN_GREEN_TIME) {
                setTrafficState(YELLOW);
            }
            break;

        case YELLOW:
            if (elapsed >= YELLOW_TIME) {
                setTrafficState(RED);
            }
            break;
    }
}

// Handle Buzzer Based on Traffic Light State
void handleBuzzer() {
    unsigned long buzzerElapsed = millis() - lastBuzzerToggle;
    int buzzerOnTime, buzzerOffTime;

    if (state == GREEN) {
        buzzerOnTime = BUZZER_GREEN_ON;
        buzzerOffTime = BUZZER_GREEN_OFF;
    } else if (state == RED) {
        buzzerOnTime = BUZZER_RED_ON;
        buzzerOffTime = BUZZER_RED_OFF;
    } else {
        digitalWrite(BUZZER, LOW);
        return;
    }

    if ((buzzerState && buzzerElapsed >= buzzerOnTime) || (!buzzerState && buzzerElapsed >= buzzerOffTime)) {
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, buzzerState);
        lastBuzzerToggle = millis();
    }
}
