/**
 * CollateralBlockMesh.h
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

#ifndef __COLLATERALBLOCKMESH_H__
#define __COLLATERALBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"

class CollateralBlock;

class CollateralBlockMesh {
public:
	CollateralBlockMesh();
	~CollateralBlockMesh();

	void Flush();
	void AddCollateralBlock(const CollateralBlock* collateralBlock);
	void RemoveCollateralBlock(const CollateralBlock* collateralBlock);
	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);

	void SetAlphaMultiplier(float alpha);

private:
    AnimationMultiLerp<float> alphaFlashAnim;

	Mesh* collateralBlockGeometry;
	std::set<const CollateralBlock*> collateralBlocks;	// A list of all the collateral blocks that are currently present in the game

	void LoadMesh();
};

inline void CollateralBlockMesh::Flush() {
	this->collateralBlocks.clear();
}

inline void CollateralBlockMesh::AddCollateralBlock(const CollateralBlock* collateralBlock) {
	this->collateralBlocks.insert(collateralBlock);
}

inline void CollateralBlockMesh::RemoveCollateralBlock(const CollateralBlock* collateralBlock) {
	this->collateralBlocks.erase(collateralBlock);
}

#endif // __COLLATERALBLOCKMESH_H__