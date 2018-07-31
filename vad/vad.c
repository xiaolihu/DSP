#include <math.h>
#include "vad.h"

static void bq2Init(iir_biquad_t *pBq2, int samplesPerMs)
{
    if (samplesPerMs == 8) {
        pBq2->a[0] = -1.93338022587993;
        pBq2->a[1] = 0.935528904979179;
        pBq2->b[0] = 0.967227282714777;
        pBq2->b[1] = -1.93445456542955;
        pBq2->b[2] = 0.967227282714777;
    } else if (samplesPerMs == 16){ 
        pBq2->a[0] = -1.96668138526349;
        pBq2->a[1] = 0.96722742815186;
        pBq2->b[0] = 0.983477203353836;
        pBq2->b[1] = -1.96695440670767;
        pBq2->b[2] = 0.983477203353836;
    } else {
        pBq2->a[0] = 0.0;
        pBq2->a[1] = 0.0;
        pBq2->b[0] = 1.0;
        pBq2->b[1] = 0.0;
        pBq2->b[2] = 0.0;
    }

    pBq2->s[0] = 0.0;
    pBq2->s[1] = 0.0;
}

void vadInit(dsp_vad *vad, int samplesPerMs) {
    vad->enabled         = 1;
    vad->sampRateShift   = (samplesPerMs>>3) + 2;    // base 8 = shift 1
    vad->sta             = VAD_STA_INIT;
    vad->noiseTH         = VAD_NOISE_TH_BASE;
    vad->state           = VadState_Silence;         // initial state = silence
    vad->noiseFloorCnt   = (VAD_NOISEFLOOR_CNT_INIT_MS<<vad->sampRateShift);
    vad->noiseFloor      = VAD_NOISE_FLOOR_INIT;
    vad->hangoverCnt     = 0;                        // for init state=silence
    vad->hangoverCntInit = (VAD_HANGOVER_CNT_INIT_RX_MS<<vad->sampRateShift);
    vad->STARise         = 1;
    vad->mohThreshold    = VAD_DEFAULT_MOH_THRESHOLD;

    bq2Init(&vad->bq2, samplesPerMs);
}


/** The input values should be bigger than 1. **/
static void log10_32(Uint16 length_m1, Uint32* inputPtr, Int16* outputPtr)
{
    double temp;
    Uint16 i;

    for (i = 0; i < (length_m1+1); i++) {
        temp = ( 1000.0f * log10((double)inputPtr[i]) ) - 9333.0f;
        outputPtr[i] = (Int16)temp;
    }
}

/**
computes the "short term average (STA)" energy level of the block
Note: changes to this function could require an update to VAD_STA_INIT and VAD_NOISE_FLOOR_INIT
**/
static Uint32 computeSTA(dsp_vad *vad, Int16 *pdata, Uint16 length, Uint32 *minSta) {
    Uint16  i;
    Int32   acc0, acc1;
    Uint32  maxSta;

    *minSta = vad->sta;
    maxSta  = vad->sta;

    for (i = 0; i < length; i++)
    {
        /**
        q.15 * q.15 = q.30
        **/
        acc1  = (Int32)pdata[i] * (Int32)pdata[i];

        if ( vad->STARise )
        {
            acc0  = -1 * (Int32)(vad->sta >> 6);
            acc1  = acc1 >> 5;
        }
        else
        {
            acc0  = -1 * (Int32)(vad->sta >> 9);
            acc1  = acc1 >> 8;
        }

        acc0 += acc1;
        vad->STARise = ( 0 >= acc0 ) ? 0 : 1;
        vad->sta += acc0;

        if ( vad->sta > maxSta )
        {
            maxSta = vad->sta;
        }
        else if ( vad->sta < *minSta )
        {
            *minSta = vad->sta;
        }
    }

    return maxSta;
}

void computeNFE(dsp_vad *vad, Int16 minStal, Uint16 length)
{
    if ( minStal <= vad->noiseFloor )
    { // reset noise floor value
        vad->noiseFloor = minStal;
        vad->noiseFloorCnt = (VAD_NOISEFLOOR_CNT_INIT_MS<<vad->sampRateShift);
        vad->noiseTH = -1 * ((vad->noiseFloor + vad->mohThreshold) >> 1);

        if ( vad->noiseTH < 0 )
            vad->noiseTH = 0;

        if ( vad->noiseTH > 1500 )
            vad->noiseTH = 1500;
    }
    else
    {
        if ( vad->noiseFloorCnt < length )
        {
            vad->noiseFloor += 100;
            vad->noiseFloorCnt = ((VAD_NOISEFLOOR_CNT_INIT_MS<<vad->sampRateShift) + vad->noiseFloorCnt - length);
            vad->noiseTH = -1 * ((vad->noiseFloor + vad->mohThreshold) >> 1);

            if ( vad->noiseTH < 0 )
                vad->noiseTH = 0;

            if ( vad->noiseTH > 1500 )
                vad->noiseTH = 1500;
        }
        else
        {
            vad->noiseFloorCnt -= length;
        }
    }
}


