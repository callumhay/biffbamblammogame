/**
 * Orientation.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ORIENTATION_H__
#define __ORIENTATION_H__

#include "Vector.h"
#include "Matrix.h"
#include "Animation.h"

#include <Eigen/Geometry>

class OrientationMultiLerp;

/**
 * Stores the full orientation (translation and rotation) for some 3D object.
 */
class Orientation3D {
    friend class OrientationMultiLerp;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Orientation3D() : translation(0,0,0), rotation(Eigen::Matrix3f::Identity()) {}
	Orientation3D(const Vector3D& translation, const Vector3D& rotationXYZ) {
        this->SetTranslation(translation);
        this->SetXYZRotation(rotationXYZ);
    }
    Orientation3D(const Eigen::Vector3f& translation, const Eigen::Matrix3f& rotation) : translation(translation), rotation(rotation) {
    }
	~Orientation3D() {}

	void SetTranslation(const Vector3D &t) {
		this->translation << t[0], t[1], t[2];
	}
	void SetXYZRotation(const Vector3D& xyzRot) {
        this->rotation = 
            Eigen::AngleAxisf(Trig::degreesToRadians(xyzRot[0]), Eigen::Vector3f::UnitX()) *
            Eigen::AngleAxisf(Trig::degreesToRadians(xyzRot[1]), Eigen::Vector3f::UnitY()) *
            Eigen::AngleAxisf(Trig::degreesToRadians(xyzRot[2]), Eigen::Vector3f::UnitZ());
	}
    void SetZYXRotation(const Vector3D& zyxRot) {
        this->rotation = 
            Eigen::AngleAxisf(Trig::degreesToRadians(zyxRot[0]), Eigen::Vector3f::UnitZ()) *
            Eigen::AngleAxisf(Trig::degreesToRadians(zyxRot[1]), Eigen::Vector3f::UnitY()) *
            Eigen::AngleAxisf(Trig::degreesToRadians(zyxRot[2]), Eigen::Vector3f::UnitX());
    }
    void SetRotationLookAtVector(const Vector3D& lookAtDir, const Vector3D& startingVec = Vector3D(0,0,-1)) {
        Eigen::Quaternionf qRot;
        qRot.FromTwoVectors(
            Eigen::Vector3f(startingVec[0], startingVec[1], startingVec[2]), 
            Eigen::Vector3f(lookAtDir[0], lookAtDir[1], lookAtDir[2]));
        qRot.normalize();
        this->rotation = qRot.toRotationMatrix();
    }

	Vector3D GetTranslation() const {
		return Vector3D(this->translation[0], this->translation[1], this->translation[2]);
	}
    Vector2D GetTranslation2D() const {
        return Vector2D(this->translation[0], this->translation[1]);
    }
	Vector3D GetXYZRotation() const {
        Eigen::Vector3f eulerAngles = this->rotation.eulerAngles(0, 1, 2);
        return Vector3D(
            Trig::radiansToDegrees(eulerAngles[0]), 
            Trig::radiansToDegrees(eulerAngles[1]), 
            Trig::radiansToDegrees(eulerAngles[2]));
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
		return Trig::radiansToDegrees(this->rotation.eulerAngles(0, 1, 2)[0]);
	}
	float GetRY() const {
		return Trig::radiansToDegrees(this->rotation.eulerAngles(0, 1, 2)[1]);
	}
	float GetRZ() const {
		return Trig::radiansToDegrees(this->rotation.eulerAngles(0, 1, 2)[2]);
	}

private:
    Eigen::Vector3f translation;
    Eigen::Matrix3f rotation;
};

inline Matrix4x4 Orientation3D::GetTransform() const {
    Eigen::Affine3f t =  Eigen::Translation3f(this->translation) * this->rotation;
    return Matrix4x4(t.data());
}

inline Orientation3D operator /(const Orientation3D& orient, float d) {
    return Orientation3D(orient.GetTranslation() / d, orient.GetXYZRotation() / d);
}

inline Orientation3D operator *(float s, const Orientation3D& orient) {
	return Orientation3D(s*orient.GetTranslation(), s*orient.GetXYZRotation());
}

inline Orientation3D operator +(const Orientation3D& a, const Orientation3D& b) {
  return Orientation3D(a.GetTranslation() + b.GetTranslation(), a.GetXYZRotation() + b.GetXYZRotation());
}

inline Orientation3D operator -(const Orientation3D& a, const Orientation3D& b) {
   return Orientation3D(a.GetTranslation() - b.GetTranslation(), a.GetXYZRotation() - b.GetXYZRotation());
}

inline Orientation3D operator -(const Orientation3D& a) {
  return Orientation3D(-a.GetTranslation(), -a.GetXYZRotation());
}

class OrientationMultiLerp : public AnimationMultiLerp<Orientation3D> {
public:
    OrientationMultiLerp() : AnimationMultiLerp<Orientation3D>() {}
    OrientationMultiLerp(const Orientation3D& ori) : AnimationMultiLerp<Orientation3D>(ori) {}
    OrientationMultiLerp(Orientation3D* interpolant) : AnimationMultiLerp<Orientation3D>(interpolant) {}
    OrientationMultiLerp(const OrientationMultiLerp& copy) : AnimationMultiLerp<Orientation3D>(copy) {}

    OrientationMultiLerp& operator=(const OrientationMultiLerp &rhs) {
        AnimationMultiLerp<Orientation3D>::operator =(rhs);
        return *this;
    }

private:
    Orientation3D ExecuteLerp(const Orientation3D& valueStart, const Orientation3D& valueEnd, 
                              double timeStart, double timeEnd) const {
        
        double normalizedTime = (x - timeStart) / (timeEnd - timeStart);

        Orientation3D result;
        
        // The translation part of the orientation is easy (simple linear interpolation)...
        result.translation = valueStart.translation + normalizedTime * (valueEnd.translation - valueStart.translation);
        
        // The rotation part requires a slerp using quaternions...
        Eigen::Quaternionf startQ(valueStart.rotation);
        Eigen::Quaternionf endQ(valueEnd.rotation);
        result.rotation = startQ.slerp(normalizedTime, endQ);

        return result;
    }
};

#endif