#ifndef _LIGHT_H__
#define _LIGHT_H__

#include "Point.h"
#include "Vector.h"
#include "Colour.h"

#include "GeometryMaker.h"

class PointLight {
private:
	Point3D position;
	Colour ambientColour;
	Colour diffuseColour;
	float linearAttenuation;

public:
	PointLight() : position(0,0,0), ambientColour(0,0,0), diffuseColour(0,0,0), linearAttenuation(0) {}
	
	PointLight(const Point3D& pos, const Colour& diffuse, float linAtten) : 
		position(pos), ambientColour(0,0,0), diffuseColour(diffuse), linearAttenuation(linAtten) {}
	
	PointLight(const Point3D& pos, const Colour& ambient, const Colour& diffuse, float linAtten) : 
		position(pos), ambientColour(ambient), diffuseColour(diffuse), linearAttenuation(linAtten) {}
	
	~PointLight() {}

	inline Colour GetAmbientColour() const { return this->ambientColour; }
	inline Colour GetDiffuseColour() const { return this->diffuseColour; }
	inline Point3D GetPosition() const { return this->position; }
	inline float GetLinearAttenuation() const { return this->linearAttenuation; }
	
	inline void SetAmbientColour(const Colour& c) { this->ambientColour = c; }
	inline void SetDiffuseColour(const Colour& c) { this->diffuseColour = c; }
	inline void SetPosition(const Point3D& p) { this->position = p; }
	inline void SetLinearAttenuation(float a) { this->linearAttenuation = a; }

	inline void DebugDraw() const {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glColor4f(this->diffuseColour.R(), this->diffuseColour.G(), this->diffuseColour.B(), 0.4f);
		
		// Draw quad at this light's location...
		glPushMatrix();
		glTranslatef(this->position[0], this->position[1], this->position[2]);
		GeometryMaker::GetInstance()->DrawCube();
		glPopMatrix();
	}

};
#endif