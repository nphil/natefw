
#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Arduino.h>
#include "CorePluginSystem.h"

class InputHandler {
private:
    int pinA, pinB, pinButton;
    int lastStateA;
    unsigned long buttonPressTime = 0;
    bool isHeld = false;
    const unsigned long LONG_PRESS_MS = 600; // Time in milliseconds for a long press

public:
    InputHandler(int a, int b, int btn) : pinA(a), pinB(b), pinButton(btn) {}

    void init() {
        pinMode(pinA, INPUT_PULLUP);
        pinMode(pinB, INPUT_PULLUP);
        pinMode(pinButton, INPUT_PULLUP);
        lastStateA = digitalRead(pinA);
    }

    void update(PluginManager& pm) {
        int currentStateA = digitalRead(pinA);
        
        // Check rotation
        if (currentStateA != lastStateA && currentStateA == LOW) {
            if (digitalRead(pinB) != currentStateA) {
                if(pm.getActive()) pm.getActive()->onEncoderEvent(EncoderEvent::ROTATE_RIGHT);
            } else {
                if(pm.getActive()) pm.getActive()->onEncoderEvent(EncoderEvent::ROTATE_LEFT);
            }
        }
        lastStateA = currentStateA;

        // Check button actions
        bool btnState = (digitalRead(pinButton) == LOW);
        static bool lastBtnState = false;

        if (btnState && !lastBtnState) {
            buttonPressTime = millis();
            isHeld = false;
        } else if (!btnState && lastBtnState) {
            if (!isHeld) {
                if(pm.getActive()) pm.getActive()->onEncoderEvent(EncoderEvent::CLICK);
            }
        }

        // Check for long press hold
        if (btnState && !isHeld && (millis() - buttonPressTime > LONG_PRESS_MS)) {
            isHeld = true;
            if(pm.getActive()) pm.getActive()->onEncoderEvent(EncoderEvent::LONG_PRESS);
        }
        
        lastBtnState = btnState;
    }
};

#endif
