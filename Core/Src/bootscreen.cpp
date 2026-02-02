#include "bootscreen.hpp"
#include "custom_fonts.h"
#include <lvgl.h>
#include <src/display/lv_display.h>
#include <src/font/lv_font.h>
#include <src/lv_api_map_v8.h>
#include <src/misc/lv_color.h>
#include <src/widgets/label/lv_label.h>

using namespace UI;

Bootscreen::~Bootscreen()
{
    destroy();
}

void Bootscreen::init()
{
    bootscreen_screen = lv_obj_create(NULL);
    lv_obj_clear_flag(bootscreen_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(bootscreen_screen, lv_color_black(), LV_PART_MAIN);

    boot_text = lv_label_create(bootscreen_screen);
    lv_label_set_text(boot_text, "CO2 MONITOR");
    lv_obj_set_style_text_color(boot_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(boot_text, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(boot_text, &custom_font_montserrat_48, LV_PART_MAIN);

    lv_screen_load(bootscreen_screen);

    timer.start(bootDuration);
}

bool Bootscreen::shouldSwitch()
{
    return done;
}

void Bootscreen::update()
{
    if (timer.isExpired())
        done = true;
}

void Bootscreen::destroy()
{
    if (bootscreen_screen)
        lv_obj_del_async(bootscreen_screen);
    bootscreen_screen = NULL;
    boot_text = NULL;
}

void Bootscreen::handleLongPress()
{
    return;
}