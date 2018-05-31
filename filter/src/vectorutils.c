// vectorutils.c
//

void VMoveShorts(short *pSrc, short *pDest, int sampleCnt)
{
    int i;

    for (i = 0; i < sampleCnt; i++) {
        *pDest++ = *pSrc++;
    }
}

void VMoveShortsSrcStride(short *pSrc, short *pDest, int sampleCnt, int srcStride)
{
    int i;

    for (i = 0; i < sampleCnt; i++) {
        *pDest++ = *pSrc++;
        pSrc += srcStride;
    }
}

void VMoveShortsDestStride(short *pSrc, short *pDest, int sampleCnt, int destStride)
{
    int i;

    for (i = 0; i < sampleCnt; i++) {
        *pDest++ = *pSrc++;
        pDest += destStride;
    }
}
