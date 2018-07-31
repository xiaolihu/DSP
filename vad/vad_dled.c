// Author: Xiaolin Huang
// All Copy Rights Are Reserved.
// Dynamic Linear Energy-Based Voice Activity Detector

#include <limits.h>

#include "vad.h"

#define R   8 // depends on sample numbers as devisor, big enough to avoid overflow
#define K   (1 << (R - 1)) // round up mid values


static Uint16 sqrt32(Uint32 input)
{
    Uint32  i, t;
    Uint32  c = 0;

    for (i = 0x40000000; i != 0; i >>= 2) {
        t = c + i;
        c >>= 1;
        if (t <= input) {
            input -= t;
            c += i;
        }
    }
    return (Uint16)c;
}

// compute N samples root mean square energy
static uint16_t computeRMS(const int16_t *pSamples, short N)
{
    int64_t sum;
    int32_t ltemp;
    int16_t stemp;
    uint16_t sinRMS;

    short i;

    for (i = 0; i < N; i++) {
        stemp = *pSamples++;
	ltemp = (int32_t)stemp * (int32_t)stemp;
	ltemp += K; // rounding
	sum += ltemp >> R; // discard LSB, close to devide by N
    }
    //check for saturation
    if (sum > LONG_MAX) sum = LONG_MAX;
    //truncate sum to unsigned 32-bit data
    sinRMS = sqrt32((Uint32) sum);

    return sinRMS;
}

static inline uint16_t findLeftMostOneBit(uint16_t number)
{
    uint16_t cnt = 0;
    while (number > 1) {
        cnt++;
        number >>= 1;
    }

    return cnt;
}

void vadDLEDInit(vad_dled_t *pVad, int samplesPerMs)
{
    pVad->firstFrame = false;
    pVad->Emin = 0;
    pVad->EminDelta = 1;
    pVad->Emax = 0;
    pVad->EThresh = 128;
    pVad->EThreshFactor = 0;

    pVad->silenceCnt = 0;
    pVad->hangoverCnt = 0;

    pVad->samplesPerMs = samplesPerMs;

    pVad->state = VadState_Speech;
}

#define Q15_MAX 15
// true - voiced,  false - unvoiced
bool vadDLEDProcess(vad_dled_t *pVad, const int16_t *pSamples, short N)
{
    uint16_t curEnergy;
    uint16_t gapEnergy;
    uint16_t threshFactor;
    static uint16_t initialEmin = 0;

    curEnergy = computeRMS(pSamples, N);
    if (!pVad->firstFrame) {
        pVad->Emax = curEnergy; 
        pVad->Emin = curEnergy;
	initialEmin = pVad->Emin;
        pVad->firstFrame = true;
    }

    if (curEnergy > pVad->Emax) pVad->Emax = curEnergy;

    if (curEnergy < pVad->Emin) {
        if (curEnergy == 0) {
            // TODO assign INITIAL Value to Emin
            pVad->Emin = initialEmin;
        } else {
            pVad->Emin = curEnergy;
        }
    }
    // calculating threshhold value
    gapEnergy = pVad->Emax - pVad->Emin;
    pVad->EThreshFactor = findLeftMostOneBit(gapEnergy);

    if (pVad->EThreshFactor > Q15_MAX) pVad->EThreshFactor = Q15_MAX;

    threshFactor = pVad->EThreshFactor;
    // threshHold = (1 - delta) * Emax + delta * Emin
    pVad->EThresh = (pVad->Emax >> threshFactor) + (pVad->Emin >> (Q15_MAX - threshFactor));

    // determine if speech is present or not
    if (curEnergy > pVad->EThresh) {
        pVad->state = VadState_Speech;
        pVad->hangoverCnt = pVad->samplesPerMs * 100; // 10 frames
    } else {
        if (pVad->hangoverCnt < N) {
            pVad->state = VadState_Silence;
            pVad->hangoverCnt = 0;
            pVad->silenceCnt++;
        } else {
            pVad->hangoverCnt -= N;
            pVad->state = VadState_Speech;
        }
    }

    // EminDelta = EminDelta * 1.0001
    // since low energy anomalies can occur there is a prevention needed.
    pVad->EminDelta = pVad->EminDelta + (pVad->Emin >> 6); // TODO need be tuned
    pVad->Emin += pVad->EminDelta; 

    return (pVad->state == VadState_Speech);
}
