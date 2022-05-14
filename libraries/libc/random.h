#pragma once

#include <types.h>

namespace LibC {
    class random {
    public:
        static int next(uint32_t max = 32767);
        static int next(uint32_t min, uint32_t max);             
        static void setSeed(uint32_t seed);
    };
}