#include "bootscreen.hpp"
#include "custom_fonts.h"
#include <lvgl.h>
#include <src/font/lv_font.h>
#include <src/lv_api_map_v8.h>
#include <src/misc/lv_color.h>
#include <src/widgets/label/lv_label.h>

// #include <lv/core/screen.hpp>

using namespace UI;

Bootscreen::~Bootscreen() {}

void Bootscreen::init()
{
    bootscreen_screen.emplace();
    bootscreen_screen->remove_flag(LV_OBJ_FLAG_SCROLLABLE).bg_color(lv_color_black());

    boot_text = lv::Label::create(*bootscreen_screen)
                    .text("CO2 MONITOR")
                    .text_color(lv_color_white())
                    .align(LV_ALIGN_CENTER, 0, 0)
                    .text_font(&custom_font_montserrat_48);

    bootscreen_screen->load();

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

/// TODO: Look into changing destroy/delete to hide.
void Bootscreen::destroy()
{
    bootscreen_screen->del();
}

void Bootscreen::handleLongPress()
{
    return;
}