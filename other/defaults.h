#ifndef DEFAULTS_H
#define DEFAULTS_H

typedef unsigned int uint;


#if __cplusplus >= 201703L
#define CPP17 1
#else
#define CPP17 0
#endif

#ifndef WAVELET_MATLAB
#define WAVELET_MATLAB 0
#endif

/// iitp
///  0 - downsample EMG, 1 - upsample EEG
#ifndef UP_DOWN_S
#define UP_DOWN_S 01
#endif

/// classifier - should check
#ifndef INERTIA
#define INERTIA 0
#endif

#ifndef SHOW_MATI_WIDGETS
#define SHOW_MATI_WIDGETS 0
#endif


#ifndef SHOW_IITP_WIDGETS
#define SHOW_IITP_WIDGETS 01
#endif

#ifndef ELENA_VARIANT
#define ELENA_VARIANT 0
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#endif
