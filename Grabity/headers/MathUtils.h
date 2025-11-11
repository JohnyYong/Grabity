/*!****************************************************************
\file: MathUtils.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim , 2301370
\brief: Provides a number of scalar math utility functions,constants
and inline methods optimized for performance such as angle conversions,
interpolations, clamping and more

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

// Constants
constexpr float PI = 3.14159265359f;
constexpr float TWO_PI = 2.0f * PI;
constexpr float HALF_PI = PI / 2.0f;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;

// Convert degrees to radians
inline float DegreesToRadians(float degrees) {
    return degrees * DEG_TO_RAD;
}

// Convert radians to degrees
inline float RadiansToDegrees(float radians) {
    return radians * RAD_TO_DEG;
}

// Clamp a value between a minimum and a maximum. Useful for lets say health value, prevent below 0 or above 100
inline float Clamp(float value, float minVal, float maxVal) {
    return std::fmax(minVal, std::fmin(value, maxVal));
}

// Linear interpolation between a and b by factor t (0 <= t <= 1)
inline float LerpScalar(float a, float b, float t) {
    return a + t * (b - a);
}

// Get the minimum of two values
inline float Min(float a, float b) {
    return (a < b) ? a : b;
}

// Get the maximum of two values
inline float Max(float a, float b) {
    return (a > b) ? a : b;
}

// Calculate the distance between two points in 2D space
inline float ScalarDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

// Calculate the square of the distance between two points (avoids expensive sqrt)
inline float ScalarDistanceSquared(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

// Generate a random float between 0 and 1
inline float RNGFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

// Generate a random float between min and max
inline float RNGRange(float min, float max) {
    return min + RNGFloat() * (max - min);
}

// Rotate a 2D vector by an angle (in radians)
inline void RotateVector(float& x, float& y, float angle) {
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);

    float newX = x * cosA - y * sinA;
    float newY = x * sinA + y * cosA;

    x = newX;
    y = newY;
}

// Wrap an angle (in radians) to the range [-PI, PI]. Useful for situations where angles are frequently manipulated(cam rotations etc)
// Used inline to reduce overhead function calls (optimizing sake)
inline float WrapAngle(float angle) {
    //wrap angles that exceed -pi,pi back to desired range
    while (angle <= -PI) angle += TWO_PI;
    while (angle > PI) angle -= TWO_PI;
    return angle;
}

#endif // MATH_UTILS_H