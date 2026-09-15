#pragma once

constexpr float kMinPan = -100.f;
constexpr float kMaxPan = 100.f;

float clampPan(float pan);
float panToRms(float pan, bool isRight);
