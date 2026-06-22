
#ifndef CORE_PLUGIN_SYSTEM_H
#define CORE_PLUGIN_SYSTEM_H

#include <Arduino.h>
#include <vector>

// The 4 inputs available from your rotary encoder
enum class EncoderEvent {
    ROTATE_LEFT,
    ROTATE_RIGHT,
    CLICK,
    LONG_PRESS
};

// Abstract interface that every future plugin/module will use
class ATSModule {
public:
    virtual const char* getModuleName() = 0;
    virtual void onInit() = 0;
    virtual void onLoop() = 0;
    virtual void onEncoderEvent(EncoderEvent event) = 0;
    virtual void onRender() = 0;
    virtual ~ATSModule() {}
};

// Manages which module is currently running and receiving inputs
class PluginManager {
private:
    std::vector<ATSModule*> modules;
    ATSModule* activeModule = nullptr;

public:
    void registerModule(ATSModule* mod) {
        modules.push_back(mod);
        mod->onInit();
        if (!activeModule) activeModule = mod; // Default to first registered
    }

    void setActiveModule(size_t index) {
        if (index < modules.size()) {
            activeModule = modules[index];
        }
    }

    ATSModule* getActive() { return activeModule; }
    const std::vector<ATSModule*>& getModules() { return modules; }
};

extern PluginManager systemPlugins;

#endif
