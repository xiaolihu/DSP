#ifndef _IIR_BIQUAD_H_
#define _IIR_BIQUAD_H_

#define _FLOAT_POINT_
// float point version
#ifdef _FLOAT_POINT_
typedef struct iir_biquad {
    float b[3];
    float a[2];
    float s[2]; // biquad Transposed Direct II
} iir_biquad_t;

void iir_biquad(float *restrict x,
                const float *b,
                const float *a,
                float *restrict s,
                float *restrict y,
                int n);

// input and output using same block memory
void iir_biquad_inplace(float *restrict x,
                        const float *b,
                        const float *a,
                        float *restrict s,
                        int n);

#endif

#endif
