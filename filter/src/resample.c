#include "rfir.h"
#include "vectorutils.h"

// downsample
//
// Two-step to downsampling:
// 1. Reduce high-frequency signal components with a digital lowpass filter.
// 2. Downsample the filtered signal by M; that is, keep only every Mth sample.
//
// Here takes a Factor M = 2 as an example
//
void decimate2to1(firFilter_t *pFir, short *pInput, short *pOutput)
{
    // memory allocation: |****nTap - 1**|*****sampleCnt*****|
    VMoveShortsSrcStride(pInput, pFir->overlapBuffer + pFir->nTap - 1, pFir->sampleCnt, 1);

    computeRFir(pFir->overlapBuffer,
                pFir->pCoeffs,
                pOutput,
                pFir->nTap,
                pFir->sampleCnt,
                2,   // Factor M = 2
                1);

    // update nTap -1 history data
    VMoveShorts(pFir->overlapBuffer + pFir->sampleCnt, pFir->overlapBuffer, pFir->nTap - 1);
}



void interp1to2(firFilter_t *pFir, short *pInput, short *pOutput)
{
    VMoveShortsSrcStride(pInput, pFir->overlapBuffer + pFir->nTap - 1, pFir->sampleCnt, 1);
    //half-band filtering
    computeRFir(pFir->overlapBuffer,
                pFir->pCoeffs,
                pOutput,
                pFir->nTap,
                pFir->sampleCnt,
                1, // no decimation
                2); // interleave to store data

    // to implement a half-band interpolating
    VMoveShortsDestStride(pInput + ((pFir->nTap >> 1) - 1),
                          pOutput + 1,
                          pFir->sampleCnt,
                          2);

    VMoveShorts(pFir->overlapBuffer + pFir->sampleCnt, pFir->overlapBuffer, pFir->nTap - 1);
}
