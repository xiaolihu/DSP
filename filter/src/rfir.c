#include <stdlib.h>
#include "rfir.h"


firFilter_t* initRFirFilter(int sampleCnt, int nTap, const short *pCoeffs)
{
    firFilter_t *pFir;
    int bufferSize = sampleCnt + nTap - 1;

    pFir = (firFilter_t*)calloc(1, sizeof(firFilter_t));

    pFir->sampleCnt = sampleCnt;
    pFir->nTap = nTap;
    pFir->pCoeffs = pCoeffs;
    pFir->overlapBuffer = (short *)calloc(1, bufferSize); // so far, no requirement on alignment

    return pFir;
}

void deinitRFirFilter(firFilter_t *pFir)
{
    free(pFir->overlapBuffer);
    free(pFir);
}

void computeRFir(short *inBuf,
                 const short *coefBuf,
                 short *outBuf,
                 int ntap, // filter length
                 int sampleCnt, // number of dot product to be calculated
                 int decFactor, // decimation factor, for downsampling usecase
                 int strideStep) // may interleave to store filtered data in half-band filter
{
    int i, j;
    long sum;

    short *xPtr = inBuf;
    const short *coefPtr = coefBuf;

    for (i = 0; i < sampleCnt; i++) {
        sum = 0;
        //filtering loop
        for (j = 0; j < ntap; j++) {
            sum += *xPtr++ * *coefPtr++;
        }

	sum >>= 15;
        //saturate 
	if (sum > 32767) {
            sum = 32767;
        } else if (sum < -32768) {
            sum = -32768;
        }

	*outBuf = (short)sum;
	outBuf += strideStep;

	xPtr = inBuf + decFactor; // do decimation before filtering if decfactore > 1
	coefPtr = coefBuf;
    }
}

