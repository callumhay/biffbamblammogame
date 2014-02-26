/**
 * CannonBlockMesh.h
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

#ifndef __CANNONBLOCKMESH_H__
#define __CANNONBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "AbstractCannonBlockMesh.h"

class CannonBlock;
class ESPPointEmitter;

class CannonBlockMesh : public AbstractCannonBlockMesh {
public:
	CannonBlockMesh();
	~CannonBlockMesh();

	void Flush();
	
    void AddCannonBlock(const CannonBlock* cannonBlock);
	void RemoveCannonBlock(const CannonBlock* cannonBlock);

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;
	
	void SetAlphaMultiplier(float alpha);

private:
	Mesh* cannonBlockBaseGeometry;
	Mesh* cannonBlockBarrelGeometry;

	std::set<const CannonBlock*> cannonBlocks;	// A list of all the cannon blocks that are currently present in the game

	void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(CannonBlockMesh);
};

inline void CannonBlockMesh::Flush() {
	this->cannonBlocks.clear();
}

inline void CannonBlockMesh::AddCannonBlock(const CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);
	this->cannonBlocks.insert(cannonBlock);
}

inline void CannonBlockMesh::RemoveCannonBlock(const CannonBlock* cannonBlock) {
    assert(cannonBlock != NULL);
	size_t numRemoved = this->cannonBlocks.erase(cannonBlock);
    UNUSED_VARIABLE(numRemoved);
	assert(numRemoved == 1);
}

inline void CannonBlockMesh::SetAlphaMultiplier(float alpha) {
    this->cannonBlockBarrelGeometry->SetAlpha(alpha);
    this->cannonBlockBaseGeometry->SetAlpha(alpha);
}

#endif // __CANNONBLOCKMESH_H__