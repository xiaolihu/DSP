#include <math.h>


#define M_PI 3.14159265358979323846264338327950288


/* @brief original dft process per mathmatical formula
 *
 * Y[m] = sum { X[n] (cos(2 * PI * n * m / N) - jsin(2 * PI * n * m / N)) }
 * X[n] = Real[2 * n] + jImag[2 * n]]
 *
 * Real(Y[m]) = y[2 * m]     = x[2 * n] * cosf() + x[2 * n + 1] * sinf()
 * Imag(Y[m]) = y[2 * m + 1] = x[2 * n + 1] * cosf() - x[2 * n] * sinf()
 *
 * @param[in]   x  pointer of complex input smaples(2 * n is real part, 2 * n +1 is image) in time domain
 * @param[out]  y  pointer of dft output sequences in frequency domain
 * @papra[in]   N  size or points of dft performed
 * 
 * return none
 *
 */

//O(N^2)
void dftProcess(const float *x, float *y, size_t N)
{
    size_t m, n;

    for (m = 0; m < N; m++) {
       y[2 * m] = 0.0f;
       y[2 * m +1] = 0.0f;

       for (n = 0; n < N; n++) {
           const float c = cosf(2 * M_PI * m * n / N);
           const float s = sinf(2 * M_PI * m * n / N);

	   y[2 * m]      += x[2 * n]      * c + x[2 * n + 1] * s;
	   y[2 * m + 1]  += x[2 * n + 1]  * c - x[2 * n] * s;
       }

    }  

}

// inverse dft
void idftProcess(const float *x, float *y, size_t N)
{
    size_t m, n;

    for (m = 0; m < N; m++) {
       y[2 * m] = 0.0f;
       y[2 * m +1] = 0.0f;

       for (n = 0; n < N; n++) {
           const float c = cosf(2 * M_PI * m * n / N);
           const float s = sinf(2 * M_PI * m * n / N);

	   y[2 * m]      += x[2 * n]      * c - x[2 * n + 1] * s;
	   y[2 * m + 1]  += x[2 * n + 1]  * c + x[2 * n] * s;
       }

       y[2 * m] = y[2 * m] / N;
       y[2 * m +1] = y[2 * m + 1] / N;
    }  

}
