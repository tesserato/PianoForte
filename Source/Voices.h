#pragma once
#include <JuceHeader.h>
#include <future>
#include <random>
#include <functional> /*lambda functions*/
#include <cmath>
#include "core/session/onnxruntime_cxx_api.h"
#include "pocketfft_hdronly.h"

const static std::vector<std::vector<float>> FREQUENCIES{
  {3.93,5.85,11.95,12.99,14.03,15.15,21.89,23.01,33.28,34.64}, // ok 01
  {3.93,5.85,11.95,12.99,14.03,15.15,21.89,23.01,33.28,34.64}, // ok 02
  {3.93,5.85,11.95,12.99,14.03,15.15,21.89,23.01,33.28,34.64}, // ok 03
  {2.99,4.96,7.0,9.99,12.09,13.11,14.13,15.21,22.85,32.39},    // ok 04 2 strings
  {2.99,4.96,7.0,9.99,12.09,13.11,14.13,15.21,22.85,32.39},    // ok 05
  {2.99,4.96,7.0,9.99,12.09,13.11,14.13,15.21,22.85,32.39},    // ok 06
  {2.99,4.96,7.0,9.99,12.09,13.11,14.13,15.21,22.85,32.39},    // ok 07
  {2.99,4.96,7.0,9.99,12.09,13.11,14.13,15.21,22.85,32.39},    // ok 08
  {2.99,4.96,7.0,9.99,12.09,13.11,14.13,15.21,22.85,32.39},  // ok 09
  {2.99,4.96,7.0,9.99,12.09,13.11,14.13,15.21,22.85,32.39},  // ok 10
  {2.99,4.96,7.0,9.99,12.09,13.11,14.13,15.21,22.85,32.39},  // ok 11
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 12 3 strings
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 13
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 14
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 15
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 16
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 17
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 18
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 19
  {1.99,3.0,4.01,5.02,6.03,10.08,14.19,15.24,22.72,23.8},      // ok 20
  {2.0,3.0,4.0,5.01,6.03,9.06,10.08,14.21,16.31,19.5},         // ok 21
  {2.0,3.0,4.0,5.01,6.03,9.06,10.08,14.21,16.31,19.5},         // ok 22
  {2.0,3.0,4.0,5.01,6.03,9.06,10.08,14.21,16.31,19.5},         // ok 23
  {2.0,3.0,4.0,5.01,6.03,9.06,10.08,14.21,16.31,19.5},         // ok 24
  {2.0,3.01,4.01,5.01,6.01,7.04,10.06,11.09,12.11,17.1},       // ok 25
  {2.0,3.01,4.01,5.01,6.01,7.04,10.06,11.09,12.11,17.1},       // ok 26
  {2.0,3.01,4.01,5.01,6.01,7.04,10.06,11.09,12.11,17.1},       // ok 27
  {2.0,3.01,4.01,5.01,6.01,7.04,10.06,11.09,12.11,17.1},       // ok 28
  {2.0,3.01,4.01,5.01,6.01,7.04,10.06,11.09,12.11,17.1},       // ok 29
  {2.0,3.01,4.01,5.01,6.01,7.04,10.06,11.09,12.11,17.1},       // ok 30
  {2.0,3.01,4.01,5.01,6.01,7.04,10.06,11.09,12.11,17.1},       // ok 31
  {2.0,3.01,4.01,5.01,6.01,7.04,10.06,11.09,12.11,17.1},       // ok 32
  {2.01,3.02,5.03,6.04,7.05,9.09,10.11,11.14,12.17,15.32},     // ok 33
  {2.01,3.02,5.03,6.04,7.05,9.09,10.11,11.14,12.17,15.32},     // ok 34
  {2.0,3.02,4.02,5.03,7.07,8.09,9.11,10.14,12.22,13.26},       // ok 35
  {2.01,3.01,4.01,5.02,6.04,7.07,8.09,10.12,13.25,15.38},      // ok 36
  {2.01,3.02,4.02,5.03,6.05,7.07,8.10,10.16,13.32,15.47},      // ok 37
  {2.01,3.02,4.02,5.03,6.05,7.07,8.10,10.16,13.32,15.47},      // ok 38
  {2.01,3.02,4.03,5.04,6.07,7.1,8.14,10.24,13.5,15.74},        // ok 39
  {2.01,3.02,4.03,5.04,6.07,7.1,8.14,10.24,13.5,15.74},        // ok 40
  {2.01,3.02,4.03,5.04,6.07,7.1,8.14,10.24,13.5,15.74},        // ok 41
  {2.01,3.02,4.03,5.04,6.07,7.1,8.14,10.24,13.5,15.74},        // ok 42
  {2.01,3.02,4.03,5.04,6.07,7.1,8.14,10.24,13.5,15.74},        // ok 43
  {1.00,3.01,4.02,5.04,6.06,7.1,8.14,9.21,10.28,13.59},        // ok 44
  {1.00,3.01,4.02,5.04,6.06,7.1,8.14,9.21,10.28,13.59},        // ok 45
  {1.01,2.01,3.02,4.03,5.06,6.08,7.12,9.17,10.33,13.54},       // ok 46
  {1.01,2.01,3.02,4.03,5.06,6.09,7.13,9.28,10.34,13.72},       // ok 47
  {1.00,2.01,3.02,4.04,5.07,6.10,7.14,8.19,10.28,12.46},       // ok 48
  {1.01,2.01,3.02,4.03,5.06,6.10,7.14,8.21,10.30,12.66},       // ok 49
  {1.00,2.01,3.02,4.04,5.07,6.11,7.16,8.24,10.33,12.27},       // ok 50
  {1.01,2.02,3.03,4.05,5.09,6.13,7.20,8.27,10.49,11.64},       // ok 51
  {1.01,2.01,3.03,4.04,5.09,6.14,7.21,8.3,10.57,11.52},        // ok 52
  {1.01,2.01,3.03,4.06,5.10,6.15,7.22,8.31,10.45,11.77},       // ok 53
  {1.01,2.01,3.03,4.05,5.10,6.15,7.23,8.32,10.46,11.39},       // ok 54
  {1.00,2.01,3.02,4.04,5.10,6.16,7.23,8.33,10.61,11.81},       // ok 55
  {1.01,2.02,3.04,4.06,5.11,6.17,7.25,8.36,9.47,10.69},        // ok 56
  {1.00,2.01,3.03,4.06,5.10,6.17,7.25,8.38,9.47,10.43},        // ok 57
  {1.01,2.02,3.04,4.06,5.12,6.18,7.33,8.45,9.61,10.83},        // ok 58
  {1.01,2.02,3.04,4.08,5.12,6.21,7.34,8.46,9.64,10.83},        // ok 59
  {1.00,2.01,3.04,4.07,5.13,6.20,7.34,8.49},                   // ok 60
  {1.00,2.02,3.04,4.09,5.15,6.23,7.38,8.55},                   // ok 61
  {1.01,2.02,3.05,4.09,5.16,6.24,7.40,8.58},                   // ok 62
  {1.01,2.02,3.05,4.09,5.18,6.28,7.45},                        // ok 63
  {1.01,2.02,3.05,4.10,5.20,6.31,7.47},                        // ok 64
  {1.01,2.02,3.06,4.06,5.21,6.32,7.50},                        // ok 65
  {1.01,2.03,3.06,4.10,5.22,6.36,7.52},                        // ok 66
  {1.01,2.03,3.07,4.10,5.25,6.39,7.60},                        // ok 67
  {1.01,2.04,3.07,4.12,5.27,6.44},      // ok 68
  {1.01,2.03,3.06,4.17,5.31,6.47},      // ok 69
  {1.01,2.03,3.06,4.18,5.32,6.49},      // ok 70
  {1.01,2.04,3.05,4.19,5.35,6.62},      // ok 71
  {1.01,2.03,3.04,4.19,5.35}, // 72
  {1.01,2.02,3.13,4.17,5.35}, // 73
  {1.01,2.04,3.13,3.94,5.35}, // 74
  {1.01,2.04,3.13,3.71}, // 75
  {1.01,2.02,3.13,3.51}, // 76
  {1.01,2.02,3.15,3.31}, // 77
  {1.01,2.06,3.13,3.13}, // 78
  {1.01,2.07,2.95,2.95}, // 79
  {1.01,2.03,2.78}, // 80
  {1.02,2.03,2.63}, // 81
  {1.02,2.03,2.48}, // 82
  {1.02,2.03,2.34}, // 83
  {1.01,2.03,2.21}, // 84
  {1.03,2.05,2.09}, // 85
  {1.03,2.05,2.09}, // 86
  {1.03,2.05,2.09}, // 87
  {1.03,2.05,2.09}, // 88
};

