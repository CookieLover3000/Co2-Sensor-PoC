#include "bootscreen.hpp"
#include "custom_fonts.h"

#include <lv/lv.hpp>

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
}

void Bootscreen::load()
{
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