/*
 * MATLAB Compiler: 5.1 (R2014a)
 * Date: Sat Oct  1 14:33:00 2016
 * Arguments: "-B" "macro_default" "-W" "lib:libcwt_r" "-T" "link:lib" "-d"
 * "/media/Files/Matlab/Projects/libcwt_r/for_testing" "-v"
 * "/media/Files/Matlab/Projects/CWT.m"
 * "class{Class1:/media/Files/Matlab/Projects/CWT.m}" 
 */

#include <stdio.h>
#define EXPORTING_libcwt_r 1
#include "libcwt_r.h"

static HMCRINSTANCE _mcr_inst = NULL;


#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libcwt_r_C_API
#define LIB_libcwt_r_C_API /* No special import/export declaration */
#endif

LIB_libcwt_r_C_API 
bool MW_CALL_CONV libcwt_rInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
    {
        mclCtfStream ctfStream = 
            mclGetEmbeddedCtfStream((void *)(libcwt_rInitializeWithHandlers));
        if (ctfStream) {
            bResult = mclInitializeComponentInstanceEmbedded(   &_mcr_inst,
                                                                error_handler, 
                                                                print_handler,
                                                                ctfStream);
            mclDestroyStream(ctfStream);
        } else {
            bResult = 0;
        }
    }  
    if (!bResult)
    return false;
  return true;
}

LIB_libcwt_r_C_API 
bool MW_CALL_CONV libcwt_rInitialize(void)
{
  return libcwt_rInitializeWithHandlers(mclDefaultErrorHandler, mclDefaultPrintHandler);
}

LIB_libcwt_r_C_API 
void MW_CALL_CONV libcwt_rTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_libcwt_r_C_API 
void MW_CALL_CONV libcwt_rPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(&stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_libcwt_r_C_API 
bool MW_CALL_CONV mlxCWT(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "CWT", nlhs, plhs, nrhs, prhs);
}

LIB_libcwt_r_C_API 
bool MW_CALL_CONV mlfCWT(int nargout, mxArray** C, mxArray* inData, mxArray* srate)
{
  return mclMlfFeval(_mcr_inst, "CWT", nargout, 1, 2, C, inData, srate);
}

