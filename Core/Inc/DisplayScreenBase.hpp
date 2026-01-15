#ifndef DISPLAY_SCREEN_BASE_H
#define DISPLAY_SCREEN_BASE_H

namespace UI
{
class DisplayScreenBase
{
  public:
    virtual ~DisplayScreenBase() = default;
    virtual void init(void) = 0;
    virtual void update(void) = 0;
    virtual void destroy(void) = 0;
    virtual bool shouldSwitch(void) = 0;
};
} // namespace UI

#endif /* DISPLAY_SCREEN_BASE_H */