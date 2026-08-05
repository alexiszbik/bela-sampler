#pragma once

constexpr float kMinCutoffHz = 20.f;
constexpr float kMaxCutoffHz = 20000.f;

float clampCutoffRatio(float ratio);
float cutoffRatioToHz(float ratio);
