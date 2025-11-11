/*!****************************************************************
\file: Matrix4x4.h
\author: Jeremy Lim Ting Jie, jeremytingjie.lim , 2301370
\brief:  This header file defines the Matrix4x4 class, 
encapsulating operations and transformations for 4x4 matrices

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#pragma once
#ifndef MATRIX4X4_H
#define MATRIX4X4_H
#include <cmath>
#include <array>

#ifdef _IMGUI
#include <iostream>
#endif // _IMGUI
#include <stdexcept>
#include "Vector3.h" 

class Matrix4x4
{
public:
    // Constructors
    Matrix4x4(); // Default constructor (Identity matrix)
    Matrix4x4(const std::array<float, 16>& values); // Custom values constructor

    // Access operators
    float& operator()(int row, int col);
    const float& operator()(int row, int col) const;

    // Basic matrix operations
    Matrix4x4 operator*(const Matrix4x4& other) const;
    Matrix4x4 operator*(float scalar) const;
    Matrix4x4 operator+(const Matrix4x4& other) const;

    // Transformation matrices
    Matrix4x4 Transpose() const;
    static Matrix4x4 Identity();
    static Matrix4x4 Translation(float x, float y, float z);
    static Matrix4x4 Scale(float x, float y, float z);
    static Matrix4x4 RotationX(float angle);
    static Matrix4x4 RotationY(float angle);
    static Matrix4x4 RotationZ(float angle);
    static Matrix4x4 Ortho(float left, float right, float bottom, float top, float nearVal, float farVal);

    // Determinant and Inverse
    float Determinant() const;
    Matrix4x4 Inverse() const;

    // Multiply Matrix4x4 by Vector3
    Vector3 operator*(const Vector3& v) const;

    // Utility
    void Print() const;

private:
    std::array<float, 16> m_Elements;

    // Helper methods for cofactor-based inversion
    float Cofactor(int row, int col) const;
    Matrix4x4 CofactorMatrix() const;
};

#endif // MATRIX4X4_H
