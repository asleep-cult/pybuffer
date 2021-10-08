#ifndef PYBUFFER_H
#define PYBUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PYBUFFER_OK             0
#define PYBUFFER_NOMEM         -1
#define PYBUFFER_OUT_OF_BOUNDS -2

#define PYBUFFER_BIG_ENDIAN    0
#define PYBUFFER_LITTLE_ENDIAN 1

#define PYBUFFER_ERROR int *error
#define PYBUFFER_SET_ERROR(CODE) *error = CODE
#define PYBUFFER_ERROR_OCCURED() *error != PYBUFFER_OK

typedef struct pybuffer {
    size_t size;
    char *data;
    bool owned;
} pybuffer;

pybuffer *pybuffer_new(size_t size, PYBUFFER_ERROR);
pybuffer *pybuffer_from_string(char *data, size_t size, PYBUFFER_ERROR);

void pybuffer_free(pybuffer *buffer);

size_t pybuffer_size(pybuffer *buffer);
char *pybuffer_data(pybuffer *buffer);

char pybuffer_charat(pybuffer *buffer, size_t index, PYBUFFER_ERROR);

char *pybuffer_read(pybuffer *buffer, size_t start, size_t stop, PYBUFFER_ERROR);

uint8_t pybuffer_readuint8(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);
int8_t pybuffer_readint8(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);

uint16_t pybuffer_readuint16(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);
int16_t pybuffer_readint16(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);

uint32_t pybuffer_readuint32(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);
int32_t pybuffer_readint32(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);

uint64_t pybuffer_readuint64(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);
int64_t pybuffer_readint64(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);

float pybuffer_readfloat(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);
double pybuffer_readdouble(pybuffer *buffer, size_t offset, int byteorder, PYBUFFER_ERROR);

void pybuffer_write(pybuffer *buffer, char *data, size_t start, size_t stop, PYBUFFER_ERROR);
void pybuffer_fill(pybuffer *buffer, char c, PYBUFFER_ERROR);

void pybuffer_writeuint8(pybuffer *buffer, uint8_t number, size_t offset, int byteorder, PYBUFFER_ERROR);
int8_t pybuffer_writeint8(pybuffer *buffer, int8_t number, size_t offset, int byteorder, PYBUFFER_ERROR);

uint16_t pybuffer_writeuint16(pybuffer *buffer, uint16_t number, size_t offset, int byteorder, PYBUFFER_ERROR);
int16_t pybuffer_writeint16(pybuffer *buffer, int16_t number, size_t offset, int byteorder, PYBUFFER_ERROR);

uint32_t pybuffer_writeuint32(pybuffer *buffer, uint32_t number, size_t offset, int byteorder, PYBUFFER_ERROR);
int32_t pybuffer_writeint32(pybuffer *buffer, int32_t number, size_t offset, int byteorder, PYBUFFER_ERROR);

uint64_t pybuffer_writeuint64(pybuffer *buffer, uint64_t number, size_t offset, int byteorder, PYBUFFER_ERROR);
int64_t pybuffer_writeint64(pybuffer *buffer, int64_t number, size_t offset, int byteorder, PYBUFFER_ERROR);

float pybuffer_writefloat(pybuffer *buffer, float number, size_t offset, int byteorder, PYBUFFER_ERROR);
double pybuffer_writedouble(pybuffer *buffer, double number, size_t offset, int byteorder, PYBUFFER_ERROR);

#endif
