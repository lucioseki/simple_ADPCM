#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "adpcm.h"

segmentHeader *code(outHeader, inHeader, indata)
	HeaderType *outHeader;
	const HeaderType inHeader;
	uint16_t **indata;
{
	*outHeader = inHeader;
	outHeader->AudioFormat = ADPCM_FORMAT;

	segmentHeader *firstHeader = malloc(sizeof(segmentHeader));
	segmentHeader *h = firstHeader;
	uint16_t *segment, buffer, bufferPos;

	int i, j, diff, auxdiff, numBits, firstPos;
	int numSamples = inHeader.Subchunk2Size  * 8 / inHeader.BitsPerSample;

	// processa as amostras
	for(i = 0; i < numSamples - 1; i++){
		firstPos = i;

		h->keyValue = (*indata)[i]; // primeira amostra do segmento

		// encontra a maior diferenca
		diff = auxdiff = 0;
		do {
			auxdiff = diff;

			diff = (*indata)[i] - (*indata)[i + 1];

			// se for negativo, troca o sinal
			if (diff < 0) diff = -diff;

			// se a diferenca for muito grante, fim do segmento
			if(diff > MAXDIFF || diff < -MAXDIFF) break;

			// armazena a maior diferenca
			if(auxdiff > diff) diff = auxdiff;

			i++;
		}while(i < numSamples - 1);

		// quantidade de amostras no segmento
		h->numSamples = i - firstPos + 1;

		// quantidade de bits por amostra
		numBits = 0;
		while(pow(2, numBits) < diff) numBits++;
		numBits++; // bit do sinal
		h->segmentBPS = numBits;

		// cria o segmento comprimido
		segment = malloc(h->numSamples * numBits / 8 + 1);
		h->segment = segment;

		// comprime o segmento
		buffer = 0;
		bufferPos = sizeof(uint16_t) * 8;
		for(j = firstPos; j < i; j++){
			diff = (*indata)[j] - (*indata)[j+1]; 

			// se tiver espaco no buffer
			if(bufferPos >= numBits){
				bufferPos -= numBits;
				// buffer += diff << bufferPos;
			}else{ // se nao couber no buffer
				// salva o que couber
				// buffer += diff << (bufferPos - numBits);

				// salva no segmento e avança a posição
				*segment = buffer;
				segment++;

				// reseta o buffer
				// bufferPos = sizeof(uint16_t) * 8;
				
				// salva o que nao coube
				// buffer += (diff >> (bufferPos - numBits)) << bufferPos;
			}
		}

		// cria proximo segmento
		if(i < numSamples -1){
			h->nextHeader = malloc(sizeof(segmentHeader));
			h = h->nextHeader;
		}else{ // ultimo segmento
			h->nextHeader = NULL;
		}
	}

	return firstHeader;
}

void decode(HeaderType *outHeader, uint16_t **outdata, const HeaderType inHeader, segmentHeader *indata){

}
