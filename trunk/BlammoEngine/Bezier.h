/**
 * Bezier.h
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

#ifndef __BEZIER_H__
#define __BEZIER_H__

#include "BasicIncludes.h"
#include "Point.h"

class Bezier {
public:
    Bezier();
    ~Bezier() {};

    void SetDrawPosition(float x, float y);
    void SetDrawPosition(const Point2D& pt);
    void MoveToCubicBezier(bool isRelative, float startControlPtX, float startControlPtY, float endControlPtX, float endControlPtY, float endPtX, float endPtY);
    void MoveToCubicBezier(bool isRelative, const Point2D& startControlPt, const Point2D& endControlPt, const Point2D& endPt);
    void Normalize();

    void UniformSample(int numSamples);
    void SmartSample(int numSamples);

    bool IsValid() const;

    void DrawGL(int beginIdx, int endIdx) const;
    void DrawGL(double t) const;
    
    int GetNumSamplePoints() const;

private:
    static const int NUM_FACTORIALS_IN_TABLE = 33;
    static const double FACTORIAL_TABLE[NUM_FACTORIALS_IN_TABLE];

    Point2D currDrawPos;
    std::vector<std::vector<Point2D> > controlPointGrps;
    std::vector<Point2D> samplePointsOnCurve;

    // Cached display lists for a sampled curve, the lists are relevant to every 50 points
    //std::vector<GLUint> sampleCurveDispLists;

    void GetNumericLimitsOfCurve(float& maxX, float& maxY, float& minX, float& minY) const;
    float GetMaxExtentsOfCurve() const;
    float GetMaxExtentsOfControlPtGrp(int idx) const;
    float GetCurveLength() const;
    float GetCurveLengthOfControlPtGrp(int idx) const;

    static void GeneratePointsAlongBezier2D(const std::vector<Point2D>& bezierControlPts, int numPointsToGenerate,
                                            std::vector<Point2D>& generatedPointsOnCurve);

    static double Ni(int n, int i);
    static double Bernstein(int n, int i, double t);
    static double Factorial(int n);
};

inline void Bezier::SetDrawPosition(float x, float y) {
    this->currDrawPos[0] = x;
    this->currDrawPos[1] = y;
}

inline void Bezier::SetDrawPosition(const Point2D& pt) {
    this->SetDrawPosition(pt[0], pt[1]);
}   

inline bool Bezier::IsValid() const {
    return this->controlPointGrps.size() >= 1;
}

inline int Bezier::GetNumSamplePoints() const {
    return static_cast<int>(this->samplePointsOnCurve.size());
}

inline double Bezier::Factorial(int n) {
    if (n < 0 || n > 32) { assert(false); }
    return FACTORIAL_TABLE[n];
}

#endif // __BEZIER_H__