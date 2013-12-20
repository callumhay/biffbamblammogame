/**
 * BoundingLines.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOUNDINGLINES_H__
#define __BOUNDINGLINES_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Collision.h"

class BoundingLines {
public:
	BoundingLines(){};
	BoundingLines(const std::vector<Collision::LineSeg2D>& lines, const std::vector<Vector2D>& norms);
    BoundingLines(const std::vector<Collision::LineSeg2D>& lines, const std::vector<Vector2D>& norms,
        const std::vector<bool>& onInside);
    BoundingLines(int numLines, const Collision::LineSeg2D lineArray[], const Vector2D normArray[], const bool onInsideArray[]);
    BoundingLines(int numLines, const Collision::LineSeg2D lineArray[], const Vector2D normArray[]);
    BoundingLines(const Collision::AABB2D& aabb);
    BoundingLines(const BoundingLines& copy);
	~BoundingLines();

    void AddBound(const Collision::LineSeg2D& line, const Vector2D& norm, bool onInside = false);
    void AddBounds(const BoundingLines& bounds);

	size_t GetNumLines() const {
		return this->lines.size();
	}

	Collision::AABB2D GenerateAABBFromLines() const;
    Collision::Circle2D GenerateCircleFromLines() const;

    // Ball - bounding line collisions, where the ball is moving...
	bool Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
	    Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& cPointOfCollision) const;
	bool Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
	    Collision::LineSeg2D& collisionLine, int& collisionLineIdx, double& timeUntilCollision, 
        Point2D& cPointOfCollision) const;

    // Ball-BoundingLine collisions where both the ball and these bounding lines are moving...
	bool Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
	    Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& cPointOfCollision, 
        const Vector2D& lineVelocity) const;

	Point2D ClosestPoint(const Point2D& pt) const;
	bool IsInside(const Point2D& pt) const;

	bool CollisionCheck(const Collision::AABB2D& aabb) const;
	bool CollisionCheck(const Collision::Circle2D& c) const;
	bool CollisionCheck(const BoundingLines& other) const;
    bool CollisionCheck(const BoundingLines& other, double dT, const Vector2D& velocity) const;
    bool CollisionCheck(const Collision::LineSeg2D& lineSeg) const;
	int CollisionCheckIndex(const BoundingLines& other) const;

	std::vector<int> CollisionCheckIndices(const BoundingLines& other) const;
    std::vector<int> CollisionCheckIndices(const Collision::Ray2D& ray) const;
	std::vector<int> CollisionCheckIndices(const Collision::LineSeg2D& lineSeg) const;
	std::vector<int> ClosestCollisionIndices(const Point2D& pt, float tolerance) const;

	bool GetCollisionPoints(const BoundingLines& other, std::list<Point2D>& collisionPts) const;
	bool GetCollisionPoints(const Collision::Circle2D& circle, std::list<Point2D>& collisionPts) const;
	bool GetCollisionPoints(const Collision::AABB2D& aabb, std::list<Point2D>& collisionPts) const;

	bool CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	
	void RotateLinesAndNormals(float angleInDegs, const Point2D& rotationCenter);
	void TranslateBounds(const Vector2D& translation);
    void ReflectX();
    void Transform(const Matrix4x4& transform);


	// Get the line at the given index within this set of bounding lines
	// Precondition: The given index must be within bounds of the number of lines.
	const Collision::LineSeg2D& GetLine(int index) const {
		assert(index < static_cast<int>(lines.size()) && index >= 0);
		return this->lines[index];
	}

	// Get the normal at the given index within this set of bounding lines
	// Precondition: The given index must be within bounds of the number of lines.
	const Vector2D& GetNormal(int index) const {
		assert(index < static_cast<int>(normals.size()) && index >= 0);
		return this->normals[index];
	}

    void SetAllBoundsInside(bool inside);
    void SetBoundInside(int index, bool inside);

	// Completely obliterates all the bounding lines for this
	void Clear() {
		this->lines.clear();
		this->normals.clear();
        this->onInside.clear();
	}
    void PopLast() {
        this->lines.pop_back();
        this->normals.pop_back();
        this->onInside.pop_back();
    }
    void Push(const Collision::LineSeg2D& line, const Vector2D& normal, bool inside = false) {
        this->lines.push_back(line);
        this->normals.push_back(normal);
        this->onInside.push_back(inside);
    }

    BoundingLines& operator=(const BoundingLines& copy);

	// Debug stuffs
	void DebugDraw() const;

private:
    static const float BALL_INSIDE_OUTSIDE_DIST_DIVISOR;
    static const float BALL_COLLISION_SAMPLING_INV_AMT;

	std::vector<Collision::LineSeg2D> lines;
	std::vector<Vector2D> normals;
    std::vector<bool> onInside;
};

inline bool BoundingLines::Collide(double dT, const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, 
                                   Collision::LineSeg2D& collisionLine, double& timeUntilCollision, Point2D& cPointOfCollision) const {

    int temp;
    return this->Collide(dT, c, velocity, n, collisionLine, temp, timeUntilCollision, cPointOfCollision);
}

/**
 * Check to see whether this collided with another set of bounding lines.
 * Returns: true if any lines in this collided with any lines in the given BoundingLines object,
 * false otherwise.
 */
inline bool BoundingLines::CollisionCheck(const BoundingLines& other) const {
	return this->CollisionCheckIndex(other) != -1;
}

#endif