const static std::vector<std::vector<float>> AMPLITUDES{
  {0.13,0.08,0.15,0.09,0.14,0.08,0.07,0.06,0.14,0.07}, // 21
  {0.1,0.11,0.14,0.14,0.14,0.07,0.06,0.06,0.11,0.07}, // 22
  {0.09,0.07,0.12,0.08,0.16,0.1,0.14,0.09,0.08,0.07}, // 23
  {0.12,0.13,0.08,0.08,0.08,0.09,0.09,0.09,0.13,0.11}, // 24
  {0.16,0.1,0.07,0.14,0.14,0.09,0.1,0.07,0.06,0.07}, // 25
  {0.1,0.22,0.09,0.09,0.08,0.1,0.12,0.09,0.06,0.05}, // 26
  {0.2,0.1,0.13,0.1,0.1,0.08,0.07,0.07,0.07,0.08}, // 27
  {0.15,0.17,0.06,0.11,0.08,0.1,0.09,0.07,0.09,0.07}, // 28
  {0.11,0.11,0.1,0.09,0.13,0.09,0.06,0.16,0.1,0.07}, // 29
  {0.13,0.12,0.06,0.09,0.1,0.07,0.14,0.07,0.1,0.11}, // 30
  {0.19,0.11,0.08,0.14,0.06,0.12,0.07,0.07,0.06,0.1}, // 31
  {0.25,0.09,0.1,0.06,0.12,0.15,0.06,0.05,0.07,0.05}, // 32
  {0.3,0.08,0.11,0.08,0.09,0.12,0.07,0.06,0.04,0.05}, // 33
  {0.32,0.09,0.09,0.08,0.08,0.05,0.08,0.06,0.07,0.08}, // 34
  {0.27,0.11,0.07,0.11,0.06,0.1,0.07,0.08,0.06,0.06}, // 35
  {0.17,0.06,0.06,0.08,0.11,0.21,0.08,0.1,0.06,0.07}, // 36
  {0.17,0.06,0.06,0.08,0.11,0.21,0.08,0.1,0.06,0.07}, // 37
  {0.14,0.13,0.14,0.05,0.09,0.09,0.07,0.05,0.06,0.16}, // 38
  {0.12,0.09,0.09,0.14,0.13,0.07,0.08,0.11,0.08,0.09}, // 39
  {0.14,0.15,0.16,0.08,0.04,0.06,0.15,0.05,0.08,0.09}, // 40
  {0.13,0.25,0.09,0.11,0.06,0.05,0.07,0.11,0.09,0.05}, // 41
  {0.13,0.25,0.09,0.11,0.06,0.05,0.07,0.11,0.09,0.05}, // 42
  {0.07,0.07,0.11,0.15,0.07,0.06,0.16,0.1,0.13,0.07}, // 43
  {0.12,0.17,0.16,0.1,0.09,0.09,0.05,0.06,0.11,0.04}, // 44
  {0.15,0.22,0.11,0.09,0.05,0.12,0.13,0.04,0.04,0.05}, // 45
  {0.23,0.07,0.12,0.06,0.09,0.09,0.04,0.19,0.05,0.06}, // 46
  {0.11,0.23,0.15,0.09,0.09,0.11,0.05,0.06,0.04,0.06}, // 47
  {0.19,0.17,0.09,0.06,0.17,0.04,0.16,0.03,0.05,0.03}, // 48
  {0.07,0.39,0.07,0.08,0.07,0.08,0.04,0.05,0.1,0.06}, // 49
  {0.28,0.1,0.14,0.07,0.06,0.07,0.07,0.11,0.04,0.04}, // 50
  {0.17,0.14,0.17,0.07,0.16,0.09,0.08,0.05,0.04,0.03}, // 51
  {0.34,0.21,0.04,0.07,0.02,0.09,0.12,0.05,0.03,0.02}, // 52
  {0.31,0.06,0.26,0.04,0.05,0.08,0.08,0.02,0.06,0.04}, // 53
  {0.31,0.06,0.26,0.04,0.05,0.08,0.08,0.02,0.06,0.04}, // 54
  {0.05,0.29,0.05,0.17,0.15,0.06,0.05,0.07,0.05,0.07}, // 55
  {0.1,0.17,0.07,0.11,0.05,0.19,0.04,0.16,0.06,0.05}, // 56
  {0.1,0.17,0.07,0.11,0.05,0.19,0.04,0.16,0.06,0.05}, // 57
  {0.39,0.11,0.18,0.08,0.02,0.05,0.04,0.09,0.02,0.02}, // 58
  {0.56,0.07,0.03,0.11,0.05,0.05,0.04,0.04,0.02,0.02}, // 59
  {0.25,0.05,0.24,0.08,0.15,0.09,0.02,0.06,0.02,0.03}, // 60
  {0.46,0.17,0.12,0.02,0.07,0.06,0.02,0.05,0.01,0.01}, // 61
  {0.21,0.44,0.06,0.04,0.08,0.06,0.05,0.02,0.02,0.02}, // 62
  {0.31,0.14,0.15,0.13,0.09,0.03,0.06,0.02,0.04,0.04}, // 63
  {0.4,0.24,0.17,0.04,0.03,0.05,0.02,0.03,0.01,0.01}, // 64
  {0.37,0.18,0.14,0.15,0.06,0.03,0.02,0.02,0.02,0.01}, // 65
  {0.36,0.36,0.03,0.09,0.07,0.02,0.03,0.03,0.01,0.01}, // 66
  {0.34,0.26,0.14,0.08,0.05,0.04,0.04,0.01,0.02,0.01}, // 67
  {0.6,0.16,0.07,0.04,0.04,0.03,0.02,0.01,0.01,0.01}, // 68
  {0.17,0.53,0.06,0.03,0.06,0.04,0.01,0.03,0.04,0.02}, // 69
  {0.47,0.2,0.11,0.08,0.03,0.03,0.02,0.01,0.02,0.03}, // 70
  {0.52,0.1,0.22,0.03,0.02,0.04,0.02,0.02,0.01,0.01}, // 71
  {0.52,0.16,0.18,0.02,0.03,0.04,0.02,0.01,0.01,0.01}, // 72
  {0.47,0.35,0.07,0.04,0.01,0.01,0.03,0.01,0.01,0.01}, // 73
  {0.55,0.16,0.13,0.08,0.02,0.01,0.02,0.01,0.01,0.01}, // 74
  {0.41,0.22,0.1,0.08,0.04,0.06,0.02,0.04,0.02,0.01}, // 75
  {0.41,0.22,0.1,0.08,0.04,0.06,0.02,0.04,0.02,0.01}, // 76
  {0.41,0.22,0.1,0.08,0.04,0.06,0.02,0.04,0.02,0.01}, // 77
  {0.26,0.33,0.2,0.02,0.1,0.01,0.03,0.02,0.03,0.01}, // 78
  {0.36,0.26,0.17,0.09,0.04,0.02,0.01,0.01,0.02,0.01}, // 79
  {0.81,0.06,0.03,0.06,0.01,0.01,0.02,0.01}, // 80
  {0.60,0.25,0.04,0.03,0.03,0.02,0.02,0.01}, // 81
  {0.60,0.25,0.04,0.03,0.03,0.02,0.02,0.01}, // 82
  {0.7,0.16,0.08,0.02,0.02,0.01,0.01}, // 83
  {0.7,0.16,0.08,0.02,0.02,0.01,0.01}, // 84
  {0.7,0.16,0.08,0.02,0.02,0.01,0.01}, // 85
  {0.7,0.16,0.08,0.02,0.02,0.01,0.01}, // 86
  {0.7,0.16,0.08,0.02,0.02,0.01,0.01}, // 87
  {0.74,0.1,0.05,0.06,0.03,0.01}, // 88
  {0.74,0.1,0.05,0.06,0.03,0.01}, // 89
  {0.74,0.1,0.05,0.06,0.03,0.01}, // 90
  {0.74,0.1,0.05,0.06,0.03,0.01}, // 91
  {0.90,0.06,0.02,0.01,0.01}, // 92
  {0.90,0.06,0.02,0.01,0.01}, // 93
  {0.90,0.06,0.02,0.01,0.01}, // 94
  {0.91,0.06,0.02,0.01}, // 95
  {0.91,0.06,0.02,0.01}, // 96
  {0.91,0.06,0.02,0.01}, // 97
  {0.91,0.06,0.02,0.01}, // 98
  {0.91,0.06,0.02,0.01}, // 99
  {0.82,0.14,0.02}, // 100
  {0.82,0.14,0.02}, // 101
  {0.82,0.14,0.02}, // 102
  {0.82,0.14,0.02}, // 103
  {0.82,0.14,0.02}, // 104
  {0.82,0.14,0.02}, // 105
  {0.82,0.14,0.02}, // 106
  {0.82,0.14,0.02}, // 107
  {0.82,0.14,0.02}, // 108
};

