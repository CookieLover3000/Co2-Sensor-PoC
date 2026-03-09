#include "DisplaySettings.hpp"

using namespace Settings;

std::atomic<uint8_t> DisplaySettings::maxBrightness(100);
std::atomic<uint8_t> DisplaySettings::minBrightness(5);
std::atomic<uint32_t> DisplaySettings::displayOffDelay(60000);

std::atomic<std::underlying_type_t<DisplaySettings::Monitor>> DisplaySettings::mainMonitor(CO2);
std::atomic<std::underlying_type_t<DisplaySettings::Monitor>> DisplaySettings::upperMonitor(TEMPERATURE);
std::atomic<std::underlying_type_t<DisplaySettings::Monitor>> DisplaySettings::lowerMonitor(HUMIDITY);
