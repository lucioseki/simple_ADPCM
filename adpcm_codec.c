#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "adpcm_codec.h"

#define NUM_SEGMENTS 200 // numero constante de segmentos

void adpcm_code(HeaderType *outHeader, uint16_t **outdata, const HeaderType inHeader, uint16_t **indata){
	int numSamples = (inHeader.Subchunk2Size * 8) / inHeader.BitsPerSample; // quantidade de amostras do audio
	int segmentSize = numSamples / NUM_SEGMENTS; // quantidade de amostras por segmento
	int i, j, diff, aux;
	int chunksize = 0;
	*outdata = malloc(inHeader.Subchunk2Size); // no maximo vai ter o tamanho do arquivo original.
	uint16_t numBits, bufferPos, bufferSpace;

	// copia e altera o cabecalho
	*outHeader = inHeader;
	outHeader->AudioFormat = ADPCM_FORMAT;

	bufferPos = 0;
	// armazena o tamanho do arquivo original
	(*outdata)[bufferPos] = inHeader.Subchunk2Size >> sizeof(uint16_t) * 8;
	bufferPos++;
	(*outdata)[bufferPos] = inHeader.Subchunk2Size << sizeof(uint16_t) * 8;

	// para cada segmento
	for(i = 0; i <= NUM_SEGMENTS; i++){
		// armazena a amostra-chave
		(*outdata)[bufferPos] = (*indata)[i];
		bufferPos++;
		chunksize += sizeof(uint16_t);

		// calcula a maior diferenca
		diff = 0;
		for(j = 1; j < segmentSize; j++){
			aux = (*indata)[i + j] - (*indata)[i + j - 1];
			if(aux < 0) aux = -aux; // inverte o sinal negativo
			if(aux > diff) diff = aux; // atualiza a maior diferenca
		}

		// calcula a quantidade de bits por amostra neste segmento
		numBits = 0;
		while(pow(2, numBits) < diff) numBits++;
		numBits++; // mais um bit do sinal // armazena a quantidade de bits por amostra neste segmento
		(*outdata)[bufferPos] = numBits;
		bufferPos++;
		chunksize += sizeof(uint16_t);

		// esvazia buffer
		bufferSpace = sizeof(uint16_t) * 8;

		// para cada amostra no segmento
		for(j = 1; j < segmentSize; j++){
			// calcula a diferenca
			diff = (*indata)[i+j] - (*indata)[i + j - 1];

			// desloca os bits ate onde nao foi ocupado ainda, e armazena no buffer
			bufferSpace -= numBits;
			(*outdata)[bufferPos] += diff << bufferSpace;

			// se ainda tem bufferSpace, pega a proxima amostra;
			// se encher o bufferSpace
			if(bufferSpace <= numBits){

				// prepara para preencher a proxima posicao do buffer de saida
				bufferPos++;
				chunksize += sizeof(uint16_t);

				// se completou certinho
				if(bufferSpace == numBits){
					// esvazia buffer;
					bufferSpace = sizeof(uint16_t) * 8;
				}

				// se nao coube tudo
				if(bufferSpace < numBits){
					// o que sobrou coloca no inicio da proxima posicao
					bufferSpace += bufferSpace - numBits;
					(*outdata)[bufferPos] += diff << bufferSpace;
				}

			} // end se nao couber no buffer
		} // end para cada amostra no segmento
	} // para cada segmento no arquivo
	
	// atualiza o tamanho do chunk de dados
	outHeader->Subchunk2Size = chunksize;

}

void adpcm_decode(HeaderType *outHeader, uint16_t **outdata, const HeaderType inHeader, uint16_t **indata){
	*outHeader = inHeader;
	outHeader->AudioFormat = PCM_FORMAT;

	// realocar memoria do arquivo original
	int32_t chunksize;
	chunksize = (*indata)[0] << sizeof(uint16_t) * 8;
	chunksize += (*indata)[1];

	outHeader->Subchunk2Size = chunksize;
	*outdata = malloc(chunksize);
}
