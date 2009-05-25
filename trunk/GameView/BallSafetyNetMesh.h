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

	void Draw(double dT, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight);
};
#endif