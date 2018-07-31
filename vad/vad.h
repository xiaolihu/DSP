#ifndef VAD_H
#define VAD_H

#include <stdbool.h>

#include "iir_biquad.h"

typedef short Int16;
typedef unsigned short Uint16;
typedef int Int32;
typedef unsigned int Uint32;

/* Use the fixed frame length for VAD algorithm, so the algorithm won't
 * change much for 8kHz vs 16kHz
 */
#define DATA_FRAME_LENGTH                 40

/* x = 1.5sec * 1000ms/sec * 8 samples/1 ms
 * samples => +1dB every x samples */
#define VAD_NOISEFLOOR_CNT_INIT_MS      1500
#define VAD_SIGNALMAX_CNT_INIT_MS       1500

#define VAD_NOISE_TH_BASE               1000
#define VAD_SIGNAL_MAX_INIT            -8000
#define VAD_DEFAULT_MOH_THRESHOLD       4000
#define VAD_NOISE_TH_MIN                 100
#define VAD_CNG_NOISE_FLOOR_COUNT       4000

/* the following two values follow computeSTA() limitation
 * to output values greater than or equal to 511
 * Changes to computeSTA() could have an impact on these
 */
#define VAD_STA_INIT                     511  //min possible output of computeSTA()
#define VAD_NOISE_FLOOR_INIT           -6627  //log10_32(511)

/* High Pass Filter for getting rid of background noise from
 * input signal before energy calculations */
/* 2nd order Butterworth highpass : (2, 240/4000, 'high') for 8kHz, in Q14 */
#define vhpfB0                         14339
#define vhpfB1                        -28678
#define vhpfB2                         14339
#define vhpfA1                        -28422
#define vhpfA2                         12550

/* 2nd order Butterworth highpass : (2, 240/8000, 'high') for 16kHz, in Q14 */
#define vhpfB0_w                       15328
#define vhpfB1_w                      -30655
#define vhpfB2_w                       15328
#define vhpfA1_w                      -30587
#define vhpfA2_w                       14340


/* Number of frames of silence before we declare silence period */
#define VAD_HANGOVER_CNT_INIT_MS        2000
#define VAD_HANGOVER_CNT_INIT_RX_MS      500        //rx hangover is smaller so jB can adapt down

/* ***************  */
#define MIN_DELTA_NF                       2        /* min change in nf to trigger sid send */
#define MIN_NF_TO_SEND                    75        /* min nf level to trigger sid send: set computeNFSTA() limit cycle min */
#define DEFAULT_NF_LEVEL       ((Int16) -68)
#define DEFAULT_NF_STA        ((Uint32) 339) /* 10^((DEFAULT_NF_LEVEL+93)/10) */
#define DEFAULT_NF_TIMEOUT     ((Int32)4000)

#define SID_STEP_SIZE                    (5)


typedef enum {
  VadState_Silence = 0,
  VadState_Speech,
  VadState_Unknown
} dsp_VadState;

typedef struct _vad {
    bool   enabled;

    int       sampRateShift;       //<--- 3: 8kHz, 4: 16kHz

    /**
     Saved STA between input frames
    **/
    Uint32    sta;

    /**
    state == 1 if VOICE
    state == 0 if SILENCE
    **/
    dsp_VadState state;

    /**
    Countdown of consecutive frames before we declare silence
    **/
    Uint16    hangoverCnt;
    Uint16    hangoverCntInit;

    /**
    Threshold above which a signal is considered to be speech
    **/
    Int16     noiseTH;

    /**
    Countdown after which the noise floor is
    incremented by 1dB
    **/
    Uint16    noiseFloorCnt;

    /**
    Noise floor in dB
    **/
    Int16     noiseFloor;

    /**
    Processed Music on hold threshold -1 * 100 * MOHThreshold
    **/
    Int16     mohThreshold;

    /**
    STARise == 1 if sta is rising
    STARise == 0 if sta is falling
    **/
    Uint16    STARise;

    Int16     cngNoiseFloor;

    Uint32    cngNFsta;
    Int32     cngNFcnt;

    /**
    High Pass Filter for input signal
    **/
    iir_biquad_t   bq2;

} dsp_vad;


typedef struct _vad_dled {
    bool firstFrame;
    uint16_t Emin;
    uint16_t EminDelta;
    uint16_t Emax;
    uint16_t EThresh;
    uint16_t EThreshFactor;

    int32_t silenceCnt;
    int32_t hangoverCnt;
    int32_t samplesPerMs;

    dsp_VadState state;
} vad_dled_t;

typedef bool VAD_State;

//void vadInit(dsp_vad *vad, bool cnEnable);
void vadInit(dsp_vad *vad, int samplesPerMs);
VAD_State vadProcess(dsp_vad *vad, Uint16 length, Int16 *data, Uint16 dataStartIdx, Uint16 dataBufferLength);
Int16 noiseLvl(dsp_vad *vad);
void computeNFSTA(dsp_vad *vad, Int16 *pdata, Uint16 length);

#endif