const static std::vector<std::vector<float>> PHASES{
  {2.93,3.02,0.54,-2.94,0.37,0.99,-1.98,3.03,0.34,-2.56}, // 21
  {-1.96,-1.26,0.09,-2.61,-0.19,2.06,-3.08,1.21,1.45,-2.92}, // 22
  {-0.1,1.7,-1.41,-0.67,-0.23,-1.42,0.37,1.23,0.99,-2.7}, // 23
  {-0.64,-2.59,1.0,-0.82,-2.17,0.75,2.55,-1.59,1.89,-0.89}, // 24
  {-1.43,2.43,1.8,0.15,2.54,-1.22,2.19,1.78,-2.12,2.28}, // 25
  {1.27,0.22,2.59,1.83,3.01,-3.1,-2.23,-2.09,-0.98,0.37}, // 26
  {-0.95,-1.61,-0.09,2.05,3.13,2.37,-2.53,-1.87,-1.35,-0.18}, // 27
  {-0.85,-0.78,-0.27,2.9,-2.08,1.15,2.59,0.18,1.79,-1.43}, // 28
  {1.51,-0.44,0.79,0.93,-0.16,1.69,2.36,-1.25,-2.82,-0.63}, // 29
  {1.15,1.73,-2.65,2.69,-2.19,-0.8,-0.07,1.69,-2.61,1.92}, // 30
  {-1.76,-2.82,-0.35,2.61,1.11,2.06,-2.17,-0.91,0.87,1.61}, // 31
  {1.68,-2.36,-2.3,2.89,-0.28,1.94,2.21,-0.86,-1.99,2.84}, // 32
  {0.48,-0.27,1.99,-0.06,-0.63,0.26,0.99,2.33,1.89,1.06}, // 33
  {-0.09,2.23,-1.6,-2.9,-2.3,-1.52,1.89,1.31,1.43,-0.7}, // 34
  {-0.85,0.52,2.62,-1.98,-1.54,-0.76,-1.32,1.29,-0.63,0.81}, // 35
  {-0.24,-2.68,-1.36,-0.85,-0.86,2.73,-0.05,-0.4,-2.01,-1.22}, // 36
  {-1.42,-1.12,-1.53,-0.4,2.88,0.08,0.68,-0.74,-0.05,-2.52}, // 37
  {-1.95,0.42,1.32,0.61,-1.9,2.36,3.07,0.22,2.43,1.65}, // 38
  {-2.86,1.86,-2.67,0.13,1.3,2.95,-0.88,2.73,2.98,0.76}, // 39
  {2.3,0.31,1.25,-3.0,-2.32,-1.85,-1.58,-0.98,2.01,-2.27}, // 40
  {-2.0,-2.12,2.42,-2.35,-1.75,-2.81,-0.81,-1.23,0.61,-2.49}, // 41
  {-2.61,-2.19,0.47,2.11,1.42,-2.15,1.92,-2.95,-2.69,2.47}, // 42
  {-1.45,-2.9,-2.3,0.76,0.19,-2.84,2.49,0.92,2.49,-0.41}, // 43
  {-1.58,-0.28,2.65,0.28,-0.48,-2.55,-2.06,-1.35,-2.53,1.51}, // 44
  {-2.35,0.92,-0.92,-3.09,1.1,-1.13,3.06,1.69,1.67,-0.38}, // 45
  {2.9,-0.12,-2.86,2.4,-1.74,-0.06,2.19,-1.02,3.13,-2.16}, // 46
  {2.16,-1.22,-2.69,0.67,-2.66,2.97,1.34,-2.8,2.43,-1.36}, // 47
  {2.81,-1.28,-1.77,0.1,-0.75,1.81,-0.95,-2.05,-0.65,-1.64}, // 48
  {-2.91,0.48,-0.91,-1.32,-0.52,0.62,-0.07,2.75,0.97,1.51}, // 49
  {-0.11,-0.94,3.05,1.43,0.72,1.37,0.21,-2.61,-2.62,-2.54}, // 50
  {-2.65,0.65,1.31,-2.19,0.74,-1.13,-2.58,-0.9,-0.78,-0.91}, // 51
  {1.28,-2.84,2.64,2.68,-3.09,2.56,-0.34,2.02,-2.67,0.68}, // 52
  {-0.3,-0.88,3.04,2.98,1.52,-2.87,1.41,-2.58,-3.04,-1.31}, // 53
  {-1.5,-2.72,2.98,0.94,1.99,2.03,-1.2,-0.4,-1.49,3.07}, // 54
  {-2.28,1.64,1.07,0.82,-1.33,-0.46,2.75,0.09,0.46,0.21}, // 55
  {-2.82,2.69,2.04,-1.04,2.65,1.05,-0.63,-0.97,-2.15,3.0}, // 56
  {2.14,0.37,-1.07,1.59,-2.72,0.65,0.74,-2.02,2.5,-2.12}, // 57
  {1.28,2.95,0.41,2.1,1.43,-3.06,-2.19,-0.69,0.88,1.37}, // 58
  {0.57,-2.86,0.43,0.02,-2.64,3.02,-1.57,-0.47,-0.56,-1.03}, // 59
  {2.07,1.3,-2.03,1.65,-0.64,1.04,2.92,-3.03,-2.78,1.34}, // 60
  {1.46,-2.57,1.95,2.69,0.5,0.78,-0.74,2.31,-2.46,2.01}, // 61
  {-0.18,-1.2,-2.2,-0.2,-2.1,-0.97,-1.51,1.19,-0.76,2.37}, // 62
  {-2.73,0.51,3.0,-2.65,2.3,-0.74,0.2,-1.89,3.0,2.99}, // 63
  {1.55,-2.31,0.91,2.1,-0.39,-0.38,1.58,2.04,2.38,1.22}, // 64
  {0.09,2.45,2.44,-0.87,1.53,2.32,1.39,1.76,-2.65,-0.4}, // 65
  {-2.31,-0.84,1.38,-2.64,-2.5,2.03,1.81,0.98,0.42,-2.16}, // 66
  {-2.91,2.96,1.05,0.64,1.53,-1.43,-1.01,-0.76,-0.17,-0.72}, // 67
  {1.99,-1.2,0.4,-1.11,1.49,2.12,1.59,-0.58,-3.03,2.92}, // 68
  {1.15,1.23,-2.75,-0.73,3.0,-2.48,-1.51,0.25,-0.7,-1.97}, // 69
  {-3.13,-1.83,-1.3,2.89,2.96,-2.13,-2.03,1.54,-1.78,1.43}, // 70
  {0.39,-0.29,1.51,2.54,2.76,1.03,2.17,1.03,-0.25,-2.98}, // 71
  {-2.19,-1.08,2.89,2.94,-2.47,1.07,-1.32,-2.78,0.46,-2.72}, // 72
  {2.81,-0.89,-2.71,-1.67,2.84,0.75,1.33,2.58,2.35,1.81}, // 73
  {0.71,-0.71,3.07,0.9,2.68,2.63,2.69,1.83,0.19,1.42}, // 74
  {2.65,2.61,-2.76,1.74,2.22,-1.45,0.08,0.16,-0.15,-0.16}, // 75
  {1.02,-2.3,-1.2,-1.26,-0.2,-0.35,2.43,2.3,2.29,0.44}, // 76
  {0.47,0.74,1.91,0.38,-1.74,1.1,-2.7,2.57,2.94,-0.61}, // 77
  {-2.08,1.1,1.92,-1.24,2.83,-1.4,-2.08,-1.23,-2.52,-0.67}, // 78
  {3.13,0.02,-1.05,-0.87,-1.17,2.28,-1.18,2.06,1.26,0.64}, // 79
  {0.1,-0.66,0.74,-2.08,2.45,2.49,1.88,-1.8,-2.43,-1.89}, // 80
  {1.51,1.14,2.96,-0.39,-2.48,0.9,1.37,-2.48,-0.66,1.96}, // 81
  {2.69,-1.03,-2.37,0.94,0.12,2.08,2.48,1.84,-1.39,1.85}, // 82
  {1.21,1.67,2.98,1.92,-0.18,1.73,2.25,1.54,1.07,2.34}, // 83
  {2.22,1.61,2.49,1.67,0.98,2.3,-2.36,-1.62,2.97,1.29}, // 84
  {-0.5,-2.46,-2.38,0.05,2.62,1.23,-2.06,-0.3,0.97,1.67}, // 85
  {2.23,2.53,0.35,-3.06,-0.07,-2.76,1.34,1.15,0.19,1.06}, // 86
  {1.2,2.46,-1.79,0.86,2.24,-3.07,-2.16,-2.4,0.11,-2.17}, // 87
  {-0.97,0.39,-2.76,-1.16,-1.4,-3.01,1.96,1.15,-0.26,-0.16}, // 88
  {0.1,-2.71,3.1,-0.48,-1.98,1.69,-1.6,0.24,-2.79,2.67}, // 89
  {-2.1,-1.76,1.82,2.91,1.06,-2.44,2.53,-2.02,2.01,1.77}, // 90
  {3.06,-2.08,2.77,1.84,2.33,-0.41,-1.97,0.71,-0.03,-0.39}, // 91
  {-1.78,-0.18,-1.95,-0.6,-1.75,-1.48,-2.54,-0.54,1.2,-0.47}, // 92
  {-2.02,2.97,1.83,-0.78,2.09,2.0,1.76,2.43,1.56,-2.72}, // 93
  {-0.41,-0.33,2.72,-2.3,-3.14,-1.34,2.7,-1.63,1.9,-3.0}, // 94
  {0.02,-0.71,0.5,-2.59,-2.63,-2.9,1.46,1.03,2.14,-2.87}, // 95
  {2.92,0.51,2.18,-1.25,-1.52,-1.57,-2.38,-2.59,-2.06,0.42}, // 96
  {-2.75,-2.54,-3.09,0.95,0.69,-0.05,0.01,-1.29,1.74,2.3}, // 97
  {-2.07,-0.78,1.82,1.73,-1.93,-2.04,-1.93,-2.69,1.33,3.14}, // 98
  {0.87,1.99,2.9,2.56,2.68,2.83,1.51,-3.14,0.1,-2.75}, // 99
  {-2.07,-3.06,2.4,2.08,2.25,2.19,-2.0,1.91,0.2,-2.25}, // 100
  {2.54,-0.08,-1.87,-1.93,-1.91,-2.03,1.02,-2.84,2.79,0.0}, // 101
  {-2.29,-2.03,0.61,0.67,0.65,0.62,0.63,-2.98,-1.77,2.85}, // 102
  {2.41,0.96,-2.17,-2.09,-2.31,-2.26,2.86,3.13,-1.16,2.79}, // 103
  {-2.7,-2.08,-3.02,-3.07,-2.99,-3.06,3.13,1.42,0.47,1.94}, // 104
  {-2.91,1.58,-1.15,-1.17,-1.26,-1.4,-1.44,0.64,-1.59,3.14}, // 105
  {3.03,2.88,2.68,2.62,2.42,2.36,2.31,-2.4,-1.78,-0.93}, // 106
  {-0.77,-2.95,-3.12,-3.07,-3.1,-3.08,2.94,-2.7,0.76,3.05}, // 107
  {-0.32,2.62,2.36,2.2,2.23,2.22,-2.35,1.21,0.87,3.14}, // 108
};


