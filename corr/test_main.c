#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>

#include "normcorr.h"


int main()
{
    FILE *pRefFile = NULL;
    FILE *pTestFile = NULL;

    short *refSig = NULL;
    short *pRefSig = NULL;
    short *testSig = NULL;

    char refFileName[256] = "speech1_m1_16_16.pcm"; // 16K 16bit
    char testFileName[256] = "test.pcm";

    int readLen = 0;

    bool isSimilar = false;


    pRefFile = fopen(refFileName, "rb");
    if ( pRefFile == NULL ) {
        printf("ERROR: Failed to open %s \n", refFileName);
	return 0;
    }

    pTestFile = fopen(testFileName, "w+");
    if ( pTestFile == NULL ) {
        printf("ERROR: Failed to open %s \n", testFileName);
	return 0;
    }

    refSig = (short *)calloc(MAX_SIG_BUFLEN_IN_SAMPLES, sizeof(short));
    if ( refSig == NULL) {
        printf("ERROR: Failed to allocate reference signal buffer !\n");
        return 0;
    }

    testSig = (short *)calloc(MAX_SIG_BUFLEN_IN_SAMPLES, sizeof(short));
    if ( testSig == NULL) {
        printf("ERROR: Failed to allocate test signal buffer !\n");
	return 0;
    }
    
    pRefSig = refSig;
    while (readLen <= (MAX_SIG_BUFLEN_IN_BYTES - SYSTEM_PCM_LEN) &&
	   fread(pRefSig, 1, SYSTEM_PCM_LEN, pRefFile) == SYSTEM_PCM_LEN ) {
       readLen += SYSTEM_PCM_LEN;
       pRefSig += SYSTEM_PCM_LEN>>1; 
    }
    //simply time domain shift to generate test signals
    //printf("readLen %d \n", readLen);
    memcpy(testSig + 320, refSig, readLen);

    //fwrite(testSig, 1, readLen + SHIFT_IN_SAMPLES * sizeof(short), pTestFile); 
    fwrite(refSig + 320, 1, readLen, pTestFile); 
#if 1
    isSimilar = normcorr(refSig + IGNORE_IN_SAMPLES,
                         testSig + IGNORE_IN_SAMPLES,
			 CORR_LEN_IN_SAMPLES);
    printf("Two signals shape is %s similar. \n", isSimilar ? "Strongly" : "Not");
#endif
    fclose(pRefFile);
    fclose(pTestFile);

    free(refSig);
    free(testSig);

    return 0;
}
