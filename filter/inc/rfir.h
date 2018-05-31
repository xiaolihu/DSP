#ifndef _RFIR_H_
#define _RFIR_H_

#define _FIXED_POINT_
#ifdef _FIXED_POINT_

typedef struct firFilter {
    int sampleCnt;
    int nTap;             // filter length
    const short *pCoeffs;
    short *overlapBuffer; // delay and work buffer, size is sampleCnt + nTap - 1
} firFilter_t;

firFilter_t* initRFirFilter(int sampleCnt, int nTap, const short *pCoeffs);

void deinitRFirFilter(firFilter_t *pFir);

/* @brief General compute FIR filter, input #sampleCnt samples to filter out
 *        desired band singals. It can be applied to LPF, HPF, BPF for single
 *        rate decimator/interpolator or FIR-EQ in direct type. y[k] = x[k] * h[k]
 * 
 * @param inBuf[in]      - point to where input samples stored, x[0], x[1], ... x[n-1]
 *        coefBuf[in]    - point to where filter coefficients stored, h[k]
 *        outBuf[out]    - point to where filtered singals put to, y[n]
 *        ntap[in]       - filter length or taps N = order + 1, even or odd up to required
 *        sampleCnt[in]  - input sample count
 *        decFactor      - if need decimation when downsampling, or no need should be 1
 *        strideStep     - if need store filtered data in this # step(offset), half-band filter etc.
 *
 * */
void computeRFir(short *inBuf,
                 const short *coefBuf,
                 short *outBuf,
                 int ntap,
                 int sampleCnt,
                 int decFactor,
                 int strideStep);

#endif 

#endif