static std::default_random_engine generator;
static std::normal_distribution<float> PHASES_NORM(0, 1.5);
static std::normal_distribution<float> PHASES_NOISE(0.01, 0.001);
const static float CENT = 1.00057778951f;
const static float MAX_NUMBER_OF_PERIODS = 4511.0f;
const static int POLYPHONY = 20; /*number of notes allowed simultaniously*/

extern bool isSustainOn;

inline float partialFromMidiKey(float key, float partial = 1.0f) {
    float beta = 0.000033; // 0.006;
    float f0 = 440.0f * std::powf(2.0f, (key - 69.0f) / 12.0f);
    float m = partial * std::sqrt(1.0f + beta * partial * partial);
    return m * f0;
}

template <typename T> float sign(T val) 
{
    return (T(0) < val) - (val < T(0));
}

template <typename T>
inline std::vector<T> complexToHalfComplex(std::vector<std::complex<T>> C)
{
    std::vector<T> H(2 * (C.size() - 1));
    H[0] = C[0].real();
    for (size_t i = 1; i < C.size() - 1; i++)
    {
        H[2 * (i - 1) + 1] = C[i].real();
        H[2 * (i - 1) + 2] = C[i].imag();
    }
    H[H.size() - 2] = C.back().real();
    if (C.back().imag() != 0)
    {
        H.push_back(C.back().imag());
    }
    return H;
}

