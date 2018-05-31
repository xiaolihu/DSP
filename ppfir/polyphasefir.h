#ifndef __PFIR_H__
#define __PFIR_H__


#ifdef __cplusplus
extern "C" {
#endif



typedef enum {PFIR_SUCCESS = 0, PFIR_FAIL, PFIR_BAD_CFG} pfir_status_t;

typedef signed short Int16;
typedef unsigned short Uint16;

typedef volatile struct pfir_upFilterState_ {
   Int16 upSampleFactor;
   Int16 subFilterLen;
   const Int16 *filterCoef;
   Int16 *overlapBuffer;
}  pfir_upFilterState_t;

typedef volatile struct pfir_dnFilterState_ {
   Int16 dnSampleFactor;
   Int16 filterLen;
   Int16 inputStrideOffset;
   const Int16 *filterCoef;
   Int16 *overlapBuffer;
}  pfir_dnFilterState_t;



pfir_status_t pfir_initUpFilter(pfir_upFilterState_t *ptrState, const Int16 *filterConfig) \
                                __attribute__((cold, warn_unused_result));

pfir_status_t pfir_initDnFilter(pfir_dnFilterState_t *ptrState, const Int16 *filterConfig) \
                                __attribute__((cold, warn_unused_result));

pfir_status_t pfir_releaseUpFilter(pfir_upFilterState_t *ptrState) \
                                   __attribute__((cold, warn_unused_result));

pfir_status_t pfir_releaseDnFilter(pfir_dnFilterState_t *ptrState) \
                                   __attribute__((cold, warn_unused_result));

pfir_status_t pfir_upFilter(pfir_upFilterState_t * __restrict ptrState,    \
                            Int16 * __restrict ptrInData,                  \
                            Int16 * __restrict ptrOutData,                 \
                            Uint16 bufLen, Uint16 * __restrict ptrOutCnt)  \
                            __attribute__((hot));

pfir_status_t pfir_dnFilter(pfir_dnFilterState_t * __restrict ptrState,    \
                            Int16 * __restrict ptrInData,                  \
                            Int16 * __restrict ptrOutData,                 \
                            Uint16 bufLen, Uint16 * __restrict ptrOutCnt)  \
                            __attribute__((hot));

Int16 *pfir_getUpFilterConfig(int intFactor);
Int16 *pfir_getDnFilterConfig(int decFactor);

extern Int16 pfir_2xIntCoef[];
extern Int16 pfir_2xDecCoef[];
extern Int16 pfir_3xIntCoef[];
extern Int16 pfir_3xDecCoef[];
extern Int16 pfir_6xIntCoef[];
extern Int16 pfir_6xDecCoef[];


#ifdef __cplusplus
}
#endif


#endif
                    
