#ifndef __BOUNDINGLINES_H__
#define __BOUNDINGLINES_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Collision.h"

class BoundingLines {

private:
	std::vector<Collision::LineSeg2D> lines;
	std::vector<Vector2D> normals;

public:
	BoundingLines(){};
	BoundingLines(const std::vector<Collision::LineSeg2D>& lines, const std::vector<Vector2D>& norms);
	~BoundingLines();

	bool Collide(const Collision::Circle2D& c, Vector2D& n, float &d);
	
	// Completely obliterates all the bounding lines for this
	void Clear() {
		this->lines.clear();
		this->normals.clear();
	}

	// Debug stuffs
	void DebugDraw() const;

};
#endif