void computeNFSTA(  dsp_vad *vad,
                    Int16 *pdata,
                    Uint16 length   )
{
    Uint16  i;
    Int32   acc0,acc1;
    Uint32  minSta;
    Int16   NFstal;

    vad->cngNFcnt -= length;

    if ( vad->cngNFcnt <= 0 )
    {
        vad->cngNFcnt = DEFAULT_NF_TIMEOUT;
        vad->cngNoiseFloor += 100;
    }

    minSta = vad->cngNFsta;

    for (i = 0; i < length; i++)
    {
        /**
         q.15 * q.15 = q.30
        **/
        acc1  = (Int32)pdata[i] * (Int32)pdata[i];

        acc0  = -1 * (Int32)(vad->cngNFsta >> 6);
        acc1  = acc1 >> 5;

        acc0 += acc1;
        vad->cngNFsta += acc0;

        if ( vad->cngNFsta < minSta )
        {
            minSta = vad->cngNFsta;
        }
    }

    if ( minSta == 0 )
    {
        minSta = 1;
    }

    log10_32(0, &minSta, &NFstal);

    if ( NFstal < vad->cngNoiseFloor )
    {
        vad->cngNoiseFloor = NFstal;
        vad->cngNFcnt = DEFAULT_NF_TIMEOUT;
    }
}

/**
 noiseLvl function returns the noise floor measured
**/
Int16 noiseLvl( dsp_vad *vad) {
    Int16 ret;
    ret = (vad->cngNoiseFloor/100);
    return ret;
}

/** The assumption: 5ms data block is maximum **/
static bool vadSubProcess(dsp_vad *vad, Int16 *data, Uint16 length)
{
    bool SpeechDetected;
    bool FrameSpeechFlag;
    Int16   tmpData[80];
    Uint32  sta[2]; ///< {min, max}
    Int16   stal[2]; ///< {min, max}

    SpeechDetected  = 1;
    FrameSpeechFlag = 0;

#if 0   // No CN
    if (vad->cnEnabled == TRUE)
    {
        /**
         Filter out noise content that we can't reproduce
        **/
        bqProcess(&vad->cnbq, data, tmpData, length);

        /**
        Estimate Noise Level
        **/
        computeNFSTA(vad, tmpData, length);
    }
#endif

    /**
    Filter out low frequency background noise
    **/
    iir_biquad_fix(&vad->bq2, data, tmpData, length);

    /**
    Compute the average energy level
    **/
    sta[1] = computeSTA(vad, tmpData, length, &sta[0]);

    /**
    Transform sta levels to logarithmic scale
    **/
    if (sta[0]<=0)
        sta[0]=1;
    if (sta[1]<=0)
        sta[1]=1;

    log10_32(1, sta, stal);

    /**
    Compute Noise Floor Estimates
    **/
    computeNFE(vad, stal[0], length);

    /**
    Determine if speech is present or not
    **/
    if (stal[1] > (vad->noiseFloor + vad->noiseTH))
    {
        FrameSpeechFlag = 1;
    }

    if ( FrameSpeechFlag == 0 )
    {
        if ( vad->hangoverCnt < length )
        {
            SpeechDetected  = 0;
            vad->hangoverCnt = 0;
            vad->state = VadState_Silence;
        }
        else
        {
            vad->hangoverCnt -= length;
        }
    }
    else
    {
        vad->hangoverCnt = vad->hangoverCntInit;
        vad->state = VadState_Speech;
    }

    return SpeechDetected;
}


VAD_State vadProcess(dsp_vad *vad,
                     Uint16 length,
                     Int16 *data,
                     Uint16 dataStartIdx,
                     Uint16 dataBufferLength)
{
    Uint16      idx;
    Uint16      step;
    Uint16      vadFrameSamples = DATA_FRAME_LENGTH;
    bool        ret = VadState_Silence;


    if ( vad->enabled == 1 )
    {
        /**
        Cut up the frame into 5ms chunks for processing purposes
        **/
        for (idx = dataStartIdx; length > 0; length -= step)
        {
            step = (length < vadFrameSamples) ? length : vadFrameSamples;

            if ( (idx + step) > dataBufferLength )
                step = dataBufferLength - idx;

            ret |= vadSubProcess(vad, &data[idx], step);

            idx += step;
        }
    }
    else
    {
        ret = VadState_Speech;
    }

    return ret;
}