inline std::vector<float> irfft(std::vector<std::complex<float>>& complexIn)
{
    std::vector<float> in = complexToHalfComplex(complexIn);
    std::vector<float> out(in.size());
    pocketfft::shape_t shapeReal = { out.size() };
    pocketfft::stride_t strideReal = { sizeof(float) };    // {(&dataReal[1] - &dataReal[0]) * CHAR_BIT};
    //pocketfft::stride_t strideComplex = { sizeof(float) }; //{(&dataComplex[1] - &dataComplex[0]) * CHAR_BIT};
    pocketfft::r2r_fftpack(shapeReal, strideReal, strideReal, { 0 }, false, false, in.data(), out.data(), 1.0f);
    // #ifdef MYDEBUG
    //   write_vector(out, "F.csv", ';');
    // #endif
    return out;
}

class ManualPiano
{
private:
    //float decay = 0.00037f;
    float n = 44100.0f;
    //float a = 0.0f;
    size_t idx = 0;
    size_t t = 0;
    float fLocal = 0.0;
    //std::vector<float> harmonics;
    //std::vector<float> amplitudes;
    //std::vector<float> phasesL;
    //std::vector<float> phasesR;
public:
    void start(int midiKey, float sampleRate = 44100.0) {
        t = 0;
        fLocal = partialFromMidiKey(midiKey) * n / sampleRate;
        idx = midiKey - 21;
        //harmonics.clear();
        //amplitudes.clear();
        //phasesL.clear();
        //phasesR.clear();
        //float localMaxFrequency = 20000.0f * n / sampleRate;
        //std::vector<float> pL;// = PHASES_NORM(generator);
        //std::vector<float> pR;// = PHASES_NORM(generator);
        //int firstPartial = 1;
        //int lastPartial = 10;

        //phasesL.push_back(PHASES_NORM(generator));
        //phasesR.push_back(PHASES_NORM(generator));

        //if (midiKey <= 41)
        //{
        //    firstPartial += 2;
        //    lastPartial += 2;
        //}
        //else if (midiKey <= 47)
        //{
        //    firstPartial += 3;
        //    lastPartial += 3;
        //}
        //else
        //{
        //}
        
        //for (size_t partial = firstPartial; partial <= lastPartial; partial++)
        //{
        //    float fLocal = partialFromMidiKey(midiKey, partial) * n / sampleRate;
        //    if (fLocal >= localMaxFrequency)
        //    {
        //        break;
        //    }
        //    harmonics.push_back(fLocal);
        //    phasesL.push_back(phasesL.back() + PHASES_NORM(generator));
        //    phasesR.push_back(phasesR.back() + PHASES_NORM(generator));
        //}
        //float N = float(harmonics.size());
        //a = 6.0f / (N * (2.0f * N * N + 3.0f * N + 1.0f));

        //for (size_t i = 0; i < harmonics.size(); i++)
        //{
        //    float ampPart = float(i) - float(harmonics.size());
        //    float amp = a * ampPart * ampPart;
        //    amplitudes.push_back(amp);
        //}
    }
    std::vector<float> step()
    {
        float yL = 0.0;
        float yR = 0.0;
        for (size_t i = 0; i < FREQUENCIES[idx].size(); i++)
        {
            float f = FREQUENCIES[idx][i] * fLocal;
            float a = AMPLITUDES[idx][i];
            float pL = PHASES[idx][i] + PHASES_NOISE(generator);
            float pR = PHASES[idx][i] + PHASES_NOISE(generator);
            //float f = harmonics[i];
            //float pi = juce::MathConstants<float>::pi;
            //float h = 2.0f * pi * f * float(t) / n;

            //float dPart = h * decay;
            //float d = amplitudes[i] / (1.0f + dPart * dPart);
            //int strings = phasesL[i].size();
            //float yPartialL = 0.0;
            //float yPartialR = 0.0;
            //for (size_t j = 0; j < strings; j++)
            //{
            //    //float m = 1.0f + float(j) * 0.0001f * CENT;
            //    float ap = std::sin(phasesL[i][j] + h);
            //    phasesL[i][j] += PHASES_NOISE(generator);
            //    if (ap > yPartialL)
            //    {
            //        yPartialL = ap;
            //    }
            //    ap = std::sin(phasesR[i][j] + h);
            //    phasesR[i][j] += PHASES_NOISE(generator);
            //    if (ap > yPartialR)
            //    {
            //        yPartialR = ap;
            //    }
            //}
            float h = 2.0f * juce::MathConstants<float>::pi * f * float(t) / n;
            float d = std::exp(-0.0003f * h);
            yL += a * std::sin(pL + h) * d;
            yR += a * std::sin(pR + h) * d;
        }
        t++;
        return { yL, yR };
    }
};

