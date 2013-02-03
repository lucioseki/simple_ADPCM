#include "wave_header.h"

#define MAXDIFF 2047

typedef struct segmentHeader{
	uint16_t keyValue;
	uint16_t numSamples;
	uint16_t segmentBPS; // bits per sample
	uint16_t *segment;
	struct segmentHeader *nextHeader;
} segmentHeader;

segmentHeader *code(HeaderType *outHeader, const HeaderType inHeader, uint16_t **indata);

void decode(HeaderType *outHeader, uint16_t **outdata, const HeaderType inHeader, uint16_t **indata);
