#include <string.h>

void* memmove(void* dst, const void* src, size_t len)
{
    char* dst_ptr;
    const char* src_ptr;
    size_t i;

    dst_ptr = dst;
    src_ptr = src;

    /* No overlap */
    if (src_ptr + len < dst_ptr || dst_ptr + len < src_ptr)
        return memcpy(dst, src, len);
    else if (src_ptr < dst_ptr)
    {
        for (i = len; i > 0; --i)
            dst_ptr[i - 1] = src_ptr[i - 1];
    }
    else
    {
        for (i = 0; i < len; ++i)
            dst_ptr[i] = src_ptr[i];
    }
    return dst;
}
