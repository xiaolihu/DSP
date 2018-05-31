 /*
  * Polyphase FIR in c implementation
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "polyphasefir.h"


/** The maximum input length is 480 shorts; ie, 10ms 48KHz data **/
#define PF_MAX_INPUT_LENGTH        480


pfir_status_t pfir_initUpFilter(pfir_upFilterState_t *ptrState, const Int16 *filterConfig)
{
    int           histLen;

    if ( !ptrState || !filterConfig )
        return PFIR_FAIL;

    ptrState->upSampleFactor =  filterConfig[0];
    ptrState->subFilterLen   =  filterConfig[1];
    ptrState->filterCoef     = &filterConfig[4];

    histLen = ptrState->subFilterLen + PF_MAX_INPUT_LENGTH;
    ptrState->overlapBuffer  =  calloc(histLen-1, sizeof(Int16));
    if ( !ptrState->overlapBuffer ) {
        return PFIR_FAIL;
    }

    return PFIR_SUCCESS;
}


pfir_status_t pfir_releaseUpFilter(pfir_upFilterState_t *ptrState)
{
    pfir_status_t status = PFIR_FAIL;

    if ( ptrState && ptrState->overlapBuffer ) {
        free(ptrState->overlapBuffer);
        ptrState->overlapBuffer = 0;
        status = PFIR_SUCCESS;
   }

   return(status);
}


pfir_status_t pfir_upFilter(pfir_upFilterState_t * __restrict __volatile__ ptrState,
                            Int16 * __restrict ptrInData,
                            Int16 * __restrict ptrOutData,
                            Uint16 bufLen, Uint16 * __restrict ptrOutCnt)
{
    Int16       *pInData;
    const Int16 *pCoef;
    Uint16       L, K;
    Uint16       i, j, k;
    long         temp, sum;

    L     = ptrState->upSampleFactor;
    K     = ptrState->subFilterLen;
    pCoef = ptrState->filterCoef;

    if ( (bufLen>PF_MAX_INPUT_LENGTH) || (bufLen<K) ) {
        *ptrOutCnt = 0;
        return PFIR_FAIL;
    }

    /** Combine the history and current input **/
    memcpy(&ptrState->overlapBuffer[K-1], ptrInData, bufLen*sizeof(Int16));

    for ( i = 0; i < bufLen; ++i ) {
        pInData = &ptrState->overlapBuffer[i];

        /** interpolation loop **/
        for ( j = 0; j < L; ++j ) {

            sum = 0;
            /** filtering loop **/
            for ( k = 0; k < K; ++k ) {
#ifdef COEF_IN_NORMAL_ORDER
                temp = pInData[K-1-k]*pCoef[L*k+j];
#else
                temp = pInData[k]*pCoef[j*K+k];
#endif
                sum += (temp>>6);
            }

            sum >>= 9;
            if ( sum > 32767 )
                sum = 32767;
            else if ( sum < -32768 )
                sum = -32768;

            ptrOutData[i*L+j] = (Int16)sum;
        }
    }

    *ptrOutCnt = bufLen * L;

    /** Store the history **/
    memcpy(&ptrState->overlapBuffer[0], &ptrInData[bufLen-K+1], (K-1)*sizeof(Int16));

    return PFIR_SUCCESS;
}


pfir_status_t pfir_initDnFilter(pfir_dnFilterState_t *ptrState, const Int16 *filterConfig)
{
    int           histLen;

    if ( !ptrState || !filterConfig )
        return PFIR_FAIL;

    ptrState->dnSampleFactor =  filterConfig[0];
    ptrState->filterLen      =  filterConfig[1];
    ptrState->filterCoef     = &filterConfig[4];

    histLen = ptrState->filterLen + PF_MAX_INPUT_LENGTH;
    ptrState->overlapBuffer  =  calloc(histLen-1, sizeof(Int16));
    if ( !ptrState->overlapBuffer ) {
        return PFIR_FAIL;
    }

    return PFIR_SUCCESS;
}


pfir_status_t pfir_releaseDnFilter(pfir_dnFilterState_t *ptrState)
{
    pfir_status_t status = PFIR_FAIL;

    if ( ptrState && ptrState->overlapBuffer ) {
        free(ptrState->overlapBuffer);
        ptrState->overlapBuffer = 0;
        status = PFIR_SUCCESS;
    }

   return(status);
}


pfir_status_t pfir_dnFilter(pfir_dnFilterState_t * __restrict __volatile__ ptrState,
                            Int16 * __restrict ptrInData,
                            Int16 * __restrict ptrOutData,
                            Uint16 bufLen, Uint16 * __restrict ptrOutCnt)
{
    Int16       *pInData;
    const Int16 *pCoef;
    Uint16       M, K;
    Uint16       i, k;
    long         temp, sum;

    M     = ptrState->dnSampleFactor;
    K     = ptrState->filterLen;
    pCoef = ptrState->filterCoef;

    if ( (bufLen>PF_MAX_INPUT_LENGTH) || (bufLen<K) ) {
        *ptrOutCnt = 0;
        return PFIR_FAIL;
    }

    /** Combine the history and current input **/
    memcpy(&ptrState->overlapBuffer[K-1], ptrInData, bufLen*sizeof(Int16));

    /** decimation loop **/
    for ( i = 0; i < bufLen; i = i+M ) {
        pInData = &ptrState->overlapBuffer[i];

        sum = 0;
        /** filtering loop **/
        for ( k = 0; k < K; ++k ) {
#ifdef COEF_IN_NORMAL_ORDER
            temp = pInData[K-1-k]*pCoef[k];
#else
            temp = pInData[k]*pCoef[k];
#endif
            sum += (temp>>6);
        }

        sum >>= 9;
        if ( sum > 32767 )
            sum = 32767;
        else if ( sum < -32768 )
            sum = -32768;

        *ptrOutData++ = (Int16)sum;
    }

    *ptrOutCnt = bufLen / M;

    /** Store the history **/
    memcpy(&ptrState->overlapBuffer[0], &ptrInData[bufLen-K+1], (K-1)*sizeof(Int16));

    return PFIR_SUCCESS;
}

Int16 * pfir_getUpFilterConfig(int intFactor)
{
   switch (intFactor)
   {
      case 2:
         return pfir_2xIntCoef;
      case 3:
         return pfir_3xIntCoef;
      case 6:
         return pfir_6xIntCoef;
      default:
         return NULL;
   }
}

Int16 * pfir_getDnFilterConfig(int decFactor)
{
   switch (decFactor)
   {
      case 2:
         return pfir_2xDecCoef;
      case 3:
         return pfir_3xDecCoef;
      case 6:
         return pfir_6xDecCoef;
      default:
         return NULL;
   }
}

