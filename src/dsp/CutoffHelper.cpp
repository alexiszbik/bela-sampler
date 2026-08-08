#include "CutoffHelper.h"

#include <cmath>
//#include <Bela.h>

inline float fast_mtof(float m)
{
    constexpr float inv12 = 0.0833333333333f;
    constexpr float a440  = 440.f;
    return a440 * exp2f((m - 69.f) * inv12);
}

static constexpr float minMidiCutoff = 14;
static constexpr float midiCutoffRange = 120;

float cutoffRatioToHz(float ratio) {

	//rt_printf("%.1f freq %.1f freq  \n", fast_mtof(minMidiCutoff), fast_mtof(midiCutoffRange + minMidiCutoff));

	return fast_mtof(ratio * midiCutoffRange + minMidiCutoff);
}
