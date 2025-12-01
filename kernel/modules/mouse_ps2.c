#include "../kernel.h"
#include "bus_ps2.h"


static void _action(regs_t *regs)
{

}

static void _setup()
{

}

MODULE(mouse_ps2)
{
    bus_ps2_set_device(1, &_action, &_setup);
    return true;
}