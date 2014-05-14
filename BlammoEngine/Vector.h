/**
 * Vector.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "BasicIncludes.h"
#include "Algebra.h"

class Matrix4x4;

// 2D Vector -----------------------------------------------------
class Vector2D {
private:
    float values[2];

public:
    Vector2D() {
        values[0] = 0.0f;
        values[1] = 0.0f;
    }

    explicit Vector2D(float f) {
        values[0] = values[1] = f;
    }

    Vector2D(float x, float y) { 
        values[0] = x;
        values[1] = y;
    }

    Vector2D(const Vector2D& other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
    }

    Vector2D& operator =(const Vector2D& other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        return *this;
    }

    bool IsZero() const;

    const float* begin() const {
        return values;
    }

    float& operator[](size_t idx) {
        assert(idx < 2);
        return values[ idx ];
    }

    float operator[](size_t idx) const {
        assert(idx < 2);
        return values[ idx ];
    }

    Vector2D& operator*=(float f) {
        values[0] *= f;
        values[1] *= f;
        return (*this);
    }
    Vector2D& operator+=(const Vector2D& v) {
        values[0] += v[0];
        values[1] += v[1];
        return (*this);
    }

    static float Magnitude(const Vector2D& v1) {
        return sqrtf(v1[0]*v1[0] + v1[1]*v1[1]);
    }

    float SqrMagnitude() const {
        return Vector2D::Dot(*this, *this);
    }

    float Magnitude() const {
        return sqrtf(this->SqrMagnitude());
    }

    static float Dot(const Vector2D& v1, const Vector2D& v2) {
        return v1[0]*v2[0] + v1[1]*v2[1];
    }

    static Vector2D Normalize(const Vector2D& v) {
        Vector2D temp(v);
        temp.Normalize();
        return temp;
    }

    static Vector2D Rotate(float angInDegs, const Vector2D& v);
    void Rotate(float angInDegs);

    void Normalize() {
        float magnitude = Vector2D::Magnitude(*this);
        if (magnitude == 0) { assert(false); return; }
        this->values[0] /= magnitude;
        this->values[1] /= magnitude;
    }

};

inline std::ostream& operator <<(std::ostream& os, const Vector2D& v) {
    return os << "v<" << v[0] << "," << v[1] << ">";
}

inline Vector2D operator *(float s, const Vector2D& v) {
    return Vector2D(s*v[0], s*v[1]);
}

inline Vector2D operator /(const Vector2D& v, float d) {
    assert(d != 0);
    return Vector2D(v[0]/d, v[1]/d);
}

inline Vector2D operator +(const Vector2D& a, const Vector2D& b) {
    return Vector2D(a[0]+b[0], a[1]+b[1]);
}

inline Vector2D operator -(const Vector2D& a, const Vector2D& b) {
    return Vector2D(a[0]-b[0], a[1]-b[1]);
}

inline Vector2D operator -(const Vector2D& a) {
    return Vector2D(-a[0], -a[1]);
}

inline bool operator ==(const Vector2D& a, const Vector2D& b) {
    return (abs(a[0] - b[0]) < EPSILON) && (abs(a[1] - b[1]) < EPSILON);
}

inline bool operator !=(const Vector2D& a, const Vector2D& b) {
    return !(a==b);
}

inline Vector2D Reflect(const Vector2D& v, const Vector2D& n) {
    Vector2D newVel = v - 2.0f * Vector2D::Dot(n, v) * n;
    return newVel;
}

inline Vector2D Vector2D::Rotate(float angInDegs, const Vector2D& v) {
    float angInRads = Trig::degreesToRadians(angInDegs);
    return Vector2D(cosf(angInRads) * v[0] - sinf(angInRads) * v[1], sinf(angInRads) * v[0] + cosf(angInRads) * v[1]);
}

inline void Vector2D::Rotate(float angInDegs) {
    float angInRads = Trig::degreesToRadians(angInDegs);

    float c = cosf(angInRads);
    float s = sinf(angInRads);
    float x = c * values[0] - s * values[1];
    float y = s * values[0] + c * values[1];

    values[0] = x;
    values[1] = y;
}

inline bool Vector2D::IsZero() const {
    static Vector2D zeroVec2D(0,0);
    return (*this == zeroVec2D);
}

// 3D Vector -----------------------------------------------------
class Vector3D {
private:
    float values[3];

public:
    Vector3D() {
        values[0] = 0.0f;
        values[1] = 0.0f;
        values[2] = 0.0f;
    }

    explicit Vector3D(float f) {
        values[0] = values[1] = values[2] = f;
    }

    Vector3D(float x, float y, float z) { 
        values[0] = x;
        values[1] = y;
        values[2] = z;
    }

    Vector3D(const Vector2D& v, float z) {
        values[0] = v[0];
        values[1] = v[1];
        values[2] = z;
    }

    explicit Vector3D(const Vector2D& v) {
        values[0] = v[0];
        values[1] = v[1];
        values[2] = 0.0f;
    }

    Vector3D(const Vector3D& other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
    }

    Vector3D& operator =(const Vector3D& other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
        return *this;
    }

    const float* begin() const {
        return values;
    }

    float& operator[](size_t idx) {
        assert(idx < 3);
        return values[idx];
    }

    float operator[](size_t idx) const {
        assert(idx < 3);
        return values[idx];
    }

    Vector3D& operator*=(float s) {
        values[0] *= s;
        values[1] *= s;
        values[2] *= s;
        return *this;
    }
    Vector3D& operator/=(float s) {
        values[0] /= s;
        values[1] /= s;
        values[2] /= s;
        return *this;
    }
    Vector3D& operator+=(const Vector3D& otherVec) {
        values[0] += otherVec[0];
        values[1] += otherVec[1];
        values[2] += otherVec[2];
        return *this;
    }

    bool IsZero() const;

    float dot(const Vector3D& other) const {
        return values[0]*other.values[0] + values[1]*other.values[1] + values[2]*other.values[2];
    }	
    float length2() const {
        return values[0]*values[0] + values[1]*values[1] + values[2]*values[2];
    }

    float length() const {
        return static_cast<float>(sqrt(length2()));
    }

    void CondenseAndNormalizeToLargestComponent() {
        int largestIdx = 0;
        float largestAbsValue = 0;
        float largestValue = 0;
        float tempAbsValue;

        for (int i = 0; i < 3; i++) {
            tempAbsValue = fabs(this->values[i]);
            if (tempAbsValue > largestAbsValue) {
                largestAbsValue = tempAbsValue;
                largestValue = this->values[i];
                largestIdx = i;
            }
        }
        for (int i = 0; i < 3; i++) {
            if (i == largestIdx) {
                this->values[i] = NumberFuncs::SignOf(largestValue);
            }
            else {
                this->values[i] = 0;
            }
        }
    }

    void Abs() {
        this->values[0] = fabs(this->values[0]);
        this->values[1] = fabs(this->values[1]);
        this->values[2] = fabs(this->values[2]);
    }

    Vector2D ToVector2D() {
        return Vector2D(this->values[0], this->values[1]);
    }

    void Normalize() {
        float magnitude = this->length();
        if (magnitude < 0.000000001f) {
            assert(false);
            this->values[0] = this->values[1] = this->values[2] = 0;			
        }
        else {
            this->values[0] /= magnitude;
            this->values[1] /= magnitude;
            this->values[2] /= magnitude;
        }
    }

    Vector3D cross(const Vector3D& other) const {
        return Vector3D(values[1]*other[2] - values[2]*other[1],
            values[2]*other[0] - values[0]*other[2],
            values[0]*other[1] - values[1]*other[0]);
    }

    static Vector3D cross(const Vector3D& v1, const Vector3D& v2) {
        return Vector3D(v1[1] * v2[2] - v1[2] * v2[1],
            v1[2] * v2[0] - v1[0] * v2[2],
            v1[0] * v2[1] - v1[1] * v2[0]);
    }

    static Vector3D cross(const Vector2D& v1, const Vector2D& v2) {
        return Vector3D(0, 0, v1[0] * v2[1] - v1[1] * v2[0]);
    }

    static Vector3D Normalize(const Vector3D& v) {
        Vector3D temp(v);
        temp.Normalize();
        return temp;
    }

    /**
    * Finds the angle between two given vectors, in radians.
    * Precondition: v1 and v2 MUST be normalized for this to work!!
    * Returns: angle between v1 and v2 in radians.
    */
    static float AngleBetweenInRadians(const Vector3D& v1, const Vector3D& v2) {
        return acosf(v1.dot(v2));
    }

    /**
    * To create an 'imperfect' perpendicular vector to the one given we
    * find the smallest index coordinate component and set it to zero,
    * then we flip the other two coordinates and negate the first.
    * Returns: An 'imperfect' perpendicular vector to the given v.
    **/
    static Vector3D MollerHughesPerpendicular(const Vector3D& v) {
        Vector3D perpendicularMH = v;
        int smallestIndex;

        // First we take the smallest coordinate component and set it to zero
        if (v[0] < v[1]) {
            if (v[0] < v[2]) {
                smallestIndex = 0;
                perpendicularMH[1] = -v[2];
                perpendicularMH[2] =  v[1]; 
            }
            else {
                smallestIndex = 2;
                perpendicularMH[0] = -v[1];
                perpendicularMH[1] =  v[0]; 
            }
        }
        else {
            if (v[1] < v[2]) {
                smallestIndex = 1;
                perpendicularMH[0] = -v[2];
                perpendicularMH[2] =  v[0]; 
            }
            else {
                smallestIndex = 2;
                perpendicularMH[0] = -v[1];
                perpendicularMH[1] =  v[0]; 
            }
        }

        perpendicularMH[smallestIndex] = 0;
        return perpendicularMH;
    }


    // Return spherical coords (r, theta (the angle to z-axis), phi (the angle to x-axis))
    static Vector3D ToSphericalFromCartesian(const Vector3D& v) {
        Vector3D sphericalCoords;
        sphericalCoords[0] = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
        sphericalCoords[1] = acosf(v[2] / sphericalCoords[0]);
        sphericalCoords[2] = atan2f(v[1], v[0]);
        return sphericalCoords;
    }

    static Vector3D ToCartesianFromSpherical(const Vector3D& v) {
        Vector3D cartesianCoords;
        float rTimesSinTheta = v[2] * sinf(v[1]);
        cartesianCoords[0] = rTimesSinTheta * cosf(v[2]);
        cartesianCoords[1] = rTimesSinTheta * sinf(v[2]);
        cartesianCoords[2] = v[2] * cosf(v[1]);
        return cartesianCoords;
    }


    static float Dot(const Vector3D& v1, const Vector3D& v2) {
        return v1.dot(v2);
    }
};

