
void *memset(void *dst, int value, unsigned n)
{
    char *dst_c = dst;
    while(n--)
    {
        *dst_c++ = value & 0xff;
    }
    return dst;
}