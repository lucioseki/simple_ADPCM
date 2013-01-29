#include <stdio.h>
#include <stdlib.h>

#include "wave_header.h"
#include "adpcm_codec.h"

int main(int argc, char **argv){
	FILE *infile, *outfile;
	HeaderType inHeader, outHeader;
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
		adpcm_code(&outHeader, &outdata, inHeader, &indata);
	}else if(inHeader.AudioFormat == ADPCM_FORMAT){
		adpcm_decode(&outHeader, &outdata, inHeader, &indata);
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
	fwrite(outdata, 1, outHeader.Subchunk2Size, outfile);

	if(outdata != NULL){
		free(outdata);
	}
	free(indata);
	fclose(outfile);
	fclose(infile);

	return 0;
}
