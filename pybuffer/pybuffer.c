#include <stdlib.h>
#include <string.h>

#include "pybuffer.h"

#define PYBUFFER_BASEOFFSET(OFFSET, BYTEORDER, SIZE) \
    BYTEORDER == PYBUFFER_BIG_ENDIAN ? OFFSET + SIZE : OFFSET

#define PYBUFFER_OFFSET(BASE_OFFSET, BYTEORDER, POSITION) \
    BYTEORDER == PYBUFFER_BIG_ENDIAN ? BASE_OFFSET - POSITION : BASE_OFFSET + POSITION

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
    return buffer->data[index];
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
    buffer->data[index] = c;
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

void pybuffer_writeuint8(pybuffer *buffer, uint8_t number, size_t offset, int byteorder, PYBUFFER_ERROR)
{
    if (offset + 1 >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
    }
    buffer->data[offset] = number & 0xFF;
}

void pybuffer_writeint8(pybuffer *buffer, int8_t number, size_t offset, int byteorder, PYBUFFER_ERROR)
{
    pybuffer_writeuint8(buffer, (uint8_t)number, offset, byteorder, error);
}

void pybuffer_writeuint16(pybuffer *buffer, uint16_t number, size_t offset, int byteorder, PYBUFFER_ERROR)
{
    if (offset + 2 >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return;
    }
    offset = PYBUFFER_BASEOFFSET(offset, byteorder, 2);
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 0)] = number & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 1)] = (number >> 8) & 0xFF;
}

void pybuffer_writeint16(pybuffer *buffer, int16_t number, size_t offset, int byteorder, PYBUFFER_ERROR)
{
    return pybuffer_writeuint16(buffer, (int16_t)number, offset, byteorder, error);
}

void pybuffer_writeuint32(pybuffer *buffer, uint32_t number, size_t offset, int byteorder, PYBUFFER_ERROR)
{
    if (offset + 4 >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return;
    }
    offset = PYBUFFER_BASEOFFSET(offset, byteorder, 4);
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 0)] = number & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 1)] = (number >> 8) & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 2)] = (number >> 16) & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 3)] = (number >> 24) & 0xFF;
}

void pybuffer_writeint32(pybuffer *buffer, int32_t number, size_t offset, int byteorder, PYBUFFER_ERROR)
{
    return pybuffer_writeuint32(buffer, (uint32_t)number, offset, byteorder, error);
}

void pybuffer_writeuint64(pybuffer *buffer, uint64_t number, size_t offset, int byteorder, PYBUFFER_ERROR)
{
    if (offset + 8 >= buffer->size) {
        PYBUFFER_SET_ERROR(PYBUFFER_OUT_OF_BOUNDS);
        return;
    }
    offset = PYBUFFER_BASEOFFSET(offset, byteorder, 4);
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 0)] = number & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 1)] = (number >> 8) & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 2)] = (number >> 16) & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 3)] = (number >> 24) & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 4)] = (number >> 32) & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 5)] = (number >> 40) & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 6)] = (number >> 48) & 0xFF;
    buffer->data[PYBUFFER_OFFSET(offset, byteorder, 7)] = (number >> 56) & 0xFF;
}

void pybuffer_writeint64(pybuffer *buffer, int64_t number, size_t offset, int byteorder, PYBUFFER_ERROR)
{
    return pybuffer_writeuint64(buffer, (uint64_t)number, offset, byteorder, error);
}
