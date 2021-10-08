#include <stdlib.h>
#include <string.h>

#include "pybuffer.h"

void ensure_owned(pybuffer *buffer, PYBUFFER_ERROR)
{
    if (!buffer->owned) {
        char *data = malloc(buffer->size);
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

pybuffer *pybuffer_from_string(char *data, size_t size, PYBUFFER_ERROR)
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
char *pybuffer_data(pybuffer *buffer) { return buffer->data; }

char pybuffer_charat(pybuffer *buffer, size_t index, PYBUFFER_ERROR)
{
    if (index >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return 0;
    }
    return *(buffer->data + index);
}

char *pybuffer_read(pybuffer *buffer, size_t start, size_t stop, PYBUFFER_ERROR)
{
    if (start >= buffer->size || stop >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return NULL;
    }
    char *data = malloc(stop - start);
    if (data == NULL) {
        PYBUFFER_SET_ERROR(PYBUFFER_NOMEM);
        return NULL;
    }
    memmove(data, buffer->data, stop - start);
    return data;
}

void pybuffer_write(pybuffer *buffer, char *data, size_t start, size_t stop, PYBUFFER_ERROR)
{
    if (start >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return;
    }
    if (stop >= buffer->size) {
        stop = buffer->size;
    }
    ensure_owned(buffer, error);
    if (PYBUFFER_ERROR_OCCURED()) {
        return;
    }
    memmove(buffer->data, data + start, stop - start);
}

void pybuffer_fill(pybuffer *buffer, char c, PYBUFFER_ERROR)
{
    ensure_owned(buffer, error);
    if (PYBUFFER_ERROR_OCCURED()) {
        return;
    }
    memset(buffer->data, c, buffer->size);
}
