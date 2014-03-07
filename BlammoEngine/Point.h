/**
 * Point.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __POINT_H__
#define __POINT_H__

#include "BasicIncludes.h"
#include "Algebra.h"

//class Vector2D;
//class Vector3D;
class Matrix4x4;

#include "Vector.h"

// Simple 2D Point --------------------------------
class Point2D {
private:
    float v_[2];

public:
    Point2D() {
        v_[0] = 0.0;
        v_[1] = 0.0;
    }

    Point2D(float x, float y) { 
        v_[0] = x;
        v_[1] = y;
    }

    Point2D(const Point2D& other) {
        v_[0] = other.v_[0];
        v_[1] = other.v_[1];
    }



    float MagnitudeAsVector() const {
        return sqrtf(v_[0]*v_[0] + v_[1]*v_[1]);
    }

    void NormalizeAsVector() {
        float magnitude = this->MagnitudeAsVector();
        if (magnitude == 0) {
            return;
        }
        this->v_[0] /= magnitude;
        this->v_[1] /= magnitude;
    }


    const float* begin() const {
        return v_;
    }

    Point2D& operator =(const Point2D& other) {
        v_[0] = other.v_[0];
        v_[1] = other.v_[1];
        return *this;
    }

    Point2D& operator/=(float f) {
        v_[0] /= f;
        v_[1] /= f;
        return *this;
    }

    Point2D& operator-=(const Vector2D& v) {
        v_[0] -= v[0];
        v_[1] -= v[1];
        return *this;
    }

    Point2D& operator+=(const Vector2D& v) {
        v_[0] += v[0];
        v_[1] += v[1];
        return *this;
    }

    float& operator[](size_t idx) {
        assert(idx < 2);
        return v_[ idx ];
    }

    float operator[](size_t idx) const {
        assert(idx < 2);
        return v_[ idx ];
    }

    // Robust equivalence
    bool operator==(const Point2D &other) {
        return 	fabs(v_[0] - other[0]) < EPSILON &&
            fabs(v_[1] - other[1]) < EPSILON;
    }

    bool operator!=(const Point2D& other) {
        return !(*this == other);
    }

    static Point2D GetMidPoint(const Point2D& p1, const Point2D& p2) {
        return Point2D((p1[0] + p2[0]) / 2.0f, (p1[1] + p2[1]) / 2.0f);
    }

    static float Distance(const Point2D& p1, const Point2D& p2) {
        return sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1]));
    }
    static float SqDistance(const Point2D& p1, const Point2D& p2) {
        return (p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1]);
    }

    friend bool operator!=(const Point2D& a, const Point2D& b);
};

// 2D Point output function for debugging purposes
inline std::ostream& operator <<(std::ostream& os, const Point2D& p)
{
    return os << "p<" << p[0] << "," << p[1] << ">";
}


inline Point2D operator +(const Point2D& a, const Vector2D& b) {
    return Point2D(a[0]+b[0], a[1]+b[1]);
}
inline Point2D operator +(const Point2D& a, const Point2D& b) {
    return Point2D(a[0]+b[0], a[1]+b[1]);
}
inline Point2D operator +(const Vector2D& a, const Point2D& b) {
    return Point2D(a[0]+b[0], a[1]+b[1]);
}
inline Vector2D operator -(const Point2D& a, const Point2D& b) {
    return Vector2D(a[0]-b[0], a[1]-b[1]);
}
inline Point2D operator -(const Point2D& a, const Vector2D& b) {
    return Point2D(a[0]-b[0], a[1]-b[1]);
}
inline Point2D operator *(float f, const Point2D& p) {
    return Point2D(f*p[0], f*p[1]);
}

inline bool operator!=(const Point2D& a, const Point2D& b) {
    return a[0] != b[0] && a[1] != b[1];
}

// Simple 3D Point --------------------------------
class Point3D {

private:
    float v_[3];

public:
    Point3D() {
        v_[0] = 0.0;
        v_[1] = 0.0;
        v_[2] = 0.0;
    }

    Point3D(float x, float y, float z) { 
        v_[0] = x;
        v_[1] = y;
        v_[2] = z;
    }

    Point3D(const Point2D& p, float z) {
        v_[0] = p[0];
        v_[1] = p[1];
        v_[2] = z;		
    }

    Point3D(const Vector2D& v, float z) {
        v_[0] = v[0];
        v_[1] = v[1];
        v_[2] = z;	
    }

    explicit Point3D(const Point2D& p) {
        v_[0] = p[0];
        v_[1] = p[1];
        v_[2] = 0.0f;		
    }

    Point3D(const Point3D& other) {
        v_[0] = other.v_[0];
        v_[1] = other.v_[1];
        v_[2] = other.v_[2];
    }

    Point3D& operator =(const Point3D& other) {
        v_[0] = other.v_[0];
        v_[1] = other.v_[1];
        v_[2] = other.v_[2];
        return *this;
    }

    const float* begin() const {
        return v_;
    }

    float& operator[](size_t idx) {
        assert(idx < 3);
        return v_[ idx ];
    }

    float operator[](size_t idx) const {
        assert(idx < 3);
        return v_[ idx ];
    }

    inline Point3D& operator+=(const Vector3D& other) {
        this->v_[0] += other[0];
        this->v_[1] += other[1];
        this->v_[2] += other[2];
        return *this;
    }

    inline Point3D& operator/=(float value) {
        this->v_[0] /= value;
        this->v_[1] /= value;
        this->v_[2] /= value;
        return *this;
    }

    // Robust equivalence
    inline bool operator==(const Point3D &other) {
        return 	fabs(v_[0] - other[0]) < EPSILON &&
            fabs(v_[1] - other[1]) < EPSILON &&
            fabs(v_[2] - other[2]) < EPSILON;
    }

    inline bool operator!=(const Point3D &other) {
        return !(*this == other);
    }

    static Point3D GetMidPoint(const Point3D& p1, const Point3D& p2) {
        return Point3D((p1[0] + p2[0]) / 2.0f, (p1[1] + p2[1]) / 2.0f, (p1[2] + p2[2]) / 2.0f);
    }

    Point2D ToPoint2D() const {
        return Point2D(this->v_[0], this->v_[1]);
    }

    friend bool operator<=(const Point3D& a, const Point3D& b);
};

inline std::ostream& operator <<(std::ostream& os, const Point3D& p) {
    return os << "p<" << p[0] << "," << p[1] << "," << p[2] << ">";
}

inline Point3D operator *(const float f, const Point3D& p) {
    return Point3D(f*p[0], f*p[1], f*p[2]);
}

inline Point3D operator /(const Point3D& p, const float f) {
    return Point3D(p[0]/f, p[1]/f, p[2]/f);
}

inline bool operator<=(const Point3D& a, const Point3D& b) {
    return (a.v_[0] <= b.v_[0] && a.v_[1] <= b.v_[1] && a.v_[2] <= b.v_[2]);
}


Point3D operator *(const Matrix4x4& M, const Point3D& p);
Point3D operator +(const Point3D& a, const Vector3D& b);
Vector3D operator -(const Point3D& a, const Point3D& b);
Point3D operator -(const Point3D& a, const Vector3D& b);

#endif