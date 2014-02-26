/**
 * AlwaysDropBlockMesh.h
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

#ifndef __ALWAYSDROPBLOCKMESH_H__
#define __ALWAYSDROPBLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

class AlwaysDropBlock;

class AlwaysDropBlockMesh {
public:
	AlwaysDropBlockMesh();
	~AlwaysDropBlockMesh();

	void Flush();

	void AddAlwaysDropBlock(const AlwaysDropBlock* block, Texture* itemTexture);
	void RemoveAlwaysDropBlock(const AlwaysDropBlock* block);

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

    void DrawBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void DrawNoBloomPass(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void SetAlphaMultiplier(float alpha);

private:
	Mesh* blockMesh;

	std::map<const AlwaysDropBlock*, Texture*> blockToItemTexMap;
	MaterialGroup* dropTypeMatGrp;
    MaterialGroup* dropColourMatGrp;
    
    void Draw(const AlwaysDropBlock* currBlock, Texture* currTex, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(AlwaysDropBlockMesh);
};

inline void AlwaysDropBlockMesh::Flush() {
	this->blockToItemTexMap.clear();
}


#endif // __ALWAYSDROPBLOCKMESH_H__