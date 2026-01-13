#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include "DisplayScreenBase.hpp"
#include "lvgl.h"
#include "SensorHandler.hpp"


namespace UI
{

class Homescreen : public DisplayScreenBase
{
  public:
    Homescreen(App::SensorHandler &s) : sensor(s){}
    ~Homescreen();

    virtual void init(void) override;
    virtual void update(void) override;
    virtual void destroy(void) override;

  private:
    App::SensorHandler &sensor;
    typedef enum
    {
        CO2,
        TEMPERATURE,
        HUMIDITY
    } Monitor;

    typedef enum
    {
        MAIN,
        UPPER,
        LOWER
    } WidgetType;
    typedef struct
    {
        const lv_color_t co2_dangerous;
        const lv_color_t co2_warning;
        const lv_color_t co2_safe;
        const lv_color_t temperature;
        const lv_color_t humidity;

    } HomescreenColors_t;

    typedef struct
    {
        lv_obj_t *arc;
        lv_obj_t *value_label;
        lv_obj_t *symbol_label;
        WidgetType type;
        Monitor monitor;
        lv_color_t active_color;
    } Widget_t;

    const HomescreenColors_t homescreen_status_colors = {
        .co2_dangerous = LV_COLOR_MAKE(0xEC, 0x1C, 0x24),
        .co2_warning = LV_COLOR_MAKE(0xF0, 0x5A, 0x28),
        .co2_safe = LV_COLOR_MAKE(0x37, 0xB3, 0x4A),
        .temperature = LV_COLOR_MAKE(0xF6, 0x92, 0x1E),
        .humidity = LV_COLOR_MAKE(0x00, 0xAD, 0xEE),

    };

    Widget_t main_widget = {
        .arc = NULL,
        .value_label = NULL,
        .symbol_label = NULL,
        .type = MAIN,
        .monitor = CO2,
        .active_color = homescreen_status_colors.co2_dangerous,
    };
    Widget_t upper_widget = {
        .arc = NULL,
        .value_label = NULL,
        .symbol_label = NULL,
        .type = UPPER,
        .monitor = TEMPERATURE,
        .active_color = homescreen_status_colors.temperature,
    };
    Widget_t lower_widget = {
        .arc = NULL,
        .value_label = NULL,
        .symbol_label = NULL,
        .type = LOWER,
        .monitor = HUMIDITY,
        .active_color = homescreen_status_colors.humidity,
    };

    lv_obj_t *homescreen_screen = NULL;

    void init_widget(Widget_t *widget);
    void update_widget_label(Widget_t *widget, const char *co2, const char *temp, const char *hum, uint16_t co2_value);
    void change_widget(Widget_t *widget, Monitor new_monitor);
};

} // namespace UI
#endif /* HOMESCREEN_H */