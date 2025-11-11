/*!****************************************************************
\file: Matrix4x4.cpp
\author: Jeremy Lim Ting Jie, jeremytingjie.lim , 2301370
\brief:  This file implements the Matrix4x4 class, providing functionality
for 4x4 matrix operations crucial for 2D transformations, including scaling, rotation, translation, and matrix inversion.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************!*/
#include "Matrix4x4.h"

//  Vector3 multiplication
Vector3 Matrix4x4::operator*(const Vector3& v) const
{
    Vector3 result;

    // Perform the multiplication
    result.x = (*this)(0, 0) * v.x + (*this)(0, 1) * v.y + (*this)(0, 2) * v.z + (*this)(0, 3);
    result.y = (*this)(1, 0) * v.x + (*this)(1, 1) * v.y + (*this)(1, 2) * v.z + (*this)(1, 3);
    result.z = (*this)(2, 0) * v.x + (*this)(2, 1) * v.y + (*this)(2, 2) * v.z + (*this)(2, 3);

    return result;
}

// Default constructor: identity matrix
Matrix4x4::Matrix4x4()
{
    *this = Identity();
}

// Constructor with custom values
Matrix4x4::Matrix4x4(const std::array<float, 16>& values) : m_Elements(values) {}

// Access operators
float& Matrix4x4::operator()(int row, int col)
{
    return m_Elements[row * 4 + col];
}

const float& Matrix4x4::operator()(int row, int col) const
{
    return m_Elements[row * 4 + col];
}

// Identity matrix
Matrix4x4 Matrix4x4::Identity()
{
    return Matrix4x4({
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        });
}

// Matrix multiplication
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
    Matrix4x4 result;

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result(row, col) = 0;
            for (int k = 0; k < 4; ++k)
            {
                result(row, col) += (*this)(row, k) * other(k, col);
            }
        }
    }

    return result;
}

// Scalar multiplication
Matrix4x4 Matrix4x4::operator*(float scalar) const
{
    Matrix4x4 result;
    for (int i = 0; i < 16; ++i)
    {
        result.m_Elements[i] = m_Elements[i] * scalar;
    }
    return result;
}

// Matrix Addition
Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const
{
    Matrix4x4 result;

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result(row, col) = 0;
            for (int k = 0; k < 4; ++k)
            {
                result(row, col) += (*this)(row, k) * other(k, col);
            }
        }
    }

    return result;
}

// Transpose the matrix
Matrix4x4 Matrix4x4::Transpose() const {
    Matrix4x4 result;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            result(col, row) = (*this)(row, col); // Swap row and column indices
        }
    }
    return result;
}

// Determinant using the cofactor expansion
float Matrix4x4::Determinant() const
{
    return (*this)(0, 0) * Cofactor(0, 0)
        + (*this)(0, 1) * Cofactor(0, 1)
        + (*this)(0, 2) * Cofactor(0, 2)
        + (*this)(0, 3) * Cofactor(0, 3);
}

// Compute cofactor for a specific element
float Matrix4x4::Cofactor(int row, int col) const
{
    Matrix4x4 subMatrix;
    int subRow = 0;

    // Create the submatrix (minor)
    for (int i = 0; i < 4; ++i)
    {
        if (i == row) continue;
        int subCol = 0;
        for (int j = 0; j < 4; ++j)
        {
            if (j == col) continue;
            subMatrix(subRow, subCol) = (*this)(i, j);
            subCol++;
        }
        subRow++;
    }

    // Return determinant of 3x3 submatrix with alternating signs
    float det3x3 = subMatrix(0, 0) * (subMatrix(1, 1) * subMatrix(2, 2) - subMatrix(1, 2) * subMatrix(2, 1))
        - subMatrix(0, 1) * (subMatrix(1, 0) * subMatrix(2, 2) - subMatrix(1, 2) * subMatrix(2, 0))
        + subMatrix(0, 2) * (subMatrix(1, 0) * subMatrix(2, 1) - subMatrix(1, 1) * subMatrix(2, 0));

    // Apply alternating sign
    return ((row + col) % 2 == 0 ? 1 : -1) * det3x3;
}

// Matrix inverse using cofactor expansion and determinant
Matrix4x4 Matrix4x4::Inverse() const
{
    float det = Determinant();
    if (std::abs(det) < 1e-6f)
    {
        throw std::runtime_error("Matrix is singular and cannot be inverted.");
    }

    // Compute cofactor matrix
    Matrix4x4 cofactorMatrix = CofactorMatrix();

    // Compute the adjugate matrix (transpose of cofactor matrix)
    Matrix4x4 adjugate;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            adjugate(i, j) = cofactorMatrix(j, i);
        }
    }

    // Inverse is adjugate matrix divided by determinant
    return adjugate * (1.0f / det);
}

// Compute the cofactor matrix
Matrix4x4 Matrix4x4::CofactorMatrix() const
{
    Matrix4x4 cofactorMatrix;
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            cofactorMatrix(row, col) = Cofactor(row, col);
        }
    }
    return cofactorMatrix;
}

// Static transformation methods
Matrix4x4 Matrix4x4::Translation(float x, float y, float z)
{
    return Matrix4x4({
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
        });
}
//Scale Matrix
Matrix4x4 Matrix4x4::Scale(float x, float y, float z)
{
    return Matrix4x4({
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
        });
}
//Rotate X axis
Matrix4x4 Matrix4x4::RotationX(float angle)
{
    float rad = angle * (3.14159265359f / 180.0f);
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return Matrix4x4({
        1, 0,    0,     0,
        0, cosA, -sinA, 0,
        0, sinA,  cosA, 0,
        0, 0,    0,     1
        });
}
//Rotate Y axis
Matrix4x4 Matrix4x4::RotationY(float angle)
{
    float rad = angle * (3.14159265359f / 180.0f);
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return Matrix4x4({
        cosA, 0, sinA, 0,
        0,    1, 0,    0,
        -sinA, 0, cosA, 0,
        0,    0, 0,    1
        });
}
//Rotate Z axis
Matrix4x4 Matrix4x4::RotationZ(float angle)
{
    float rad = angle * (3.14159265359f / 180.0f);
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    return Matrix4x4({
        cosA, -sinA, 0, 0,
        sinA,  cosA, 0, 0,
        0,     0,    1, 0,
        0,     0,    0, 1
        });
}

Matrix4x4 Matrix4x4::Ortho(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    return Matrix4x4({
        2.f / (right - left), 0.f, 0.f, -(right + left) / (right - left),
        0.f, 2.f / (top - bottom), 0.f, -(top + bottom) / (top - bottom),
        0.f, 0.f, -2.f / (farVal - nearVal), -(farVal + nearVal) / (farVal - nearVal),
        0.f, 0.f, 0.f, 1.f
        });
}
#ifdef _LOGGING
// Print the matrix
void Matrix4x4::Print() const
{
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            std::cout << (*this)(row, col) << " ";
        }
        std::cout << std::endl;
    }
}
#endif // _LOGGING
