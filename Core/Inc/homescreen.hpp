#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include "DisplayScreenBase.hpp"
#include "SensorDriverBase.hpp"
#include "SensorHandler.hpp"
#include <DisplaySettings.hpp>
#include <lv/lv.hpp>
#include <optional>

namespace UI
{

class Homescreen : public DisplayScreenBase
{
  public:
    Homescreen(App::SensorHandler &s) : sensor(s) {}
    ~Homescreen();

    virtual void init(void) override;
    virtual void load(void) override;
    virtual void update(void) override;
    virtual void destroy(void) override;
    virtual bool shouldSwitch(void) override;
    virtual void handleLongPress(void) override;

  private:
    App::SensorHandler &sensor;
    Drivers::SCD40Data sensorData = {0, 0, 0};
    Settings::DisplaySettings settings;

    typedef struct
    {
        const lv::Color co2_dangerous;
        const lv::Color co2_warning;
        const lv::Color co2_safe;
        const lv::Color temperature;
        const lv::Color humidity;

    } HomescreenColors_t;

    typedef struct
    {
        lv::Arc arc;
        lv::Label value_label;
        lv::Label symbol_label;
        Settings::DisplaySettings::WidgetType type;
        Settings::DisplaySettings::Monitor monitor;
        lv::Color active_color;
    } Widget_t;

    const HomescreenColors_t homescreen_status_colors = {
        .co2_dangerous = lv::rgb(0xEC, 0x1C, 0x24),
        .co2_warning = lv::rgb(0xF0, 0x5A, 0x28),
        .co2_safe = lv::rgb(0x37, 0xB3, 0x4A),
        .temperature = lv::rgb(0xF6, 0x92, 0x1E),
        .humidity = lv::rgb(0x00, 0xAD, 0xEE),

    };

    Widget_t main_widget = {
        .arc = {},
        .value_label = {},
        .symbol_label = {},
        .type = Settings::DisplaySettings::MAIN,
        .monitor = settings.getMainMonitor(),
        .active_color = homescreen_status_colors.co2_dangerous,
    };
    Widget_t upper_widget = {
        .arc = {},
        .value_label = {},
        .symbol_label = {},
        .type = Settings::DisplaySettings::UPPER,
        .monitor = settings.getUpperMonitor(),
        .active_color = homescreen_status_colors.temperature,
    };
    Widget_t lower_widget = {
        .arc = {},
        .value_label = {},
        .symbol_label = {},
        .type = Settings::DisplaySettings::LOWER,
        .monitor = settings.getLowerMonitor(),
        .active_color = homescreen_status_colors.humidity,
    };

    // not really optional, but the object should be initialized during the init() call and not at construct time of
    // this class.
    std::optional<lv::Screen> homescreen_screen;

    void init_widget(Widget_t *widget);
    void update_widget_monitor(Widget_t *widget);
    void update_widget_label(Widget_t *widget, const char *co2, const char *temp, const char *hum, uint16_t co2_value);
    void change_widget(Widget_t *widget, Settings::DisplaySettings::Monitor new_monitor);
    bool getSensorData();

    bool screenSwitch = false;
};

} // namespace UI
#endif /* HOMESCREEN_H */