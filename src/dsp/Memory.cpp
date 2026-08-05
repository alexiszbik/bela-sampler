
#include "Memory.h"

void Memory::copy(float* _in, float* _out, const size_t frameCount) {
    memcpy(_out, _in, sizeof(float)*frameCount);
}
