/**
 * BoundingLines.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

    void RemoveBound(const Collision::LineSeg2D& line);

	size_t GetNumLines() const {
		return this->lines.size();
	}
    bool IsEmpty() const {
        return this->GetNumLines() == 0;
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
    bool ClosestPointAndNormal(const Point2D& pt, float toleranceRadius, 
        Point2D& closestPt, Vector2D& closestNormal) const;

	bool IsInside(const Point2D& pt) const;

	bool CollisionCheck(const Collision::AABB2D& aabb) const;
	bool CollisionCheck(const Collision::Circle2D& c) const;
    bool CollisionCheck(const Collision::Circle2D& c, float& sqrDist) const;
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
		assert(index < static_cast<int>(this->lines.size()) && index >= 0);
		return this->lines[index];
	}

	// Get the normal at the given index within this set of bounding lines
	// Precondition: The given index must be within bounds of the number of lines.
	const Vector2D& GetNormal(int index) const {
		assert(index < static_cast<int>(this->normals.size()) && index >= 0);
		return this->normals[index];
	}

    bool GetOnInside(int index) const {
        assert(index < static_cast<int>(this->onInside.size()) && index >= 0);
        return this->onInside[index];
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

    void DrawSimpleBounds() const;

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

/**
 * Check to see if the given circle collides with any of the bounding lines of this.
 */
inline bool BoundingLines::CollisionCheck(const Collision::Circle2D& c) const {
    float temp;
	return this->CollisionCheck(c, temp);
}

#endif