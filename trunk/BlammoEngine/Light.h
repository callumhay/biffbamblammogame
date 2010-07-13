/**
 * Light.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef _LIGHT_H__
#define _LIGHT_H__

#include "BasicIncludes.h"
#include "Point.h"
#include "Vector.h"
#include "Colour.h"
#include "Animation.h"

#include "GeometryMaker.h"

class PointLight {
private:
	bool isOn;
	float lightIntensity;			// The current intensity of the light
	Colour onAmbientColour;		// The colour of the light's ambient when it's turned on completely
	Colour onDiffuseColour;		// The colour of the light's diffuse when it's turned on completely
	Colour currAmbientColour;	// The current ambient colour of the light
	Colour currDiffuseColour;	// The current diffuse colour of the light

	Point3D position;
	float linearAttenuation;

	std::list<AnimationMultiLerp<float> > lightIntensityAnim;
	std::list<AnimationMultiLerp<Colour> > lightColourChangeAnim;
	std::list<AnimationMultiLerp<Colour> > lightColourStrobeAnim;
	std::list<AnimationMultiLerp<Point3D> > lightPositionAnim;

public:
	PointLight() : position(0,0,0), currAmbientColour(0,0,0), currDiffuseColour(0,0,0), linearAttenuation(0), isOn(true), lightIntensity(1.0f) {}
	
	PointLight(const Point3D& pos, const Colour& diffuse, float linAtten) : 
		position(pos), onAmbientColour(0,0,0), currAmbientColour(0,0,0), 
		onDiffuseColour(diffuse), currDiffuseColour(diffuse), linearAttenuation(linAtten), isOn(true), lightIntensity(1.0f) {}
	
	PointLight(const Point3D& pos, const Colour& ambient, const Colour& diffuse, float linAtten) : 
		position(pos), onAmbientColour(ambient), currAmbientColour(ambient), 
		onDiffuseColour(diffuse), currDiffuseColour(diffuse), linearAttenuation(linAtten), isOn(true), lightIntensity(1.0f) {}
	
	~PointLight() {}

	//inline Colour GetAmbientColour() const { return this->currAmbientColour; }
	inline const Colour& GetDiffuseColour() const { return this->currDiffuseColour; }

	//inline Colour* GetDiffuseColourPtr() { return &this->currDiffuseColour; }  
	inline const Point3D& GetPosition() const { return this->position; }
	inline float GetLinearAttenuation() const { return this->linearAttenuation; }
	inline bool GetIsTurnedOn() const { return this->isOn; }
	
	//inline void SetAmbientColour(const Colour& c) { this->onAmbientColour = c; }
	inline void SetDiffuseColour(const Colour& c) { this->onDiffuseColour = c; }
	inline void SetPosition(const Point3D& p) { this->position = p; }
	inline void SetLinearAttenuation(float a) { this->linearAttenuation = a; }

	void SetLightOn(bool turnOn, float animationTime);
	void SetLightStrobeOn(const Colour& strobeEndColour, float strobeTime);
	void SetLightStrobeOff();
	void SetLightColourChange(const Colour& newColour, float changeTime);
	void SetLightPositionChange(const Point3D& newPosition, float changeTime);
	void Tick(double dT);

	inline void DebugDraw() const {
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glColor4f(this->onDiffuseColour.R(), this->onDiffuseColour.G(), this->onDiffuseColour.B(), 0.4f);
		
		// Draw quad at this light's location...
		glPushMatrix();
		glTranslatef(this->position[0], this->position[1], this->position[2]);
		GeometryMaker::GetInstance()->DrawCube();
		glPopMatrix();

		glPopAttrib();
	}

};
#endif