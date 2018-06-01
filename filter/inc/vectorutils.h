#ifndef _VECTOR_UTILS_H_
#define _VECTOR_UTILS_H_

void VMoveShorts(short *pSrc, short *pDest, int sampleCnt);

void VMoveShortsSrcStride(short *pSrc, short *pDest, int sampleCnt, int srcStride);


void VMoveShortsDestStride(short *pSrc, short *pDest, int sampleCnt, int destStride);

#endif
