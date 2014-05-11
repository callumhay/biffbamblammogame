/**
 * TeslaBlockMesh.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#ifndef __TESLABLOCKMESH_H__
#define __TESLABLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESPParticleScaleEffector.h"
#include "../ESPEngine/ESPParticleColourEffector.h"

class TeslaBlock;
class ESPPointEmitter;
class Texture2D;
class CgFxPostRefract;

class TeslaBlockMesh {
public:
	TeslaBlockMesh();
	~TeslaBlockMesh();

	void Flush();
	void AddTeslaBlock(TeslaBlock* teslaBlock);
	void RemoveTeslaBlock(TeslaBlock* teslaBlock);
	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

    void DrawPostEffects(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void SetAlphaMultiplier(float alpha);

private:
	static const float COIL_ROTATION_SPEED_DEGSPERSEC;

    typedef std::set<TeslaBlock*> TeslaBlockSet;
    typedef TeslaBlockSet::iterator TeslaBlockSetIter;
    typedef TeslaBlockSet::const_iterator TeslaBlockSetConstIter;
    
	std::map<std::string, MaterialGroup*> materialGroups;  // Material groups for the static parts of the tesla block mesh
	TeslaBlockSet teslaBlocks;                             // A list of all the tesla blocks that are currently present in the game

	Mesh* teslaBaseMesh;
	Mesh* teslaCoilMesh;

	ESPParticleScaleEffector flarePulse;
	//ESPParticleAccelEffector sparkGravity;
	ESPPointEmitter* teslaCenterFlare;
	//ESPPointEmitter* teslaSparks;
	Texture2D* flareTex;
	//Texture2D* sparkTex;

    ESPPointEmitter* shieldEmitter;
    ESPParticleScaleEffector haloExpandPulse;
    ESPParticleColourEffector haloFader;
    Texture2D* haloTexture;

	void DrawTeslaShield(const Matrix4x4& screenAlignMatrix);

	void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(TeslaBlockMesh);
};

inline void TeslaBlockMesh::Flush() {
	this->teslaBlocks.clear();
}

inline void TeslaBlockMesh::AddTeslaBlock(TeslaBlock* teslaBlock) {
	assert(teslaBlock != NULL);
	std::pair<TeslaBlockSetIter, bool> insertResult = this->teslaBlocks.insert(teslaBlock);
	assert(insertResult.second);
}	

inline void TeslaBlockMesh::RemoveTeslaBlock(TeslaBlock* teslaBlock) {
	size_t numRemoved = this->teslaBlocks.erase(teslaBlock);
    UNUSED_VARIABLE(numRemoved);
	assert(numRemoved == 1);
}

inline const std::map<std::string, MaterialGroup*>& TeslaBlockMesh::GetMaterialGroups() const {
	return this->materialGroups;
}

#endif // __TESLABLOCKMESH_H__