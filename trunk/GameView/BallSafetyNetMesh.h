/**
 * BallSafetyNetMesh.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
class CgFxCelShading;
class GameLevel;

class BallSafetyNetMesh {
public:
	static const float SAFETY_NET_HEIGHT;
	static const float SAFETY_NET_DEPTH;

	BallSafetyNetMesh();
	~BallSafetyNetMesh();

	bool IsPlayingAnimation() const { 
		return this->currAnimation == BallSafetyNetMesh::CreationAnimation || this->currAnimation == BallSafetyNetMesh::DestructionAnimation;
	}
	void Regenerate(const GameLevel& currLevel);

	void CreateBallSafetyNet();
	void DestroyBallSafetyNet(const GameLevel& currLevel, float destructionXPos);
	void SetAlpha(float alpha) {
		assert(alpha >= 0 && alpha <= 1);
		this->idleAlpha = alpha;
	}
	
    void Tick(double dT);
	void Draw(const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);

    const Point2D& GetSafetyNetCenterPosition() const;

private:
	enum BallSafetyNetAnimation{ Dead, Idle, CreationAnimation, DestructionAnimation };

	BallSafetyNetAnimation currAnimation;

	CgFxCelShading* shadingMaterial;
	GLuint displayListID;
    Point2D centerPos;
	
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
};

inline const Point2D& BallSafetyNetMesh::GetSafetyNetCenterPosition() const {
    return this->centerPos;
}


#endif