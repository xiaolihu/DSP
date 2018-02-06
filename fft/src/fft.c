// reference
// https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm
//
#include <math.h>
#include "fft.h"

static size_t reverseBits(size_t x, int n)
{
    size_t ret = 0;

    for (int i = 0; i < n; i++) {
        ret = (ret << 1) | (x & 0x1);
        x >>= 1;	
    } 
    
    return ret;
}

// N MUST be a power of 2, N-point decimation-in-time FFT
// complex input with N points
void fftProcessRadix2(float *real, float *imag, size_t N)
{
    int nStages = 0;

    for (size_t stage = N; stage > 1; stage >>= 1) {
        nStages++;
    }

    if (1 << nStages != N) {
        printf("ERROR: N = %d is not a power of 2!");
        return;
    }

    // calculate twiddle factors table for lookup later
    // w[m] = exp(- 2 * PI * m / N)
    // w[m + N/2] = -w[m}
    float *w_real = (float *) malloc((N / 2) * sizeof(float));
    float *w_imag = (float *) malloc((N / 2) * sizeof(float));
    for ( size_t i = 0; i < N / 2; i ++) {
        w_real[i] = cosf(2 * M_PI * i / N);
        w_imag[i] = sinf(2 * M_PI * i / N);
    }

    // re-order the input signals via bit-reversed addressing
    // so the output will be in normal order. 
    for (size_t i = 0; i < N; i++) {
        size_t j = reverseBits(i, nStages);
        if (j > i) {
            // swap real and imag part of input complex
	    float temp = real[i];
	    real[i] = real[j];
	    real[j] = temp;

	    temp = imag[i];
	    imag[i] = imag[j];
	    imag[j] = temp;
        }
    }

     // cooley-tukey decimation-in-time Radix-2 FFT O(N * log(N))
     // iterations through dyads, quadruples, octads and so on...
    for (size_t stage = 2; stage <= N; stage <<= 1) { // horizontally
        size_t halfSize = stage >> 1;
	size_t wStep =  N / stage; // step of twiddle factor table
	// iteration through groups of different twiddle factors
	for (size_t group = 0; group < N; group += stage) { // vertically
            // iteration within group
	    for (size_t pair = group, w = 0; pair < group + halfSize; pair++, w += wStep) {
                size_t  match = pair + halfSize;

		// two-point dft transform
		float tempRe =  real[match] * w_real[w] + imag[match] * w_imag[w];
		float tempIm = -real[match] * w_imag[w] + imag[match] * w_real[w];
		// F[match] 
                real[match] = real[pair] - tempRe;
		imag[match] = imag[pair] - tempIm;
		// F[pair]
		real[pair]  = real[pair] + tempRe;
		imag[pair]  = imag[pair] + tempIm;
	    }
	}
    }
    // cleanup
    free(w_real);
    free(w_imag);
}

void ifftProcessRadix2(float *real, float *imag, size_t N)
{
    fftProcessRadix2(image, real, N);
}
