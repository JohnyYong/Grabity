/*!****************************************************************
\file: Vector3.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim , 2301370
\brief: Defines the Vector3 struct, which represents a vector with x,y,z
componenets. It can do vector arithmetics, normalization, dot product,
cross product and other useful calculations.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#ifndef VECTOR3_H
#define VECTOR3_H

struct Vector3 {
    float x, y, z;

    // Constructors initialized to origin 0,0,0 for uninitialized vectors and easy initialization of specific value vectors
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    //operator overloading for vector arithmetics.
    // returns new vec3 instance
 
    // Vector Addition
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Vector Subtraction
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Scalar Multiplication
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Scalar Division
    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    // Dot Product
    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross Product
    Vector3 Cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Length (magnitude)
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Length Squared (for optimization when only comparison needed)
    float LengthSquared() const {
        return x * x + y * y + z * z;
    }

    // Normalize
    Vector3 Normalize() const {
        float len = Length(); // Calculate the length of the vector
        return len != 0 ? Vector3(x / len, y / len, z / len) : Vector3(0, 0, 0); //check for 0, if 0 return zero vec to prevent division by 0
        //returns a new vec3 normalized obj
    }

    // Distance between two vectors
    static float Distance(const Vector3& a, const Vector3& b) {
        return (b - a).Length();
    }

    // Distance squared between two vectors (saves computational cost of square rooting for optimization)
    static float DistanceSquared(const Vector3& a, const Vector3& b) {
        return (b - a).Length() * (b - a).Length();
    }

    // Angle between two vectors (in radians), useful for shooting mechanic, AI FOV
    static float AngleBetween(const Vector3& a, const Vector3& b) {
        float dotProd = a.Dot(b);
        float lengths = a.Length() * b.Length();
        return std::acos(dotProd / lengths);
    }

    // Linear interpolation (lerp) between two vectors. Useful for smooth animations/movements!
    static Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
        return start + (end - start) * t;
    }

    // Reflect this vector over a normal (useful for collision response)
    Vector3 Reflect(const Vector3& normal) const {
        return *this - normal * (2 * this->Dot(normal));
    }
#ifdef _LOGGING
    // Output for debugging, use friend in cases of private member variables
    friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
#endif // _LOGGING
};

#endif // VECTOR3_H