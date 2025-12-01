#include "../src/video.h"
#include "../kernel.h"
#include <string.h>
#include <stdint.h>

static img_t _buffer;
static uint8_t _raw_buffer[38400];

img_t *get_buffer()
{
    return &_buffer;
}

MODULE(video_vga_mono)
{
    memset(&_buffer, 0, sizeof(img_t));
    _buffer.width = 640;
    _buffer.height = 480;
    _buffer.pitch = 80;
    _buffer.size = sizeof(_raw_buffer);
    _buffer.buffer = _raw_buffer;
    _video.get_buffer = get_buffer;
    return true;
}
