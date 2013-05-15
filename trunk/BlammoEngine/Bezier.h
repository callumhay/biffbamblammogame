/**
 * Bezier.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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