#pragma once

#include <stdint.h>

typedef struct img_t
{
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t size;
    void *buffer;
} img_t;

typedef struct video_t
{
    img_t *(*get_buffer)();
    uint8_t *(*get_background_pattern)();
    void (*set_background_pattern)(uint8_t *pattern);
    void (*fill_pattern)(img_t *, uint8_t *pattern);
} video_t;

extern video_t _video;

void video_init();