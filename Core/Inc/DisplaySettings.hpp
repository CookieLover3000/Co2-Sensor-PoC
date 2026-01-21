#ifndef DISPLAY_SETTINGS_H
#define DISPLAY_SETTINGS_H

#include <atomic>
#include <cstdint>
namespace Settings
{
class DisplaySettings
{
  public:
    uint8_t getMaxBrightness() const { return maxBrightness.load(); }
    uint8_t getMinBrightness() const { return minBrightness.load(); }
    uint32_t getDisplayOffDelay() const { return displayOffDelay.load(); }
    void setMaxBrightness(uint8_t value) { maxBrightness.store(value); }
    void setMinBrightness(uint8_t value) { minBrightness.store(value); }
    void setDisplayOffDelay(uint32_t value) { displayOffDelay.store(value); }

  private:
    static std::atomic<uint8_t> maxBrightness;    // percentage
    static std::atomic<uint8_t> minBrightness;    // percentage
    static std::atomic<uint32_t> displayOffDelay; // ms
};
} // namespace Settings

#endif /* DISPLAY_SETTINGS_H */