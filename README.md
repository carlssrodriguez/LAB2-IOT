🚦 ESP32 Traffic Light System
This project implements a traffic light system using an ESP32 (TTGO) board. It controls Red, Yellow, and Green LEDs, a buzzer for visually impaired users, and a pedestrian button.
Startup Sequence: Red → Red-Yellow → Green
Button Press: Green stays on until pressed, then waits 5 seconds before transitioning.
Buzzer: Green: Beeps 500ms ON / 1500ms OFF; Red: Beeps 250ms ON / 250ms OFF
Built using PlatformIO / Arduino IDE with non-blocking execution (millis()). 🚀
Demo in Youtube: https://youtube.com/shorts/rRyuGwJ9AQw?si=A4g8Qsq93j1uHhEp
