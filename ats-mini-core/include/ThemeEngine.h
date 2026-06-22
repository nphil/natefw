
#ifndef THEME_ENGINE_H
#define THEME_ENGINE_H

#include <stdint.h>

struct MaterialTheme {
    uint16_t background;
    uint16_t surface;
    uint16_t surfaceVariant;
    uint16_t primary;
    uint16_t secondary;
    uint16_t onBackground;
    uint16_t onSurface;
    uint16_t onPrimary;
    bool isDark;
};

// RGB565 Color Palettes matching Material 3 Specs
const MaterialTheme MaterialDark = {
    .background = 0x10A2,      // Dark canvas
    .surface = 0x18E4,         // Dark card base
    .surfaceVariant = 0x2946,  // Highlighted item container
    .primary = 0xAECF,         // Soft pastel focus color
    .secondary = 0xCE79,       
    .onBackground = 0xE73C,    // Off-white text
    .onSurface = 0xE73C,       
    .onPrimary = 0x0145,       // Dark text to contrast primary color
    .isDark = true
};

const MaterialTheme MaterialLight = {
    .background = 0xFEF9,     // Light canvas
    .surface = 0xF77D,        // Light card base
    .surfaceVariant = 0xE73C, // Highlighted item container
    .primary = 0x2236,        // Deep contrast color
    .secondary = 0x3357,      
    .onBackground = 0x10A2,   // Near black text
    .onSurface = 0x10A2,       
    .onPrimary = 0xFFFF,      // Pure white text
    .isDark = false
};

class ThemeEngine {
private:
    MaterialTheme currentTheme = MaterialDark;

public:
    void setTheme(bool dark) {
        currentTheme = dark ? MaterialDark : MaterialLight;
    }
    const MaterialTheme& get() { return currentTheme; }
};

extern ThemeEngine sysTheme;

#endif
