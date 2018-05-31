#include <stdio.h>
#include <stdlib.h>

#include "polyphasefir.h"

static int doParseCommand();

int main(int argc, char* argv[])
{
    // initialize polyphase FIR filters per input arguments.
    //
    int decimFactor = 3;
    int interpFactor = 1;

    pfir_upFilterState_t *firUp = NULL;
    pfir_dnFilterState_t *firDn = NULL;

    FILE *pInput = NULL;
    FILE *pOutput = NULL;

    char inFile[256] = "Sweep_sine_48K_16bit.raw";
    char outFile[256]= "out.pcm";

    Uint16 frameLength = 960; // temp value, should be determined by sample rate
    Uint16 readLength = 0;
    Uint16 writeLength = 0;
    Uint16 firOutSampleCnt = 0;
    short inBuffer[480];
    short outBuffer[480];
 

    // eg: resampler -r 48000 -d 2  -i in.pcm -o out.pcm    
    if ( decimFactor == 2 || decimFactor == 3 || decimFactor == 6 ) {
        Int16* filterCfg = pfir_getDnFilterConfig(decimFactor);
        firDn = malloc(sizeof *firDn);

        (void)pfir_initDnFilter(firDn, filterCfg);
    }
    
    // eg: resampler -r 8000 -u 6  -i in.pcm -o out.pcm    
    if ( interpFactor == 2 || interpFactor == 3 || interpFactor == 6 ) {
        Int16* filterCfg = pfir_getUpFilterConfig(interpFactor);
        firUp = malloc(sizeof *firUp);

        (void)pfir_initUpFilter(firUp, filterCfg);
    }

    // open files where to read and write data
    if (firDn == NULL && firUp == NULL ) {
        printf("WARN:No resampling requested ! \n");
        return 0;
    }

    pInput = fopen(inFile, "r");
    pOutput = fopen(outFile, "w");
    if (pInput == NULL || pOutput == NULL) {
        printf("Error: Failed to open file ! \n");
        return 0;
    }
    readLength = frameLength;
    // loop to resample the input signal every 10ms
    while (fread(inBuffer, 1, readLength, pInput) == readLength) {
        if (firDn) {
            pfir_dnFilter(firDn, inBuffer, outBuffer,
                          readLength >> 1, &firOutSampleCnt);
            writeLength = firOutSampleCnt << 1;
            fwrite(outBuffer, 1, writeLength, pOutput); 
        } else if (firUp) {
            pfir_upFilter(firUp, inBuffer, outBuffer,
                          readLength >> 1, &firOutSampleCnt);
            writeLength = firOutSampleCnt << 1;
            fwrite(outBuffer, 1, writeLength, pOutput); 
        } else {
            
        }
    }

    return 0; 
}
