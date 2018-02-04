// 2018-01-18
// normcorr.c

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

#include "normcorr.h"

// per cauchy-swarchz inequality
//
//          sum{ x[n] * y[n] }
// -------------------------------------  <= 1
// sqrt{ (sum(x[n] ^ 2) * sum(y[n] ^ 2) }  
//
// do normalized cross-correlation for judging whether two sequences are similar.
// if result is close to 1, meaning they are exactly same.
// if result is 0, no correlation at all.
// if result is -1, two signals are totally opposite, PI shift in phase.
//
// param[in] refSig   reference signal
// param[in] testSig  signal to be decided if similar to refSig
//
// return    true if strongly similar, false if not

bool normcorr(short *refSig, short *testSig, uint32_t corrLength)
{
    double *xcorr = NULL;
    double sum, sumX, sumY;
    short *xn, *yn;
    double xcorrMax = 0;
    double normxcorr = 0;
    uint32_t xcorrMaxIndex = 0;
    int i, j, d;

    xcorr = (double *)calloc(WIN_LEN, sizeof(double));
    // TIME Domain correlation
    for (d = 0, i = 0; d < WIN_LEN; d++, i++ ) {
        sum = 0.0;
        xn = refSig;
	yn = testSig + d; // shift d sample
    	for (j = 0; j < corrLength; j++) {
           sum += *xn++ * *yn++; //sum{ x[n] * y[n] }
	}
        xcorr[i] = sum / corrLength;
	//printf("xcorr[%d]: %lf \n", i, sum);
    }

    // find the peak to estimate delay
    for (i = 0; i < WIN_LEN; i++) {
        if (xcorr[i] > xcorrMax) {
            xcorrMax = xcorr[i];
            xcorrMaxIndex = i;
        }
    }
    //printf("peak point %d: %f \n", xcorrMaxIndex, xcorr[xcorrMaxIndex]);
    // normalize cross-correlation
    xn = refSig;
    yn = &testSig[xcorrMaxIndex];
    for (j = 0; j < corrLength; j++) {
        sumX += xn[j] * xn[j];
        sumY += yn[j] * yn[j];
    }
    sumX = sumX / corrLength;
    sumY = sumY / corrLength;

    //printf("sumX:%f, sumY:%f \n", sumX, sumY);
    if (sumX != 0 && sumY != 0) {
        normxcorr = xcorr[xcorrMaxIndex] / sqrt(sumX * sumY);
    }
    
    free(xcorr);

    //printf("start to determine if similar %f! \n", normxcorr);
    if ( fabs(normxcorr) > 0.75 && fabs(normxcorr) <= 1.0) {
       return true;
    } else {
       return false;
    }
}