class NeuralModel
 {  
 public:
     //float sampleRate = 44100.0;
     std::vector<int64_t> inputShape{ 0 }, outputShape{ 0 };
     Ort::Session session = Ort::Session{ nullptr };
     std::string inputName, outputName;
     //std::vector<Ort::AllocatedStringPtr> inputNamePtr, outputNamePtr;

     NeuralModel(const char* name = "engineMain")
     {
        //juce::Logger::setCurrentLogger(juce::FileLogger::createDefaultAppLogger("PFplugin", "PFplugin.txt", "######"));
        //juce::Logger::writeToLog("ModelInfo constructor, Current dir: " + juce::File::getCurrentWorkingDirectory().getFullPathName());
        Ort::Env env{ ORT_LOGGING_LEVEL_WARNING, "Piano" };
        //juce::Logger::writeToLog("ModelInfo constructor, passed env");

        //const Ort::SessionOptions session_options;

        /// Load from file
        //std::string str = "SteinwayD.oms";
        //std::wstring wide_string = std::wstring(str.begin(), str.end());
        //std::basic_string<ORTCHAR_T> model_file = std::basic_string<ORTCHAR_T>(wide_string);
        //session = Ort::Session(env, model_file.c_str(), session_options);
        /// Load from memory
        int dataSizeInBytes;
        const void* data = BinaryData::getNamedResource(name, dataSizeInBytes);
        if (data) {
            session = Ort::Session(env, data, dataSizeInBytes, Ort::SessionOptions{ nullptr });
        }
        else {
            juce::Logger::writeToLog("Could not find engine");
            throw;
        }
        inputShape = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
        outputShape = session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape(); 

        Ort::AllocatorWithDefaultOptions allocator;

        //std::vector<const char*> input_node_names;

        auto input_name = session.GetInputNameAllocated(0, allocator);
        //input_node_names.push_back(input_name.get());
        //std::vector < const char* >outputNames{ session.GetOutputNameAllocated(0, allocator).get() };
        inputName = input_name.get();
        //std::cout << input_name.get() << "\n";
        auto output_name = session.GetOutputNameAllocated(0, allocator);
        outputName = output_name.get();
        //DBG("oN=" + outputName);
        std::string nm = name;
        //DBG("Model " + nm + " constructed. I=" + std::to_string(inputShape[0]) + " T=" + std::to_string(outputShape[0]) + " IN=" + inputName + " oN=" + outputName);
     }
     void eval(std::vector<float> &I, std::vector<float> &O) {
         //DBG("before eval ->" + inputName);
     /*    std::vector<Ort::Value> inputTensor;
         std::vector<Ort::Value> outputTensor;*/
         //DBG("I = " + std::to_string(I.size()) + " O = " + std::to_string(O.size()));
         Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtDeviceAllocator, OrtMemType::OrtMemTypeDefault);
         auto inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, I.data(), I.size(), inputShape.data(), inputShape.size());
         //DBG("1");
         auto outputTensor = Ort::Value::CreateTensor<float>(memoryInfo, O.data(), O.size(), outputShape.data(), outputShape.size());
         //DBG("2");
         //Ort::AllocatorWithDefaultOptions allocator;
         //DBG("3");
         //std::vector < const char* >inputNames{ session.GetInputNameAllocated(0, allocator).get() };
         //DBG("4");
         //std::vector < const char* >outputNames{ session.GetOutputNameAllocated(0, allocator).get() };
         //DBG("5");
         //std::string oN = *outputNames.data();
         //DBG("oN=" + inputName);
         //std::string iN(*inputNames.begin(), *inputNames.end());
         //DBG("iN=" + iN);

         std::vector < const char* >inputNames{ inputName.data()};
         std::vector < const char* >outputNames{ outputName.data() };

         session.Run(
             Ort::RunOptions{ nullptr },
             inputNames.data(),
             &inputTensor,
             1,
             outputNames.data(),
             &outputTensor,
             1
         );
         //DBG("7");
         //DBG("after eval ->" + inputName);
     }
 };

