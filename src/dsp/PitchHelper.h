#pragma once

constexpr float kMinPitchSemitones = -48.f;
constexpr float kMaxPitchSemitones = 48.f;

float clampPitchSemitones(float semitones);
float semitonesToPlaybackSpeed(float semitones);
