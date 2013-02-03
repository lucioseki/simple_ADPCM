#include <stdio.h>
#include <stdlib.h>

#include "wave_header.h"
#include "adpcm.h"

int main(int argc, char **argv){
	FILE *infile, *outfile;
	HeaderType inHeader, outHeader;
	segmentHeader *segHeader;
	uint16_t *indata, *outdata = NULL;

	// verifica o uso
	if(argc != 3){
		printf("use: %s infile outfile\n", argv[0]);
		return 1;
	}

	// abre arquivo de entrada
	infile = fopen(argv[1], "rb");
	if(infile == NULL){
		perror(argv[1]);
		exit(1);
	}

	// le arquivo de entrada
	fread(&inHeader, sizeof(HeaderType), 1, infile);
	indata = malloc(inHeader.Subchunk2Size);
	fread(indata, 1, inHeader.Subchunk2Size, infile);

	// verifica formato
	if(inHeader.AudioFormat == PCM_FORMAT){
		segHeader = code(&outHeader, inHeader, &indata);
	}else if(inHeader.AudioFormat == ADPCM_FORMAT){
		decode(&outHeader, &outdata, inHeader, &indata);
	}else{
		printf("Invalid file format\n");
		exit(1);
	}

	// abre arquivo de saida
	outfile = fopen(argv[2], "wb");
	if(outfile == NULL){
		perror(argv[2]);
		exit(1);
	}

	// escreve arquivo de saida
	fwrite(&outHeader, sizeof(HeaderType), 1, outfile);

	if(inHeader.AudioFormat == PCM_FORMAT){
		for(segHeader; segHeader->nextHeader != NULL; segHeader = segHeader->nextHeader){
			// tamanho do audio que deve ser recuperado
			fwrite(&(inHeader.Subchunk2Size), sizeof(int32_t), 1, outfile);

			// primeira amostra do segmento
			fwrite(&(segHeader->keyValue), sizeof(uint16_t), 1, outfile); 

			// quantidade de amostras no segmento
			fwrite(&(segHeader->numSamples), sizeof(uint16_t), 1, outfile);

			// bits por amostra do segmento
			fwrite(&(segHeader->segmentBPS), sizeof(uint16_t), 1, outfile);

			// os valores das amostras comprimidas
			fwrite(segHeader->segment, segHeader->segmentBPS/8, segHeader->numSamples, outfile);
		} 
	}else if(inHeader.AudioFormat == ADPCM_FORMAT){
		
	}

	if(outdata != NULL){
		free(outdata);
	}
	free(indata);
	fclose(outfile);
	fclose(infile);

	return 0;
}
