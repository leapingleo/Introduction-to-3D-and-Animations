//
//  helper.cpp
//  opengl
//
//  Created by Leapingleo on 23/9/19.
//  Copyright © 2019 Leapingleo. All rights reserved.
//

#include <stdio.h>

typedef struct { float a, kx, kz, w; } Sinewave;
float gravity = -0.5;
struct Keyframe {
    float time;
    float value;
};

struct Interpolator {
    float duration;
    float currTime;
    float min;
    float max;
    float minToMaxStepSize;
        
    //number of keyframes
    int nKeyframes;
    
    //number of frames for the animation
    Keyframe keyframes[11];
};

Interpolator upperRotation = {
    1.6, //duration
    0, //current time
    -20, // min
    -90, // max
    -20, // stepSize
    10 // 10 frames
//    {
//        {0, -20},
//        {0.1, -40},
//        {0.2, -60},
//        {0.3, -90},
//        {0.4, -90},
//        {0.5, -90},
//        {0.6, -90},
//        {0.7, -90},
//        {0.8, -90},
//        {0.9, -90},
//        {1.0, -90},
//        {1.1, -90},
//        {1.2, -90},
//        {1.3, -90},
//        {1.4, -60},
//        {1.5, -40},
//        {1.6, -20},
//    }
};

Interpolator midRotation = {
    1.6, //duration
    0, //current time
    30,
    90,
    20,
    10 // 10 frames
//    {
//        {0, 30},
//        {0.1, 50},
//        {0.2, 70},
//        {0.3, 90},
//        {0.4, 90},
//        {0.5, 90},
//        {0.6, 90},
//        {0.7, 90},
//        {0.8, 90},
//        {0.9, 90},
//        {1.0, 90},
//        {1.1, 90},
//        {1.2, 90},
//        {1.3, 90},
//        {1.4, 70},
//        {1.5, 50},
//        {1.6, 30},
//    }
};

Interpolator lowerRotation = {
    1.6, //duration
    0, //current time
    -45,
    -125,
    -25,
    10
    // 10 frames
//    {
//        {0, -45},
//        {0.1, -70},
//        {0.2, -95},
//        {0.3, -125},
//        {0.4, -125},
//        {0.5, -125},
//        {0.6, -125},
//        {0.7, -125},
//        {0.8, -125},
//        {0.9, -125},
//        {1.0, -125},
//        {1.1, -125},
//        {1.2, -125},
//        {1.3, -125},
//        {1.4, -95},
//        {1.5, -70},
//        {1.6, -45},
//    }
};

Interpolator torsorRotation = {
    1.6, //duration
    0, //current time
    10, //min
    40, //max
    10, //step size
    11 // n frames
//    {
//        {0.0, 10},
//        {0.1, 20},
//        {0.2, 30},
//        {0.3, 40},
//        {0.4, 40},
//        {0.5, 40},
//        {0.6, 40},
//        {0.7, 40},
//        {0.8, 30},
//        {0.9, 20},
//        {1.0, 10},
//        {1.1, 40},
//        {1.2, 40},
//        {1.3, 40},
//        {1.4, 30},
//        {1.5, 20},
//        {1.6, 10},
//    }
};

Interpolator frontUpperRotation = {
    1.6, //duration
    0, //current time
    45, //min
    15, //max
    5, //step
    10 // 10 frames
//    {
//        {0, 45},
//        {0.1, 85},
//        {0.2, 45},
//        {0.3, 30},
//        {0.4, 15},
//        {0.5, 15},
//        {0.6, 15},
//        {0.7, 30},
//        {0.8, 45},
//        {0.9, 80},
//        {1.0, 45},
//    }
};

Interpolator frontLowerRotation = {
    1.6, //duration
    0, //current time
    -60, //min
    -159, //max
    -33, //stepsize
    10 //frames
//    {
//        {0, -60},
//        {0.1, -93},
//        {0.2, -126},
//        {0.3, -160},
//        {0.4, -160},
//        {0.5, -160},
//        {0.6, -160},
//        {0.7, -160},
//        {0.8, -160},
//        {0.9, -160},
//        {1.0, -160},
//        {1.1, -160},
//        {1.2, -160},
//        {1.3, -160},
//        {1.4, -126},
//        {1.5, -93},
//        {1.6, -60},
//    }
};


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

float lerp(float t0, float v0, float t1, float v1, float t) {
    return v0 + (t - t0) / (t1 - t0) * (v1 - v0);
}

int findInterval(Interpolator inter, float keyTimeValue) {
    for (int i = 0; i < inter.nKeyframes; i++) {
        if (keyTimeValue >= inter.keyframes[i].time && keyTimeValue <= inter.keyframes[i + 1].time) {
         //   std::cout << i << std::endl;  
            return i;
        }
    }
    return -1;
}

//int findInterval(float keys[], float key){
//  for (int i = 0; i < 5; i++){
//      if (key >= keys[i] && key <= keys[i + 1]) {
//
//          return i;
//      }
//  }
//  return -1;
//}

//float interpolator(float keys[], float t) {
//    int i = findInterval(inter, t);
//    float v = lerp(inter.keyframes[i].time, inter.keyframes[i].value, inter.keyframes[i + 1].time,
//                   inter.keyframes[i + 1].value, t);
//    return v;
//}

float interpolator(Interpolator inter, float key){
  int i = findInterval(inter, key);
  float v = lerp(inter.keyframes[i].time, inter.keyframes[i].value, inter.keyframes[i + 1].time, inter.keyframes[i + 1].value, key);
  return v;
}


Interpolator updateFrames(Interpolator inter, float t, bool specialFrames) {
    float timeStepSize = t / (inter.nKeyframes);
  //  inter.keyframs[0].value = inter.min;
    
    for (int i = 0; i < inter.nKeyframes + 1; i++) {
        inter.keyframes[i].time = i * timeStepSize;
        
        if (specialFrames) {
            if (i < 4) {
                if (i % 2 == 0) {
                    inter.keyframes[i].value = inter.min;
                } else if (i == 1) {
                    inter.keyframes[i].value = inter.min + inter.minToMaxStepSize;
                } else {
                    inter.keyframes[i].value = inter.min - inter.minToMaxStepSize;
                }
            } else if (i > 6) {
                if (i % 2 == 0) {
                    inter.keyframes[i].value = inter.min;
                } else if (i == 7) {
                    inter.keyframes[i].value = inter.min - inter.minToMaxStepSize;
                } else {
                    inter.keyframes[i].value = inter.min + inter.minToMaxStepSize;
                }
            } else {
                inter.keyframes[i].value = inter.max;
            }
        } else {
            if (i < 4){
                inter.keyframes[i].value = inter.min + inter.minToMaxStepSize * i;
            } else if (i > 7) {
                inter.keyframes[i].value = inter.keyframes[i - 1].value - inter.minToMaxStepSize;
            } else {
                inter.keyframes[i].value = inter.max;
            }
        }
     //   std::cout << "time " << inter.keyframes[i].time << " with value: " << inter.keyframes[i].value << std::endl;
    }
    return inter;
}

float getTimeOfFlight(float v){
    return 2.0 * v / -gravity;
}