inline std::ostream& operator <<(std::ostream& os, const Vector3D& v) {
    return os << "v<" << v[0] << "," << v[1] << "," << v[2] << ">";
}

inline Vector3D operator *(float s, const Vector3D& v) {
    return Vector3D(s*v[0], s*v[1], s*v[2]);
}

inline Vector3D operator /(const Vector3D& v, float d) {
    assert(d != 0);
    return Vector3D(v[0]/d, v[1]/d, v[2]/d);
}

inline Vector3D operator +(const Vector3D& a, const Vector3D& b) {
    return Vector3D(a[0]+b[0], a[1]+b[1], a[2]+b[2]);
}

inline Vector3D operator -(const Vector3D& a, const Vector3D& b) {
    return Vector3D(a[0]-b[0], a[1]-b[1], a[2]-b[2]);
}

inline Vector3D operator -(const Vector3D& a) {
    return Vector3D(-a[0], -a[1], -a[2]);
}

inline Vector3D cross(const Vector3D& a, const Vector3D& b) {
    return a.cross(b);
}

inline bool operator ==(const Vector3D& a, const Vector3D& b) {
    return (abs(a[0] - b[0]) < EPSILON) && (abs(a[1] - b[1]) < EPSILON) && (abs(a[2] - b[2]) < EPSILON);
}

