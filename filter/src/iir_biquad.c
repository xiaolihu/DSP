// 2018-4-16
// iir_biquad.c

#include "iir_biquad.h"

#ifdef _FLOAT_POINT_

/*
 * @brief IIR biquad implementation DIRECT Form II Transposed Structure
 *
 * @param x[in]  -  pointer to input data to be filtered x[n]
 *        b[in]  -  pointer to numerators of cofficients b0 ~ b2, zero point
 *        a[in]  -  pointer to denumerators of cofficients a1 ~ a2, pole point
 *        s[inout]  - pointer to intermediate data of irr filter
 *        y[out] -  pointer to output buffer after filtered y[n]
 *        n[in]  -  number of signal smaples to be filtered
 *
 * */
void iir_biquad(float *restrict x, const float *b, const float *a, float *restrict s,  float *restrict y, int n)
{
    int i;
    const float b0 = b[0];
    const float b1 = b[1];
    const float b2 = b[2];
    const float a1 = a[0];
    const float a2 = a[1];
    float s0 = s[0];
    float s1 = s[1];

    for (i = 0; i < n; i++) {
        y[i] = b0 * x[i] + s0;
        s0 = x[i] * b1 - y[i] * a1 + s1;
        s1 = x[i] * b2 - y[i] * a2;
    }

    s[0] = s0;
    s[1] = s1;
}

// store filtered to same buffer with input original data, saving memory usage
void iir_biquad_inplace(float *restrict x, const float *b, const float *a, float *restrict s, int n)
{
    int i;
    const float b0 = b[0];
    const float b1 = b[1];
    const float b2 = b[2];
    const float a1 = a[0];
    const float a2 = a[1];
    float s0 = s[0];
    float s1 = s[1];
    float y_i;

    for (i = 0; i < n; i++) {
        y_i = b0 * x[i] + s0;
        s0 = x[i] * b1 - y_i * a1 + s1;
        s1 = x[i] * b2 - y_i * a2;
        x[i] = y_i; // store filtered data
    }

    s[0] = s0;
    s[1] = s1;
}

#endif


// saturation check
static inline int16_t _sat16(int32_t a)
{
    static const int32_t MAX_INT16 = 0x00007FFFLL;
    static const int32_t MIN_INT16 = 0xFFFF8000LL;
    if ( a > MAX_INT16 ) {
        return MAX_INT16;
    }
    if ( a < MIN_INT16 ) {
        return MIN_INT16;
    }
    return (int16_t)a;
}

// notch filter process with fixed-point input/output version
void iir_biquad_fix(iir_biquad_t *pIIRBiquad, int16_t *in, int16_t *out, int blockSize)
{
    int i;
    float y = 0;

    const float b0 = pIIRBiquad->b[0];
    const float b1 = pIIRBiquad->b[1];
    const float b2 = pIIRBiquad->b[2];
    const float a1 = pIIRBiquad->a[0];
    const float a2 = pIIRBiquad->a[1];
    float s0 = pIIRBiquad->s[0];
    float s1 = pIIRBiquad->s[1];

    for (i = 0; i < blockSize; i++) {
        float input = (float)in[i] * (float)0.000030517578125; //  * 1/32768
        int32_t output;
        {
            // direct form II transpose
            y = input * b0  + s0;
            s0 = input * b1 - y * a1 + s1;
            s1 = input * b2 - y * a2;
        }
        output = y * 32768.0;
        out[i] = _sat16(output);
    }

    pIIRBiquad->s[0] = s0;
    pIIRBiquad->s[1] = s1;
}
