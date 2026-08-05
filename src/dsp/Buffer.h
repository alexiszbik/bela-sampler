
#pragma once

#include "Memory.h"

class Buffer {
public:
    Buffer() {
        initBuf(&data, 0.0f);
    }
    
    Buffer(float initValue) {
        initBuf(&data, initValue);
    }
    
    ~Buffer() {
        free(data);
    }
    
    operator float*() { return data; }
private:
    float* data;
    
};