inline bool operator !=(const Vector3D& a, const Vector3D& b) {
    return !(a==b);
}

inline bool Vector3D::IsZero() const {
    static Vector3D zeroVec3D(0,0,0);
    return (*this == zeroVec3D);
}

Vector3D operator *(const Matrix4x4& M, const Vector3D& v);
Vector3D transNorm(const Matrix4x4& M, const Vector3D& n);


// 4D Vector -----------------------------------------------------
class Vector4D {

private:
    float values[4];

public:
    Vector4D() {
        values[0] = 0.0;
        values[1] = 0.0;
        values[2] = 0.0;
        values[3] = 0.0;
    } 
    Vector4D(float x, float y, float z, float w) { 
        values[0] = x;
        values[1] = y;
        values[2] = z;
        values[3] = w;
    }

    Vector4D(const Vector4D& other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
        values[3] = other.values[3];
    }

    Vector4D& operator =(const Vector4D& other) {
        values[0] = other.values[0];
        values[1] = other.values[1];
        values[2] = other.values[2];
        values[3] = other.values[3];
        return *this;
    }

    float& operator[](size_t idx) {
        assert(idx < 4);
        return values[ idx ];
    }

    float operator[](size_t idx) const {
        assert(idx < 4);
        return values[ idx ];
    }

    static float Dot(const Vector4D& v1, const Vector4D& v2) {
        return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] + v1[3]*v2[3];
    }

};

inline std::ostream& operator <<(std::ostream& os, const Vector4D& v) {
    return os << "v<" << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << ">";
}

inline Vector4D operator *(float s, const Vector4D& v) {
    return Vector4D(s*v[0], s*v[1], s*v[2], s*v[3]);
}

inline Vector4D operator /(const Vector4D& v, float d) {
    assert(d != 0);
    return Vector4D(v[0]/d, v[1]/d, v[2]/d, v[3]/d);
}

inline Vector4D operator +(const Vector4D& a, const Vector4D& b) {
    return Vector4D(a[0]+b[0], a[1]+b[1], a[2]+b[2], a[3]+b[3]);
}

inline Vector4D operator -(const Vector4D& a, const Vector4D& b) {
    return Vector4D(a[0]-b[0], a[1]-b[1], a[2]-b[2], a[3]-b[3]);
}

#endif