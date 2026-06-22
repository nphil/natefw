
#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "CorePluginSystem.h"
#include "ThemeEngine.h"
#include "InputHandler.h"

// Define ATS Mini specific encoder pinout mappings
#define ENCODER_PIN_A  1
#define ENCODER_PIN_B  2
#define ENCODER_BUTTON 3

class LGFX_ATSMini : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    LGFX_ATSMini() {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;
            cfg.sclk_pin = 4;
            cfg.mosi_pin = 6;
            cfg.miso_pin = -1;
            cfg.dc_pin   = 7;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = 5;
            cfg.pin_rst          = 8;
            cfg.pin_busy         = -1;
            cfg.panel_width      = 170;
            cfg.panel_height     = 320;
            cfg.offset_x         = 35;
            cfg.offset_y         = 0;
            cfg.invert           = true;
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};

LGFX_ATSMini lcd;
PluginManager systemPlugins;
ThemeEngine sysTheme;
InputHandler input(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_BUTTON);

// Initial basic Core Module tracking Volume and UX state
class RadioCoreModule : public ATSModule {
private:
    int currentSelection = 0;
    bool editMode = false;
    int volume = 30;
    bool needsRefresh = true;

public:
    const char* getModuleName() override { return "Radio Core"; }
    
    void onInit() override {
        needsRefresh = true;
    }

    void onLoop() override {}

    void onEncoderEvent(EncoderEvent event) override {
        needsRefresh = true;
        switch(event) {
            case EncoderEvent::ROTATE_LEFT:
                if (editMode) {
                    if (currentSelection == 0) volume = max(0, volume - 1);
                } else {
                    currentSelection = max(0, currentSelection - 1);
                }
                break;
            case EncoderEvent::ROTATE_RIGHT:
                if (editMode) {
                    if (currentSelection == 0) volume = min(63, volume + 1);
                } else {
                    currentSelection = min(1, currentSelection + 1);
                }
                break;
            case EncoderEvent::CLICK:
                editMode = !editMode; // Toggle between traversing cards and editing a card value
                break;
            case EncoderEvent::LONG_PRESS:
                sysTheme.setTheme(!sysTheme.get().isDark); // Global Material You theme swap shortcut
                break;
        }
    }

    void onRender() override {
        if (!needsRefresh) return;
        needsRefresh = false;

        const auto& color = sysTheme.get();
        lcd.startWrite();
        lcd.clear(color.background);

        // Render Material You Top Status Bar Bar Header
        lcd.fillRect(0, 0, 170, 32, color.surfaceVariant);
        lcd.setTextColor(color.onSurface);
        lcd.drawString(getModuleName(), 8, 8);

        // Component UI Card Block
        // If hovered: use surfaceVariant. If clicked into edit mode: flash vibrant Primary color.
        uint16_t cardBg = (currentSelection == 0) ? (editMode ? color.primary : color.surfaceVariant) : color.surface;
        uint16_t textCol = (currentSelection == 0 && editMode) ? color.onPrimary : color.onSurface;
        
        lcd.fillRect(10, 50, 150, 60, cardBg);
        lcd.setTextColor(textCol);
        lcd.drawString("Volume Control", 20, 60);
        
        char volStr[16];
        sprintf(volStr, "%d", volume);
        lcd.drawString(volStr, 20, 80);
        
        lcd.endWrite();
    }
};

RadioCoreModule radioModule;

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.setRotation(1);
    sysTheme.setTheme(true); // Default to Dark Theme
    input.init();
    
    systemPlugins.registerModule(&radioModule);
}

void loop() {
    input.update(systemPlugins);
    
    if (systemPlugins.getActive()) {
        systemPlugins.getActive()->onLoop();
        systemPlugins.getActive()->onRender();
    }
    delay(10);
}
