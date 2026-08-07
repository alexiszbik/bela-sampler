
#pragma once

#include "DelayWrite.h"
#include <math.h>

class DelayRead {
    
public :
    
    DelayRead() {
    }
    
    float x_sr;         /* samples per msec */
    
    void init(float sampleRate) {
        x_sr = sampleRate;
    }

    void process(float *times, float *out, DelayWrite *writerBlock, int n)
    {
        DelayWrite::t_delwritectl *ctl = &(writerBlock->x_cspace);
        
        int nsamps = ctl->c_n;
        float limit = nsamps - n;
        
        float *vp = ctl->c_vec;
        float *wp = vp + ctl->c_phase;
        
        float zerodel = 0;
        float fn = static_cast<float>(n - 1);
        
        for (int i = 0; i < n; i++) {
            
            float delsamps = (x_sr * times[i] - zerodel);
            
            if (!(delsamps >= 1.00001f)) {    /* too small or NAN */
                delsamps = 1.00001f;
            }
            if (delsamps > limit) {       /* too big */
                delsamps = limit;
            }
            
            delsamps += fn;
            fn -= 1.f;
            
            int idelsamps = static_cast<int>(delsamps);
            float frac = delsamps - static_cast<float>(idelsamps);
            
            float* bp = wp - idelsamps;
            if (bp < vp + XTRASAMPS) {
                bp += nsamps;
            }
            
            float d = bp[-3];
            float c = bp[-2];
            float b = bp[-1];
            float a = bp[0];
            float cminusb = c - b;
            
            out[i] = b + frac * (cminusb - 0.1666667f * (1.f - frac) * ((d - a - 3.0f * cminusb) * frac + (d + 2.0f * a - 3.0f * b)));
        }
    }
};

