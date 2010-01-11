/**
 * Beam.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009 - 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BEAM_H__
#define __BEAM_H__

#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Tree.h"

class Beam {
public:
	enum BeamType { PaddleLaserBeam };

	// Structure for defining a part of the overall beam - a complete
	// linear segment of the beam
	struct BeamSegment {
		double timeSinceFired;				// Time in seconds since this part of the beam was fired/shot
		Collision::LineSeg2D segment;	// The line segment defining this part of the beam (from origin to end)
		float radius;									// Half-width of the beam segment
	};

	virtual ~Beam() {};
	BeamType GetBeamType() const { return this->type; }

protected:
	BeamType type;									// Type of beam
	Tree<BeamSegment> beamSegTree;	// The beams parts (that form a tree) that make up this entire beam

	Beam(BeamType type) : type(type) {};
};

class LaserBeam : public Beam {
public:
	LaserBeam();
	~LaserBeam();



};


#endif