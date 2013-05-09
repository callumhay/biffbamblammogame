/**
 * Bezier.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BEZIER_H__
#define __BEZIER_H__

#include "BasicIncludes.h"
#include "Point.h"

class Bezier {
public:

    static void GeneratePointsAlongBezier2D(const std::vector<Point2D>& bezierControlPts, int numPointsToGenerate,
                                         std::vector<Point2D>& generatedPointsOnCurve);
    

private:
    static const int NUM_FACTORIALS_IN_TABLE = 33;
    static const double FACTORIAL_TABLE[NUM_FACTORIALS_IN_TABLE];


    Bezier() {};
    ~Bezier() {};

    static double Ni(int n, int i);
    static double Bernstein(int n, int i, double t);
    static double Factorial(int n);
};


inline double Bezier::Factorial(int n) {
    if (n < 0 || n > 32) { assert(false); }
    return FACTORIAL_TABLE[n];
}

#endif // __BEZIER_H__