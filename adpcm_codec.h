#include "wave_header.h"

void adpcm_code(HeaderType *outHeader, uint16_t **outdata, const HeaderType inHeader, const uint16_t *indata);

void adpcm_decode(HeaderType *outHeader, uint16_t **outdata, const HeaderType inHeader, const uint16_t *indata);
