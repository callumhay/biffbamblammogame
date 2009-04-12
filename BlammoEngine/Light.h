#ifndef _LIGHT_H__
#define _LIGHT_H__

#include "Point.h"
#include "Vector.h"
#include "Colour.h"

#include "GeometryMaker.h"

class PointLight {
private:
	Point3D position;
	Colour  colour;
	float linearAttenuation;

public:
	PointLight() : position(0,0,0), colour(0,0,0), linearAttenuation(0) {}
	PointLight(const Point3D& pos, const Colour& c, float linAtten) : position(pos), colour(c), linearAttenuation(linAtten) {}
	~PointLight() {}

	inline Colour GetColour() const { return this->colour; }
	inline Point3D GetPosition() const { return this->position; }
	inline float GetLinearAttenuation() const { return this->linearAttenuation; }
	
	inline void SetColour(const Colour& c) { this->colour = c; }
	inline void SetPosition(const Point3D& p) { this->position = p; }

	inline void DebugDraw() const {
		glColor3f(this->colour.R(), this->colour.G(), this->colour.B());
		
		// Draw quad at this light's location...
		glPushMatrix();
		glTranslatef(this->position[0], this->position[1], this->position[2]);
		GeometryMaker::GetInstance()->DrawCube();
		glPopMatrix();
	}

};
#endif