//class DigitalWaveguide
// {
// private:
//     NeuralModel* model;// = NeuralModel("engineDW");
//     size_t currentStep = 0;
//     //size_t length = 1102;
//     size_t smoothing;
//     float sustain;
//     std::vector<float> delayL;
//     std::vector<float> delayR;
//     std::vector<float> powerSpectrum;
//     std::vector<float> I{ 0 };
// public:
//     DigitalWaveguide(){}
//     DigitalWaveguide(NeuralModel* m, const size_t delayLength = 1102) {
//         model = m;
//         delayL.resize(delayLength);
//         delayR.resize(delayLength);
//         powerSpectrum.resize(model->outputShape[0]);
//     }
//     void start(const float normKey, const size_t _smoothing = 2, const float _sustain = 0.90, size_t delayLength = 1102) {
//         
//         //float midiKey = normKey * 87.0f + 21.0f;
//         //float globalFrequency = frequencyFromMidiKey(midiKey);
//         //int period = int(std::round(model->sampleRate / globalFrequency));
//         //int reps = std::ceil(float(delayLength) / float(period));
//         //delayLength = period * reps;
//
//         sustain = _sustain;
//         smoothing = _smoothing;
//         currentStep = 0;
//         I[0] = normKey;
//         
//         model->eval(I, powerSpectrum);
//         DBG("outside eval DigitalWaveguide");
//
//         size_t delayLengthFD = delayLength / 2 + 1;
//
//         std::vector<float> newPowerSpectrum;
//         if (delayLengthFD != powerSpectrum.size())
//         {
//             newPowerSpectrum.resize(delayLengthFD, 0.0);
//             size_t n = powerSpectrum.size();
//             for (size_t i = 0; i < n; i++)
//             {
//                 int idx = int(std::round(float(i * (delayLengthFD - 1)) / float(n - 1)));
//                 newPowerSpectrum[idx] += powerSpectrum[i];
//             }
//         }
//         else
//         {
//             newPowerSpectrum = powerSpectrum;
//         }
//
//
//         //size_t i = 0;
//         ////float m = 0.95f;
//         //float localF0 = int(std::round(globalFrequency * float(delayLength) / model->sampleRate));         
//         //while (localF0 < newPowerSpectrum.size()) {
//         //    newPowerSpectrum[localF0] *= 1.5f;
//         //    //m *= m;
//         //    i++;
//         //    localF0 = int(std::round(globalFrequency * float(i * delayLength) / model->sampleRate));
//         //}
//         
//         std::vector<std::complex<float>> freqsL(newPowerSpectrum.size());
//         std::vector<std::complex<float>> freqsR(newPowerSpectrum.size());
//
//         float randomPhaseL = PHASES_NORM(generator);
//         float randomPhaseR = PHASES_NORM(generator);
//         for (size_t i = 0; i < freqsL.size(); i++)
//         {
//             //newPowerSpectrum[i] += std::abs(POWER_NORM(generator));
//             randomPhaseL += PHASES_NORM(generator);
//             freqsL[i] = { newPowerSpectrum[i] * std::sin(randomPhaseL), newPowerSpectrum[i] * std::cos(randomPhaseL) };
//
//             randomPhaseR += PHASES_NORM(generator);
//             freqsR[i] = { newPowerSpectrum[i] * std::sin(randomPhaseR), newPowerSpectrum[i] * std::cos(randomPhaseR) };
//         }
//         delayL = irfft(freqsL);
//         delayR = irfft(freqsR);
//
//
//         float maxDelayAmpL = 0.0;
//         float maxDelayAmpR = 0.0;
//         for (size_t i = 0; i < delayL.size(); i++)
//         {
//             float currDelayAmpL = std::abs(delayL[i]);
//             if (currDelayAmpL > maxDelayAmpL)
//             {
//                 maxDelayAmpL = currDelayAmpL;
//             }
//             float currDelayAmpR = std::abs(delayR[i]);
//             if (currDelayAmpR > maxDelayAmpR)
//             {
//                 maxDelayAmpR = currDelayAmpR;
//             }
//         }
//
//         float n = float(delayL.size() - 1);
//         //float delta = juce::MathConstants<float>::pi / n;
//         for (size_t i = 0; i < delayL.size(); i++) {
//             delayL[i] /=  maxDelayAmpL;
//             delayR[i] /= maxDelayAmpR;
//
//             float p = (2.0f / n) * (float(i) - (n / 2.0f));
//             float e = 1.0f - p * p * p * p * p * p;
//
//             //float e = std::sin(float(i * reps) * delta);
//             //e = std::powf(e, 0.2f);
//             delayL[i] *= e;
//             delayR[i] *= e;
//         }
//         auto l = delayL.size();
//         delayL.resize(2 * l, 0.0);
//         delayR.resize(2 * l, 0.0);
//         //for (size_t i = 0; i < l; i++)
//         //{
//         //    delayL[l + i] = -delayR[i];
//         //    delayR[l + i] = -delayL[i];
//         //}
//         DBG("end of start DigitalWaveguide");
//         return;
//     }
//     std::vector<float> step() {
//         DBG("step start");
//         size_t pr = currentStep % delayL.size();
//         size_t pl = (currentStep + delayL.size() / 2) % delayL.size();
//
//         float wL = (delayL[pr] + delayL[pl]) / 2.0;
//         float wR = (delayR[pr] + delayR[pl]) / 2.0;
//
//         float wAvgL = 0.0;
//         float wAvgR = 0.0;
//         for (size_t i = pr; i < pr + smoothing; i++)
//         {
//             auto idx = i % delayL.size();
//             wAvgL += delayL[idx];
//             wAvgR += delayR[idx];
//         }
//         wAvgL /= float(smoothing);
//         wAvgR /= float(smoothing);
//         delayL[pr] = -1 * wAvgL * sustain;
//         delayL[pl] *= -POWER_NORM(generator);
//         delayR[pr] = -1 * wAvgR * sustain;
//         delayR[pl] *= -POWER_NORM(generator);
//
//         currentStep++;
//         DBG("step end");
//         return { wL, wR };
//     }
// };

