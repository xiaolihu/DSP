#include <stdlib.h>
#include <stdint.h>

#include "notch.h"

// 2nd-order notch filter intializing...
iir_biquad_t *init_notch(void)
{
    iir_biquad_t *pIIR_notch = calloc(1, sizeof (iir_biquad_t));

    // Q = 33, fc = 840 Hz, Ab = -3dB
    pIIR_notch->a[0] = -1.882782830476468;
    pIIR_notch->a[1] = 0.990077124552211;
    pIIR_notch->b[0] = 0.995038562276106;
    pIIR_notch->b[1] = -1.882782830476468;
    pIIR_notch->b[2] = 0.995038562276106;

    pIIR_notch->s[0] = 0.0;
    pIIR_notch->s[0] = 0.0;

    return pIIR_notch;
}

// saturating check
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
void notch_process(iir_biquad_t *pIIR_notch, int16_t *in, int16_t *out, int blockSize)
{
    int i;
    float y = 0;

    const float b0 = pIIR_notch->b[0];
    const float b1 = pIIR_notch->b[1];
    const float b2 = pIIR_notch->b[2];
    const float a1 = pIIR_notch->a[0];
    const float a2 = pIIR_notch->a[1];
    float s0 = pIIR_notch->s[0];
    float s1 = pIIR_notch->s[1];

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

    pIIR_notch->s[0] = s0;
    pIIR_notch->s[1] = s1;
}
