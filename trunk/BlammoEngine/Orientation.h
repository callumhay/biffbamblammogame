#ifndef __ORIENTATION_H__
#define __ORIENTATION_H__

#include "Vector.h"

/**
 * Stores the full orientation (translation and rotation) for some 3D object.
 */
class Orientation3D {
private:
	Vector3D translation;
	Vector3D rotation;

public:
	Orientation3D() {}
	Orientation3D(const Vector3D& translation, const Vector3D& rotation) : translation(translation), rotation(rotation) {}
	~Orientation3D() {}

	void SetTranslation(const Vector3D &t) {
		this->translation = t;
	}
	void SetRotation(const Vector3D& r) {
		this->rotation = r;
	}

	Vector3D GetTranslation() const {
		return this->translation;
	}
	Vector3D GetRotation() const {
		return this->rotation;
	}

	float GetTX() const {
		return this->translation[0];
	}
	float GetTY() const {
		return this->translation[1];
	}
	float GetTZ() const {
		return this->translation[2];
	}

	float GetRX() const {
		return this->rotation[0];
	}
	float GetRY() const {
		return this->rotation[1];
	}
	float GetRZ() const {
		return this->rotation[2];
	}
};

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