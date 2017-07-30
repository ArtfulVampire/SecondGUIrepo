#ifndef DEFAULTS_H
#define DEFAULTS_H

typedef unsigned int uint;

/// 0 - usual, 1 - mine, with normalization
#ifndef COHERENCY_TYPE
#define COHERENCY_TYPE 01
#endif

#ifndef WAVELET_MATLAB
#define WAVELET_MATLAB 01
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
#define SHOW_IITP_WIDGETS 0
#endif

#ifndef ELENA_VARIANT
#define ELENA_VARIANT 0
#endif

#endif
