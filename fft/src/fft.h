#ifndef _FFT_H_
#define _FFT_H_


void fftProcessRadix2(float *real, float *imag, size_t N);
void ifftProcessRadix2(float *real, float *imag, size_t N);

#define M_PI 3.14159265358979323846264338327950288

#endif
