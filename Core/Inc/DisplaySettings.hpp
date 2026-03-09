#ifndef DISPLAY_SETTINGS_H
#define DISPLAY_SETTINGS_H

#include <atomic>
#include <cstdint>
namespace Settings
{
class DisplaySettings
{
  public:
    enum WidgetType
    {
        MAIN,
        UPPER,
        LOWER
    };
    typedef enum
    {
        CO2,
        TEMPERATURE,
        HUMIDITY
    } Monitor;

    uint8_t getMaxBrightness() const { return maxBrightness.load(); }
    uint8_t getMinBrightness() const { return minBrightness.load(); }
    uint32_t getDisplayOffDelay() const { return displayOffDelay.load(); }
    void setMaxBrightness(uint8_t value) { maxBrightness.store(value); }
    void setMinBrightness(uint8_t value) { minBrightness.store(value); }
    void setDisplayOffDelay(uint32_t value) { displayOffDelay.store(value); }

    void setMainMonitor(Monitor monitor) { mainMonitor.store(static_cast<std::underlying_type_t<Monitor>>(monitor)); }
    void setUpperMonitor(Monitor monitor) { upperMonitor.store(static_cast<std::underlying_type_t<Monitor>>(monitor)); }
    void setLowerMonitor(Monitor monitor) { lowerMonitor.store(static_cast<std::underlying_type_t<Monitor>>(monitor)); }
    void setMonitor(WidgetType type, Monitor monitor)
    {
        std::underlying_type_t<Monitor> value = static_cast<std::underlying_type_t<Monitor>>(monitor);
        switch (type)
        {
        case MAIN:
            mainMonitor.store(value);
            break;
        case UPPER:
            upperMonitor.store(value);
            break;
        case LOWER:
            lowerMonitor.store(value);
            break;
        default:
            break;
        }
    }

    Monitor getMainMonitor() const { return static_cast<Monitor>(mainMonitor.load()); }
    Monitor getUpperMonitor() const { return static_cast<Monitor>(upperMonitor.load()); }
    Monitor getLowerMonitor() const { return static_cast<Monitor>(lowerMonitor.load()); }
    Monitor getMonitor(WidgetType type) const
    {
        switch (type)
        {
        case MAIN:
            return static_cast<Monitor>(mainMonitor.load());
        case UPPER:
            return static_cast<Monitor>(upperMonitor.load());
        case LOWER:
            return static_cast<Monitor>(lowerMonitor.load());
        default:
            return static_cast<Monitor>(mainMonitor.load());
        }
    }

  private:
    static std::atomic<uint8_t> maxBrightness;    // percentage
    static std::atomic<uint8_t> minBrightness;    // percentage
    static std::atomic<uint32_t> displayOffDelay; // ms

    static std::atomic<std::underlying_type_t<Monitor>> mainMonitor;
    static std::atomic<std::underlying_type_t<Monitor>> upperMonitor;
    static std::atomic<std::underlying_type_t<Monitor>> lowerMonitor;
};
} // namespace Settings

#endif /* DISPLAY_SETTINGS_H */