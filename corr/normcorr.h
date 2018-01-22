#ifndef _NORM_CORR_C_
#define _NORM_CORR_C_

#define SAMPLE_LEN_IN_BYTES sizeof(short)

#define MAX_FS 48000
#define MAX_SIG_BUFLEN_IN_SAMPLES (5 * MAX_FS) //5 seconds samples
#define MAX_SIG_BUFLEN_IN_BYTES (5 * MAX_FS * SAMPLE_LEN_IN_BYTES)

#define SYSTEM_CLOCK_RATE 16000
#define SYSTEM_PCM_LEN (160 * SAMPLE_LEN_IN_BYTES)

#define SHIFT_IN_SAMPLES (500 * SYSTEM_CLOCK_RATE / 1000)

//#define IGNORE_IN_SAMPLES (250 * SYSTEM_CLOCK_RATE / 1000)
#define IGNORE_IN_SAMPLES 0
#define CORR_LEN_IN_SAMPLES (2 * SYSTEM_CLOCK_RATE)

#define WIN_LEN (MAX_SIG_BUFLEN_IN_SAMPLES - CORR_LEN_IN_SAMPLES)

bool normcorr(short *refSig, short *testSig, uint32_t corrLength);

#endif
