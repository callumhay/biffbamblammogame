#ifndef __SHAPE2D_H__
#define __SHAPE2D_H__

#include "Point.h"

class LineSeg2D {

private:
	Point2D p1, p2;

public:
	LineSeg2D() {};
	LineSeg2D(const Point2D& p1, const Point2D& p2) : p1(p1), p2(p2) {}
	~LineSeg2D() {}


	void SetP1(const Point2D& p) {
		this->p1 = p;
	}
	Point2D P1() const {
		return this->p1;
	}

	void SetP2(const Point2D& p) {
		this->p2 = p;
	}
	Point2D P2() const {
		return this->p2;
	}

};

class Circle2D {
private:
	Point2D center;
	float radius;

public:
	Circle2D(const Point2D& c, float r): center(c), radius(r) {}
	~Circle2D(){}
	
	void SetCenter(const Point2D& c) {
		this->center = c;
	}
	Point2D Center() const {
		return this->center;
	}
	float Radius() const {
		return this->radius;
	}
};

#endif