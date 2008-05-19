#ifndef __BOUNDINGLINES_H__
#define __BOUNDINGLINES_H__

#include <vector>

#include "../Utils/Shape2D.h"
#include "../Utils/Vector.h"

class BoundingLines {

private:
	std::vector<LineSeg2D> lines;
	std::vector<Vector2D> normals;

public:
	BoundingLines(){};
	BoundingLines(const std::vector<LineSeg2D>& lines, const std::vector<Vector2D>& norms);
	~BoundingLines();

	bool Collide(const Circle2D& c, Vector2D& n, float &d);
	
	// Completely obliterates all the bounding lines for this
	void Clear() {
		this->lines.clear();
		this->normals.clear();
	}

	// Debug stuffs
	void DebugDraw() const;

};
#endif