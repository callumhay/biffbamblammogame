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

	bool Collide(const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, float &d);
	Point2D ClosestPoint(const Point2D& pt);

	bool CollisionCheck(const BoundingLines& other) const;
	int CollisionCheckIndex(const BoundingLines& other) const;
	std::vector<int> CollisionCheckIndices(const BoundingLines& other) const;
	
	// Get the line at the given index within this set of bounding lines
	// Precondition: The given index must be within bounds of the number of lines.
	inline Collision::LineSeg2D GetLine(int index) {
		assert(index < static_cast<int>(lines.size()) && index >= 0);
		return this->lines[index];
	}

	// Get the normal at the given index within this set of bounding lines
	// Precondition: The given index must be within bounds of the number of lines.
	inline Vector2D GetNormal(int index) {
		assert(index < static_cast<int>(normals.size()) && index >= 0);
		return this->normals[index];
	}

	// Completely obliterates all the bounding lines for this
	void Clear() {
		this->lines.clear();
		this->normals.clear();
	}

	// Debug stuffs
	void DebugDraw() const;

};
#endif