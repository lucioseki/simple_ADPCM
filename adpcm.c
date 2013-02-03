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
	uint16_t diff, auxdiff;

	int i, j, numBits, firstPos;
	int numSamples = inHeader.Subchunk2Size  * 8 / inHeader.BitsPerSample;

	// processa as amostras
	for(i = 0; i < numSamples - 1; i++){
		firstPos = i;

		h->keyValue = (*indata)[i]; // primeira amostra do segmento

		// encontra a maior diferenca
		diff = auxdiff = 0;
		while(i < numSamples - 1){
			auxdiff = (*indata)[i] - (*indata)[i + 1];

			// se for negativo, troca o sinal
			if ( ((int16_t)auxdiff) < 0 ) auxdiff = -auxdiff;

			// se a diferenca for muito grante, fim do segmento
			if(auxdiff > MAXDIFF) break;

			// armazena a maior diferenca
			if(auxdiff > diff) diff = auxdiff;

			i++;
		}

		// quantidade de amostras no segmento
		h->numSamples = i - firstPos + 1;

		// quantidade de bits por amostra
		numBits = 0;
		while(pow(2, numBits) < diff) numBits++;
		numBits++; // bit do sinal
		h->segmentBPS = numBits;

		// cria o segmento comprimido
		segment = malloc( (h->numSamples * numBits / 8) + 1);
		h->segment = segment;

		// comprime o segmento
		buffer = 0;
		bufferPos = 0;
		for(j = firstPos; j < i; j++){
			diff = (*indata)[j] - (*indata)[j+1]; 

			// desloca os bits para o inicio
			diff = diff << (sizeof(uint16_t) * 8 - numBits);

			// se tiver espaco no buffer
			if( (sizeof(uint16_t) * 8) - bufferPos >= numBits){

				// armazena no buffer
				buffer += diff >> bufferPos;
				bufferPos += numBits;

			}else{ // se nao couber no buffer
				// salva o que couber
				buffer += diff >> bufferPos;

				// salva no segmento e avança a posição
				*segment = buffer;
				segment++;

				// reseta o buffer
				buffer = 0;

				// salva o que nao coube no inicio do buffer
				buffer += diff << (sizeof(uint16_t) * 8 - bufferPos);

				// avanca a posicao no buffer
				bufferPos = (sizeof(uint16_t) * 8 - bufferPos - numBits);
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

void decode(HeaderType *outHeader, uint16_t **outdata, const HeaderType inHeader, uint16_t **indata){

}
