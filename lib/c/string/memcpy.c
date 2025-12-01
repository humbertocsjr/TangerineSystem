
void *memcpy(void *dst, void *src, unsigned n)
{
    char *dst_c = dst;
    char *src_c = src;
    while(n--)
    {
        *dst_c++ = *src_c++;
    }
    return dst;
}