struct pianoSound : public juce::SynthesiserSound
{
    pianoSound() {}
    bool appliesToNote(int midiNoteNumber) override { 
        if (21 <= midiNoteNumber && midiNoteNumber <= 108)
        {
            return true;
        }
        return false; 
    }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

struct pianoVoice : public juce::SynthesiserVoice
{
public:
    NeuralModel* MI;// = NeuralModel(); // = ModelInfo::instance();
    //customSynth* CS;
    double lastActive = juce::Time::getMillisecondCounterHiRes();
    float tailOff = 0.0;
    bool  voiceIsActive = false;
    pianoVoice(NeuralModel* _MI) {
        //mp = ManualPiano();
        MI = _MI;
        //MI->sampleRate = getSampleRate();
        targetAmps = std::vector<float>(MI->outputShape[0], 0);
        currentAmps = std::vector<float>(MI->outputShape[0], 0);
        phasesC1 = std::vector<float>(MI->outputShape[0], 0);
        phasesC2 = std::vector<float>(MI->outputShape[0], 0);
        I0 = std::vector<float>(MI->inputShape[0], 0);
    };

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<pianoSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

    void stopNote(float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newValue) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    bool isVoiceActive() const override {
        return tailOff > 0.0;
    }

    void getNextSample();

private:
    ManualPiano mp;
    float currentDecay = 1.0f;
    std::future<void> fut;
    std::vector<float> targetAmps;// = std::vector<float>(MI->outputShape[0], 0);
    std::vector<float> currentAmps;// = std::vector<float>(MI->outputShape[0], 0);
    std::vector<float> phasesC1;// = std::vector<float>(MI->outputShape[0], 0);
    std::vector<float> phasesC2;// = std::vector<float>(MI->outputShape[0], 0);
    std::vector<float> I0;// = std::vector<float>(MI->inputShape[0], 0);
    std::vector<float> W = std::vector<float>(2, 0);
    long x = 0;
    bool  keyIsDown = false;
    float level = 0.0f;
    float midiKey = 0.0f;
    float f = 0.0f;
    float period = 0.0f;
    float deltaStep = 0.0f;
    float fps = 44100.0f;

    void forward() {
        MI->eval(I0, targetAmps);

        //float ampsSum = 0.0;
        //for (size_t i = 0; i < targetAmps.size(); i++)
        //{
        //    ampsSum += targetAmps[i];
        //}
        //for (size_t i = 0; i < targetAmps.size(); i++)
        //{
        //    targetAmps[i] /= ampsSum;
        //}
        return;
    };
};

