#include "video.h"
#include <string.h>

video_t _video;

void video_init()
{
    memset(&_video, 0, sizeof(video_t));
}