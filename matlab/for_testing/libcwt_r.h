/*
 * MATLAB Compiler: 5.1 (R2014a)
 * Date: Sat Oct  1 14:33:00 2016
 * Arguments: "-B" "macro_default" "-W" "lib:libcwt_r" "-T" "link:lib" "-d"
 * "/media/Files/Matlab/Projects/libcwt_r/for_testing" "-v"
 * "/media/Files/Matlab/Projects/CWT.m"
 * "class{Class1:/media/Files/Matlab/Projects/CWT.m}" 
 */

#ifndef __libcwt_r_h
#define __libcwt_r_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_libcwt_r
#define PUBLIC_libcwt_r_C_API __global
#else
#define PUBLIC_libcwt_r_C_API /* No import statement needed. */
#endif

#define LIB_libcwt_r_C_API PUBLIC_libcwt_r_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_libcwt_r
#define PUBLIC_libcwt_r_C_API __declspec(dllexport)
#else
#define PUBLIC_libcwt_r_C_API __declspec(dllimport)
#endif

#define LIB_libcwt_r_C_API PUBLIC_libcwt_r_C_API


#else

#define LIB_libcwt_r_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libcwt_r_C_API 
#define LIB_libcwt_r_C_API /* No special import/export declaration */
#endif

extern LIB_libcwt_r_C_API 
bool MW_CALL_CONV libcwt_rInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libcwt_r_C_API 
bool MW_CALL_CONV libcwt_rInitialize(void);

extern LIB_libcwt_r_C_API 
void MW_CALL_CONV libcwt_rTerminate(void);



extern LIB_libcwt_r_C_API 
void MW_CALL_CONV libcwt_rPrintStackTrace(void);

extern LIB_libcwt_r_C_API 
bool MW_CALL_CONV mlxCWT(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);



extern LIB_libcwt_r_C_API bool MW_CALL_CONV mlfCWT(int nargout, mxArray** C, mxArray* inData, mxArray* srate);

#ifdef __cplusplus
}
#endif
#endif
