/**
 * ESPVolumeEmitter.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __ESPVOLUMEEMITTER_H__
#define __ESPVOLUMEEMITTER_H__

#include "ESPEmitter.h"

/**
 * An area emitter - emits sprites from a defined area.
 */
class ESPVolumeEmitter : public ESPEmitter {
public:
	ESPVolumeEmitter();
	~ESPVolumeEmitter();

	// Rectangle-prism volume setters
	//void SetEmitArea(const Point3D& center, const Vector3D& rectHalfVert, const Vector3D& rectHalfHoriz);
	void SetEmitVolume(const Point3D& min, const Point3D& max);
	void SetEmitDirection(const Vector3D& dir);

protected:
	Vector3D CalculateRandomInitParticleDir() const;
	Point3D  CalculateRandomInitParticlePos() const;

private:
	// Super class for area shapes to emit from
	struct EmitVolume {
		virtual ~EmitVolume() {}
		virtual Point3D GetRandomPointInVolume() const = 0;
	};
	//struct EmitOval : public EmitVolume {
	//	Point3D center;
	//	float radius;
	//	virtual Point3D GetRandomPointInVolume() const;
	//};
	struct EmitAABB : public EmitVolume {
		Point3D min, max;
		virtual Point3D GetRandomPointInVolume() const;
	};

	Vector3D emitDir;													// The direction (in emitter coords) that the emitter fires towards
	ESPVolumeEmitter::EmitVolume* emitVolume;	// The area to emit particles from

	ESPVolumeEmitter(const ESPVolumeEmitter& e);
	ESPVolumeEmitter& operator=(const ESPVolumeEmitter& e);

};
#endif
