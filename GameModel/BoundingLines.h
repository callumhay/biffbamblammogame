/**
 * BoundingLines.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

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

	size_t GetNumLines() const {
		return this->lines.size();
	}

	Collision::AABB2D GenerateAABBFromLines() const;

	bool Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
							 Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	Point2D ClosestPoint(const Point2D& pt) const;
	bool IsInside(const Point2D& pt) const;

	bool CollisionCheck(const Collision::AABB2D& aabb) const;
	bool CollisionCheck(const Collision::Circle2D& c) const;
	bool CollisionCheck(const BoundingLines& other) const;
    bool CollisionCheck(const Collision::LineSeg2D& lineSeg) const;
	int CollisionCheckIndex(const BoundingLines& other) const;

	std::vector<int> CollisionCheckIndices(const BoundingLines& other) const;
	std::vector<int> CollisionCheckIndices(const Collision::LineSeg2D& lineSeg) const;
	std::vector<int> ClosestCollisionIndices(const Point2D& pt, float tolerance) const;

	bool GetCollisionPoints(const BoundingLines& other, std::list<Point2D>& collisionPts) const;
	bool GetCollisionPoints(const Collision::Circle2D& circle, std::list<Point2D>& collisionPts) const;
	bool GetCollisionPoints(const Collision::AABB2D& aabb, std::list<Point2D>& collisionPts) const;

	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	
	void RotateLinesAndNormals(float angleInDegs, const Point2D& rotationCenter);
	void TranslateBounds(const Vector2D& translation);

	// Get the line at the given index within this set of bounding lines
	// Precondition: The given index must be within bounds of the number of lines.
	inline const Collision::LineSeg2D& GetLine(int index) const {
		assert(index < static_cast<int>(lines.size()) && index >= 0);
		return this->lines[index];
	}

	// Get the normal at the given index within this set of bounding lines
	// Precondition: The given index must be within bounds of the number of lines.
	inline const Vector2D& GetNormal(int index) const {
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