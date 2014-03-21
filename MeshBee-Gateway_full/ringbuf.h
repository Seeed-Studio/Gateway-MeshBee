#ifndef TERMBOX_RINGBUFFER_H
#define TERMBOX_RINGBUFFER_H

#include "common.h"
#define ERINGBUFFER_ALLOC_FAIL -1

struct ringbuffer {
    uint8 *buf;
    uint32 size;

    uint8 *begin;
    uint8 *end;
};

typedef struct ringbuffer RingBuffer;

int init_ringbuffer(struct ringbuffer *r, uint8 *buff, uint32 size);
void free_ringbuffer(struct ringbuffer *r);
void clear_ringbuffer(struct ringbuffer *r);
uint32 ringbuffer_free_space(struct ringbuffer *r);
uint32 ringbuffer_data_size(struct ringbuffer *r);
void ringbuffer_push(struct ringbuffer *r, const void *data, uint32 size);
void ringbuffer_pop(struct ringbuffer *r, void *data, uint32 size);
void ringbuffer_read(struct ringbuffer *r, void *data, uint32 size);

#endif