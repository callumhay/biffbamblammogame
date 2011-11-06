/**
 * Light.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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

// A simpler less stack/heap intensive version of the PointLight
class BasicPointLight {
public:
	BasicPointLight() {}
	BasicPointLight(const Point3D& pos, const Colour& diffuse, float linAtten) : 
			position(pos), diffuseColour(diffuse), linearAttenuation(linAtten) {}

	void Copy(BasicPointLight& light) const;

	inline const Colour& GetDiffuseColour() const { return this->diffuseColour; }
	inline const Point3D& GetPosition() const { return this->position; }
	inline float GetLinearAttenuation() const { return this->linearAttenuation; }

	inline void SetDiffuseColour(const Colour& c) { this->diffuseColour = c; }
	inline void SetPosition(const Point3D& p) { this->position = p; }
	inline void SetLinearAttenuation(float a) { this->linearAttenuation = a; }

private:
	Colour diffuseColour;
	Point3D position;
	float linearAttenuation;

	// Disallow copy and assignment
	BasicPointLight(const BasicPointLight& p);
	BasicPointLight& operator=(const BasicPointLight& p);
};

inline void BasicPointLight::Copy(BasicPointLight& light) const {
	light.SetDiffuseColour(this->GetDiffuseColour());
	light.SetPosition(this->GetPosition());
	light.SetLinearAttenuation(this->GetLinearAttenuation());
}

// Full animating, state-tracking point light
class PointLight {
public:
	PointLight() : position(0,0,0), currDiffuseColour(0,0,0), linearAttenuation(0), isOn(true), lightIntensity(1.0f) {}
	
	PointLight(const Point3D& pos, const Colour& diffuse, float linAtten) : 
		position(pos), onDiffuseColour(diffuse), currDiffuseColour(diffuse), linearAttenuation(linAtten), isOn(true), lightIntensity(1.0f) {}
	
	~PointLight() {}

	void CopyBasicAttributes(PointLight& copyToMe) const;
	void ConvertToBasicPointLight(BasicPointLight& basicPtLight) const;

	inline const Colour& GetDiffuseColour() const { return this->currDiffuseColour; }
	inline const Point3D& GetPosition() const { return this->position; }
	inline float GetLinearAttenuation() const { return this->linearAttenuation; }
	inline bool GetIsTurnedOn() const { return this->isOn; }
	
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

private:
	bool isOn;
	float lightIntensity;			// The current intensity of the light
	Colour onDiffuseColour;		// The colour of the light's diffuse when it's turned on completely
	Colour currDiffuseColour;	// The current diffuse colour of the light

	Point3D position;
	float linearAttenuation;

	std::list<AnimationMultiLerp<float> > lightIntensityAnim;
	std::list<AnimationMultiLerp<Colour> > lightColourChangeAnim;
	std::list<AnimationMultiLerp<Colour> > lightColourStrobeAnim;
	std::list<AnimationMultiLerp<Point3D> > lightPositionAnim;

	// Disallow copy and assignment
	PointLight(const PointLight& p);
	PointLight& operator=(const PointLight& p);

};

inline void PointLight::CopyBasicAttributes(PointLight& copyToMe) const {
	copyToMe.SetPosition(this->GetPosition());
	copyToMe.SetDiffuseColour(this->GetDiffuseColour());
	copyToMe.SetLinearAttenuation(this->GetLinearAttenuation());
}

inline void PointLight::ConvertToBasicPointLight(BasicPointLight& basicPtLight) const {
	basicPtLight.SetDiffuseColour(this->GetDiffuseColour());
	basicPtLight.SetPosition(this->GetPosition());
	basicPtLight.SetLinearAttenuation(this->GetLinearAttenuation());
}

#endif