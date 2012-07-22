/**
 * BallSafetyNetMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BALLSAFETYNETMESH_H__
#define __BALLSAFETYNETMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Orientation.h"
#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Animation.h"

class Camera;
class PointLight;
class CgFxPhong;

class BallSafetyNetMesh {
private:
	enum BallSafetyNetAnimation{ Dead, Idle, CreationAnimation, DestructionAnimation };

	BallSafetyNetAnimation currAnimation;

	CgFxPhong* shadingMaterial;
	GLuint displayListID;
	
	// Destruction animation values 
	GLuint leftPieceDispListID, rightPieceDispListID;
	Orientation3D leftPieceOrient, rightPieceOrient;
	AnimationLerp<Orientation3D> leftPieceAnim, rightPieceAnim;

	// Misc. animation values
	float idleAlpha;
	float pieceAlpha;
	AnimationLerp<float> pieceFadeAnim;

	void InitializeMaterials();
	static void DrawSafetyNetMesh(float minXCoord, float maxXCoord, float minTexCoordU, float maxTexCoordU);

public:
	static const float SAFETY_NET_HEIGHT;
	static const float SAFETY_NET_DEPTH;

	BallSafetyNetMesh();
	~BallSafetyNetMesh();

	bool IsPlayingAnimation() const { 
		return this->currAnimation == BallSafetyNetMesh::CreationAnimation || this->currAnimation == BallSafetyNetMesh::DestructionAnimation;
	}
	void Regenerate(const Vector2D& levelDimensions);

	void CreateBallSafetyNet();
	void DestroyBallSafetyNet(const Vector2D& levelDimensions, float destructionXPos);
	void SetAlpha(float alpha) {
		assert(alpha >= 0 && alpha <= 1);
		this->idleAlpha = alpha;
	}
	

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);
};
#endif