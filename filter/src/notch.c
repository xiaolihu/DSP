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

void reset_notch(iir_biquad_t *pIIR_notch)
{
    pIIR_notch->s[0] = 0.0;
    pIIR_notch->s[0] = 0.0;
}

// notch filter process with fixed-point input/output version
void notch_process(iir_biquad_t *pIIR_notch, int16_t *in, int16_t *out, int blockSize)
{
    iir_biquad_fix(pIIR_notch, in, out, blockSize);
}
