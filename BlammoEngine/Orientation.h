/**
 * Orientation.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ORIENTATION_H__
#define __ORIENTATION_H__

#include "Vector.h"
#include "Matrix.h"

/**
 * Stores the full orientation (translation and rotation) for some 3D object.
 */
class Orientation3D {
private:
	Vector3D translation;
	Vector3D rotation;

public:
	Orientation3D() : translation(0,0,0), rotation(0,0,0) {}
	Orientation3D(const Vector3D& translation, const Vector3D& rotation) : translation(translation), rotation(rotation) {}
	~Orientation3D() {}

	void SetTranslation(const Vector3D &t) {
		this->translation = t;
	}
	void SetRotation(const Vector3D& r) {
		this->rotation = r;
	}

	const Vector3D& GetTranslation() const {
		return this->translation;
	}
	const Vector3D& GetRotation() const {
		return this->rotation;
	}

	Matrix4x4 GetTransform() const;

	float GetTX() const {
		return this->translation[0];
	}
	void SetTX(float tx) {
		this->translation[0] = tx;
	}
	float GetTY() const {
		return this->translation[1];
	}
	void SetTY(float ty) {
		this->translation[1] = ty;
	}
	float GetTZ() const {
		return this->translation[2];
	}
	void SetTZ(float tz) {
		this->translation[2] = tz;
	}

	float GetRX() const {
		return this->rotation[0];
	}
	void SetRX(float rx) {
		this->rotation[0] = rx;
	}
	float GetRY() const {
		return this->rotation[1];
	}
	void SetRY(float ry) {
		this->rotation[1] = ry;
	}
	float GetRZ() const {
		return this->rotation[2];
	}
	void SetRZ(float rz) {
		this->rotation[2] = rz;
	}
};

inline Matrix4x4 Orientation3D::GetTransform() const {
	Matrix4x4 translationMat	  = Matrix4x4::translationMatrix(this->translation);
	Matrix4x4 rotationMatX			= Matrix4x4::rotationMatrix('x', this->rotation[0], true);
	Matrix4x4 rotationMatY			= Matrix4x4::rotationMatrix('y', this->rotation[1], true);
	Matrix4x4 rotationMatZ			= Matrix4x4::rotationMatrix('z', this->rotation[2], true);

	return translationMat * rotationMatX * rotationMatY * rotationMatZ;

}

inline Orientation3D operator *(float s, const Orientation3D& orient) {
	return Orientation3D(s*orient.GetTranslation(), s*orient.GetRotation());
}

inline Orientation3D operator /(const Orientation3D& orient, float d) {
	assert(d != 0);
	return Orientation3D(orient.GetTranslation()/d, orient.GetRotation()/d);
}

inline Orientation3D operator +(const Orientation3D& a, const Orientation3D& b) {
  return Orientation3D(a.GetTranslation() + b.GetTranslation(), a.GetRotation() + b.GetRotation());
}

inline Orientation3D operator -(const Orientation3D& a, const Orientation3D& b) {
   return Orientation3D(a.GetTranslation() - b.GetTranslation(), a.GetRotation() - b.GetRotation());
}

inline Orientation3D operator -(const Orientation3D& a) {
  return Orientation3D(-a.GetTranslation(), -a.GetRotation());
}
#endif