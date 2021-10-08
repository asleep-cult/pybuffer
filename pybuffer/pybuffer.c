#include <stdlib.h>
#include <string.h>

#include "pybuffer.h"

void ensure_owned(pybuffer *buffer, PYBUFFER_ERROR)
{
    if (!buffer->owned) {
        unsigned char *data = malloc(buffer->size);
        if (data == NULL) {
            PYBUFFER_SET_ERROR(PYBUFFER_NOMEM);
            return;
        }
        memmove(data, buffer->data, buffer->size);
        buffer->data = data;
        buffer->owned = true;
    }
}

pybuffer *pybuffer_new(size_t size, PYBUFFER_ERROR)
{
    pybuffer *buffer = malloc(sizeof(*buffer));
    if (buffer == NULL) {
        goto nomem;
    }
    if (size > 0) {
        buffer->data = malloc(size);
        if (buffer->data == NULL) {
            goto nomem;
        }
    }
    buffer->size = size;
    buffer->owned = true;
    return buffer;

nomem:
    PYBUFFER_SET_ERROR(PYBUFFER_NOMEM);
    free(buffer);
    return NULL;
}

pybuffer *pybuffer_from_string(unsigned char *data, size_t size, PYBUFFER_ERROR)
{
    pybuffer *buffer = malloc(sizeof(*buffer));
    if (buffer == NULL) {
        PYBUFFER_ERROR(PYBUFFER_ERROR);
        return NULL;
    }
    buffer->size = size;
    buffer->owned = false;
    buffer->data = data;
    return buffer;
}

void pybuffer_free(pybuffer *buffer)
{
    if (buffer->owned) {
        free(buffer->data);
    }
    free(buffer);
}

size_t pybuffer_size(pybuffer *buffer) { return buffer->size; }
unsigned char *pybuffer_data(pybuffer *buffer) { return buffer->data; }

unsigned char pybuffer_charat(pybuffer *buffer, size_t index, PYBUFFER_ERROR)
{
    if (index >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return 0;
    }
    return *(buffer->data + index);
}

void pybuffer_setcharat(pybuffer *buffer, size_t index, unsigned char c, PYBUFFER_ERROR)
{
    if (index >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return;
    }
    ensure_owned(buffer, error);
    if (PYBUFFER_ERROR_OCCURED()) {
        return;
    }
    *(buffer->data + index) = c;
}

unsigned char *pybuffer_read(pybuffer *buffer, size_t start, size_t stop, PYBUFFER_ERROR)
{
    if (start >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return NULL;
    }
    if (stop >= buffer->size) {
        stop = buffer->size;
    }
    unsigned char *data = malloc(stop - start);
    if (data == NULL) {
        PYBUFFER_SET_ERROR(PYBUFFER_NOMEM);
        return NULL;
    }
    memmove(data, buffer->data + start, stop - start);
    return data;
}

int pybuffer_write(pybuffer *buffer, unsigned char *data, size_t size, size_t offset, PYBUFFER_ERROR)
{
    if (offset >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return 0;
    }
    if (size + offset >= buffer->size) {
        size = buffer->size;
    }
    ensure_owned(buffer, error);
    if (PYBUFFER_ERROR_OCCURED()) {
        return 0;
    }
    memmove(buffer->data + offset, data, size);
    return size;
}

void pybuffer_fill(pybuffer *buffer, unsigned char c, PYBUFFER_ERROR)
{
    ensure_owned(buffer, error);
    if (PYBUFFER_ERROR_OCCURED()) {
        return;
    }
    memset(buffer->data, c, buffer->size);
}
