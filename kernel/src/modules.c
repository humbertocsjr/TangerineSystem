#include "../kernel.h"
#include "../../config.h"
#include "debug.h"
#include <string.h>

static module_t *_modules[MODULES_MAX];
static uint32_t _modules_count = 0;

void modules_init()
{
    _modules_count = 0;
    module_t *mod = (module_t *)0x100000;
    extern void _etext;
    while(mod < &_etext)
    {
        if(mod->signature1 == 0x12344321 && mod->signature2 == 0x43211234)
        {
            if(_modules_count >= MODULES_MAX)
            {
                debug_print("ERROR: MODULE LIST OVERFLOW\n");
                break;
            }
            _modules[_modules_count++] = mod;
            debug_print("Module found: %s\n", mod->name);
        }
        mod++;
    }
}

module_t *modules_find(char *name)
{
    for (int i = 0; i < _modules_count; i++)
    {
        if(!strcmp(_modules[i]->name, name))
        {
            return _modules[i];
        }
    }
    return 0;
}