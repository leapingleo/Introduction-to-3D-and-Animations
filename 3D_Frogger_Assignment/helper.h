//
//  helper.cpp
//  opengl
//
//  Created by Leapingleo on 23/9/19.
//  Copyright © 2019 Leapingleo. All rights reserved.
//

#include <stdio.h>

typedef struct { float a, kx, kz, w; } Sinewave;

float magnitude(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z);
}

float deg2rad(float deg) {
    return M_PI * deg / 180;
}

float rad2deg(float rad) {
    return 180 * rad / M_PI;
}

float calculateSin(float A, float x, float t) {
    float waveLen = 2;
    return A * sin(2 * M_PI / waveLen * x + M_PI / 4 * t);
}

float calcSineWave(Sinewave sw, float x, float z, float t) {
    return sw.a * sinf(sw.kx * x + sw.kz * z + sw.w * t);
}

bool isUnderwater(Sinewave sw, float x, float y, float z, float t) {
    return y <= calcSineWave(sw, x, 0, t);
}
