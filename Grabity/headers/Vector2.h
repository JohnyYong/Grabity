/*!****************************************************************
\file: Vector2.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim , 2301370
\brief: Defines the Vector2 struct, which represents a vector with x and y
        componenets. It can do vector arithmetics, normalization, dot product,
        interpolation and other useful calculations

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

struct Vector2 {
    float x, y;

    // Constructors
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}

    // Vector Addition
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    // Vector Subtraction
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    // Scalar Multiplication
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    // Scalar Division
    Vector2 operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    // Dot Product (help with player view direction)
    float Dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Length (magnitude) (helps with speed/distance of objects)
    float Length() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalization
    Vector2 Normalize() const {
        float len = Length();
        return len != 0 ? Vector2(x / len, y / len) : Vector2(0, 0);
    }

    // Distance between two vectors (useful for collision logic)
    static float Distance(const Vector2& a, const Vector2& b) {
        return (b - a).Length();
    }

    // Distance squared between two vectors (saves computational cost of square rooting for optimization)
    static float DistanceSquared(const Vector2& a, const Vector2& b) {
        return (b - a).Length() * (b - a).Length();
    }

    // Angle between this vector and another ( useful for aiming,player controls, ai decision making)
    float AngleBetween(const Vector2& other) const {
        float dotProduct = Dot(other);
        float lengthsProduct = Length() * other.Length();
        if (lengthsProduct == 0) return 0; // Handle zero-Length vectors
        float cosine = dotProduct / lengthsProduct;
        return std::acos(cosine);
    }

    // Project this vector onto another vector (useful for shadow)
    Vector2 ProjectOnto(const Vector2& other) const {
        float scalar = Dot(other) / other.Dot(other);
        return other * scalar;
    }

    // Get a perpendicular vector (90 degrees rotation) (helps with lateral movement in 2d game and pathfinding)
    Vector2 Perpendicular() const {
        return Vector2(-y, x);
    }

    // Linear interpolation (useful for smooth transitions in animation or blending colors/camera movements)
    static Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
        return a + (b - a) * t;
    }
};

#endif // VECTOR2_H