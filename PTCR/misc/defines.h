#pragma once
#include <cmath>
#include <memory>
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#define uchar unsigned char
#define uint unsigned int
#define sptr std::shared_ptr
#define msha std::make_shared
#define uptr std::unique_ptr
#define muni std::make_unique
#define GAMMA2 1
#define DEBUG 1
#define USE_SSE (1 && __SSE2__)
constexpr float pi = 3.1415926535897932385;
constexpr float pi2 = 2.0 * pi;
constexpr float pi4 = 4.0 * pi;
constexpr float ipi = 1.0 / pi;
constexpr float ipi2 = 1.0 / pi2;
constexpr float hpi = 0.5 * pi;
constexpr float qpi = 0.25 * pi;
constexpr float ihpi = 1.0 / hpi;
constexpr float infp = 1e30f;
constexpr float infn = -1e30f;
constexpr float eps = 1e-4f;
constexpr float eps2 = 1e-7f;
using std::vector;
using std::sin;
using std::cos;
using std::fabs;
using std::fmaxf;
using std::fminf;
using std::abs;
using std::fabs;
using std::fmax;
using std::fmin;
using std::sqrt;
using std::sqrtf;
using std::tan;
using std::atan2;
using std::acos;
using std::asin;
using std::cout;