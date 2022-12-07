#pragma once
#include <cmath>
#include <memory>
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#define uchar unsigned char
#define uint unsigned int
#define sptr std::shared_ptr
#define msha std::make_shared
#define uptr std::unique_ptr
#define muni std::make_unique
#define GAMMA2 1
#define DEBUG 0
#define USE_SSE (1 && __SSE2__)
#define WIDTH 960
#define HEIGHT 720
#define SCALE 1.f
#define CLAMP ImGuiSliderFlags_AlwaysClamp
constexpr float pi = 3.1415926535897932385;
constexpr float pi2 = 2.0 * pi;
constexpr float pi4 = 4.0 * pi;
constexpr float ipi = 1.0 / pi;
constexpr float ipi2 = 1.0 / pi2;
constexpr float hpi = 0.5 * pi;
constexpr float qpi = 0.25 * pi;
constexpr float ihpi = 1.0 / hpi;
constexpr float infp = 1e4f;
constexpr float infn = -1e4f;
constexpr float eps = 1e-4f;
constexpr float eps2 = 1e-6f;
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
using std::string;
#define loop(i,j) for(uint i = 0 ; i < j ; i++)
#define println cout<<"\n"