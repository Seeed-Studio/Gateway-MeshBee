#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuf.h"

//modify void *buff to char *buff
int init_ringbuffer(struct ringbuffer *r, uint8 *buff, uint32 size)
{
    r->buf = buff;
    r->size = size;
    clear_ringbuffer(r);

    return 0;
}

void free_ringbuffer(struct ringbuffer *r)
{
    //free(r->buf);
}

void clear_ringbuffer(struct ringbuffer *r)
{
    r->begin = 0;
    r->end = 0;
}

uint32 ringbuffer_free_space(struct ringbuffer *r)
{
    if (r->begin == 0 && r->end == 0) return r->size;

    if (r->begin <= r->end) return r->size - (r->end - r->begin) - 1;
    else return r->begin - r->end - 1;
}

uint32 ringbuffer_data_size(struct ringbuffer *r)
{
    if (r->begin == 0 && r->end == 0) return 0;

    if (r->begin <= r->end) return r->end - r->begin + 1;
    else return r->size - (r->begin - r->end) + 1;
}


void ringbuffer_push(struct ringbuffer *r, const void *data, uint32 size)
{
    if (ringbuffer_free_space(r) < size) return;

    if (r->begin == 0 && r->end == 0)
    {
        memcpy(r->buf, data, size);
        r->begin = r->buf;
        r->end = r->buf + size - 1;
        return;
    }

    r->end++;
    if (r->begin < r->end)
    {
        if ((uint32)(r->buf + r->size - r->end/*begin*/) >= size)
        {
            /* we can fit without cut */
            memcpy(r->end, data, size);
            r->end += size - 1;
        } else
        {
            /* make a cut */
            uint32 s = r->buf + r->size - r->end;
            memcpy(r->end, data, s);
            size -= s;
            memcpy(r->buf, (char *)data + s, size);
            r->end = r->buf + size - 1;
        }
    } else
    {
        memcpy(r->end, data, size);
        r->end += size - 1;
    }
}

void ringbuffer_pop(struct ringbuffer *r, void *data, uint32 size)
{
    if (ringbuffer_data_size(r) < size) return;

    int need_clear = 0;
    if (ringbuffer_data_size(r) == size) need_clear = 1;

    if (r->begin < r->end)
    {
        if (data) memcpy(data, r->begin, size);
        r->begin += size;
    } else
    {
        if ((uint32)(r->buf + r->size - r->begin) >= size)
        {
            if (data) memcpy(data, r->begin, size);
            r->begin += size;
        } else
        {
            uint32 s = r->buf + r->size - r->begin;
            if (data) memcpy(data, r->begin, s);
            size -= s;
            if (data) memcpy((char *)data + s, r->buf, size);
            r->begin = r->buf + size;
        }
    }

    if (need_clear) clear_ringbuffer(r);
}

void ringbuffer_read(struct ringbuffer *r, void *data, uint32 size)
{
    if (ringbuffer_data_size(r) < size) return;

    if (r->begin < r->end) memcpy(data, r->begin, size);
    else
    {
        if ((uint32)(r->buf + r->size - r->begin) >= size) memcpy(data, r->begin, size);
        else
        {
            uint32 s = r->buf + r->size - r->begin;
            memcpy(data, r->begin, s);
            size -= s;
            memcpy((char *)data + s, r->buf, size);
        }
    }
}