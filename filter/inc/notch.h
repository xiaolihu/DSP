#ifndef _NOTCH_H_
#define _NOTCH_H_

#include "iir_biquad.h"

iir_biquad_t *init_notch(void);

// fixed-point input/output signal
void notch_process(iir_biquad_t *pIIR_notch,
                   int16_t *in,
                   int16_t *out,
                   int blockSize);

#endif
