/**
 * Bezier.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "Bezier.h"
#include "Algebra.h"

const double Bezier::FACTORIAL_TABLE[] = {
   1.0,
   1.0,
   2.0,
   6.0,
   24.0,
   120.0,
   720.0,
   5040.0,
   40320.0,
   362880.0,
   3628800.0,
   39916800.0,
   479001600.0,
   6227020800.0,
   87178291200.0,
   1307674368000.0,
   20922789888000.0,
   355687428096000.0,
   6402373705728000.0,
   121645100408832000.0,
   2432902008176640000.0,
   51090942171709440000.0,
   1124000727777607680000.0,
   25852016738884976640000.0,
   620448401733239439360000.0,
   15511210043330985984000000.0,
   403291461126605635584000000.0,
   10888869450418352160768000000.0,
   304888344611713860501504000000.0,
   8841761993739701954543616000000.0,
   265252859812191058636308480000000.0,
   8222838654177922817725562880000000.0,
   263130836933693530167218012160000000.0
};

Bezier::Bezier() : currDrawPos(0,0) {
}

void Bezier::MoveToCubicBezier(bool isRelative,
                               float startControlPtX, float startControlPtY, 
                               float endControlPtX, float endControlPtY, 
                               float endPtX, float endPtY) {

    
   
    Point2D absoluteEndPt = this->currDrawPos + Vector2D(endPtX, endPtY);
    std::vector<Point2D> subCurve;
    subCurve.reserve(4);
    subCurve.push_back(this->currDrawPos);

    if (isRelative) {
        subCurve.push_back(this->currDrawPos + Vector2D(startControlPtX, startControlPtY));
        subCurve.push_back(this->currDrawPos + Vector2D(endControlPtX, endControlPtY));
        subCurve.push_back(absoluteEndPt);
    }
    else {
        subCurve.push_back(Point2D(startControlPtX, startControlPtY));
        subCurve.push_back(Point2D(endControlPtX, endControlPtY));
        subCurve.push_back(Point2D(endPtX, endPtY));
    }

    this->controlPointGrps.push_back(subCurve);
    this->SetDrawPosition(absoluteEndPt);
}

void Bezier::MoveToCubicBezier(bool isRelative, const Point2D& startControlPt, const Point2D& endControlPt, const Point2D& endPt) {
    this->MoveToCubicBezier(isRelative, startControlPt[0], startControlPt[1],
        endControlPt[0], endControlPt[1], endPt[0], endPt[1]);
}

void Bezier::Normalize() {
    float maxSizeDiff = this->GetMaxExtentsOfCurve();
    for (int i = 0; i < static_cast<int>(this->controlPointGrps.size()); i++) {
        for (int j = 0; j < static_cast<int>(this->controlPointGrps[i].size()); j++) {
            this->controlPointGrps[i][j] /= maxSizeDiff;
        }
    }

    if (!this->samplePointsOnCurve.empty()) {
        for (int i = 0; i < static_cast<int>(this->samplePointsOnCurve.size()); i++) {
            this->samplePointsOnCurve[i] /= maxSizeDiff;
        }
    }   
}

void Bezier::UniformSample(int numSamples) {
    assert(numSamples > 0);
    
    if (!this->IsValid()) {
        return;
    }
    
    int samplesPerSegment = numSamples / static_cast<int>(this->controlPointGrps.size());
    assert(samplesPerSegment > 1);

    this->samplePointsOnCurve.clear();
    this->samplePointsOnCurve.reserve(numSamples);

    for (int i = 0; i < static_cast<int>(this->controlPointGrps.size()); i++) {
        
        std::vector<Point2D> generatedPoints;
        generatedPoints.reserve(samplesPerSegment);
        Bezier::GeneratePointsAlongBezier2D(this->controlPointGrps[i], samplesPerSegment, generatedPoints);
        this->samplePointsOnCurve.insert(this->samplePointsOnCurve.end(), generatedPoints.begin(), generatedPoints.end());
    }
}

void Bezier::SmartSample(int numSamples) {
    assert(numSamples > 0);
    
    if (!this->IsValid()) {
        return;
    }

    const float totalCurveLength = this->GetCurveLength();
 
    std::vector<float> weightings;
    weightings.reserve(this->controlPointGrps.size());
    for (int i = 0; i < static_cast<int>(this->controlPointGrps.size()); i++) {
        float currWeighting = this->GetCurveLengthOfControlPtGrp(i) / totalCurveLength;
        assert(currWeighting > 0.0 && currWeighting <= 1.0);
        weightings.push_back(currWeighting);
    }

    this->samplePointsOnCurve.clear();
    this->samplePointsOnCurve.reserve(numSamples);

    for (int i = 0; i < static_cast<int>(this->controlPointGrps.size()); i++) {
        
        int samplesPerSegment = static_cast<int>(numSamples * weightings[i]);
        samplesPerSegment = std::max<int>(samplesPerSegment, 2);

        std::vector<Point2D> generatedPoints;
        generatedPoints.reserve(samplesPerSegment);

        Bezier::GeneratePointsAlongBezier2D(this->controlPointGrps[i], samplesPerSegment, generatedPoints);
        this->samplePointsOnCurve.insert(this->samplePointsOnCurve.end(), generatedPoints.begin(), generatedPoints.end());
    }

}

void Bezier::DrawGL(int beginIdx, int endIdx) const {
    
    assert(beginIdx <= endIdx);
    assert(beginIdx >= 0 && beginIdx < this->GetNumSamplePoints());
    assert(endIdx >= 0 && endIdx < this->GetNumSamplePoints());

    if (this->GetNumSamplePoints() < 2 || beginIdx == endIdx) {
        return;
    }

    glBegin(GL_LINE_STRIP);
    
    for (int i = beginIdx; i <= endIdx; i++) {
        const Point2D& samplePt = this->samplePointsOnCurve[i];
        glVertex2f(samplePt[0], samplePt[1]);
    }

    glEnd();
}

void Bezier::DrawGL(double t) const {
    assert(t >= 0.0 && t <= 1.0);

    int endIdx = static_cast<int>(t * (this->GetNumSamplePoints()-1));
    this->DrawGL(0, endIdx);
}

void Bezier::GetNumericLimitsOfCurve(float& maxX, float& maxY, float& minX, float& minY) const {

    maxX = -std::numeric_limits<float>::max();
    maxY = -std::numeric_limits<float>::max();
    minX =  std::numeric_limits<float>::max();
    minY =  std::numeric_limits<float>::max();

    for (int i = 0; i < static_cast<int>(this->controlPointGrps.size()); i++) {
        for (int j = 0; j < static_cast<int>(this->controlPointGrps[i].size()); j++) {
            maxX = std::max<float>(maxX, this->controlPointGrps[i][j][0]);
            maxY = std::max<float>(maxY, this->controlPointGrps[i][j][1]);
            minX = std::min<float>(minX, this->controlPointGrps[i][j][0]);
            minY = std::min<float>(minY, this->controlPointGrps[i][j][1]);
        }
    }
}

float Bezier::GetMaxExtentsOfCurve() const {
    float maxX, maxY, minX, minY;
    this->GetNumericLimitsOfCurve(maxX, maxY, minX, minY);
    return std::max(fabs(maxX - minX), fabs(maxY - minY));
}

float Bezier::GetMaxExtentsOfControlPtGrp(int idx) const {

    float maxX = -std::numeric_limits<float>::max();
    float maxY = -std::numeric_limits<float>::max();
    float minX =  std::numeric_limits<float>::max();
    float minY =  std::numeric_limits<float>::max();

    for (int j = 0; j < static_cast<int>(this->controlPointGrps[idx].size()); j++) {
        maxX = std::max<float>(maxX, this->controlPointGrps[idx][j][0]);
        maxY = std::max<float>(maxY, this->controlPointGrps[idx][j][1]);
        minX = std::min<float>(minX, this->controlPointGrps[idx][j][0]);
        minY = std::min<float>(minY, this->controlPointGrps[idx][j][1]);
    }

    return std::max(fabs(maxX - minX), fabs(maxY - minY));
}

float Bezier::GetCurveLength() const {
    float length = 0.0;
    for (int i = 0; i < static_cast<int>(this->controlPointGrps.size()); i++) {
        length += this->GetCurveLengthOfControlPtGrp(i);
    }

    return length;
}

float Bezier::GetCurveLengthOfControlPtGrp(int idx) const {
    float length = 0.0f;
    
    const Point2D* lastPt = &this->controlPointGrps[idx][0];
    for (int i = 1; i < static_cast<int>(this->controlPointGrps[idx].size()); i++) {
        const Point2D* currPt = &this->controlPointGrps[idx][i];
        length += Point2D::Distance(*lastPt, *currPt);
        lastPt = currPt;
    }

    return length;
}

void Bezier::GeneratePointsAlongBezier2D(const std::vector<Point2D>& bezierControlPts, int numPointsToGenerate,
                                         std::vector<Point2D>& generatedPointsOnCurve) {
    
    generatedPointsOnCurve.clear();
    generatedPointsOnCurve.reserve(numPointsToGenerate);


    double t = 0;
    double step = 1.0 / static_cast<double>(numPointsToGenerate - 1);
    
    if (bezierControlPts.size() == 4) {
        for (int i1 = 0; i1 != numPointsToGenerate; i1++) {
        
            if ((1.0 - t) < 5e-6) {
                t = 1.0;
            }

            Point2D generatedPt = 
                pow((1.0 - t), 3) * bezierControlPts[0] +
                3*t*pow((1.0 - t), 2) * bezierControlPts[1] +
                3*t*t*(1.0 - t) * bezierControlPts[2] +
                pow(t, 3) * bezierControlPts[3];

            generatedPointsOnCurve.push_back(generatedPt);
            t += step;
        }
    }

    /*
    double t = 0;
    double step = 1.0 / static_cast<double>(numPointsToGenerate - 1);
    int numOfKnots = static_cast<int>(bezierControlPts.size()) / 2;
 
    for (int i1 = 0; i1 != numPointsToGenerate; i1++) {
        
        if ((1.0 - t) < 5e-6) {
            t = 1.0;
        }
 
        Point2D newPoint(0.0, 0.0);
        for (int i = 0; i != numOfKnots; i++) {

            double basis = Bernstein(numOfKnots-1, i, t);
            newPoint[0] += basis * bezierControlPts[i][0];
            newPoint[1] += basis * bezierControlPts[i][1];
        }
        generatedPointsOnCurve.push_back(newPoint);
 
        t += step;
    }
    */
}

double Bezier::Ni(int n, int i) {
    double a1 = Bezier::Factorial(n);
    double a2 = Bezier::Factorial(i);
    double a3 = Bezier::Factorial(n - i);

    return a1 / (a2 * a3);
}

// Calculate Bernstein basis
double Bezier::Bernstein(int n, int i, double t) {

    double ti  = 0; /* t^i */
    double tni = 0; /* (1 - t)^i */

    // Prevent problems with pow
    if (t == 0.0 && i == 0) {
        ti = 1.0;
    }
    else {
        ti = pow(t, i);
    }

    if (n == i && t == 1.0) {
        tni = 1.0;
    }
    else {
        tni = pow((1 - t), (n - i));
    }

    return Ni(n, i) * ti * tni;
}