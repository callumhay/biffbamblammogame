/**
 * FragileCannonBlockMesh.h
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

#ifndef __FRAGILECANNONBLOCKMESH_H__
#define __FRAGILECANNONBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "AbstractCannonBlockMesh.h"

class FragileCannonBlock;


class FragileCannonBlockMesh : public AbstractCannonBlockMesh {
public:
    FragileCannonBlockMesh();
    ~FragileCannonBlockMesh();

    void Flush();

    void AddFragileCannonBlock(const FragileCannonBlock* block);
    void RemoveFragileCannonBlock(const FragileCannonBlock* block);

    void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;

    void SetAlphaMultiplier(float alpha);

private:
    Mesh* basePostMesh; 
    Mesh* baseBarMesh;
    Mesh* barrelMesh;

    std::set<const FragileCannonBlock*> cannonBlocks;
    void DrawBlockMesh(float rotDegs, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(FragileCannonBlockMesh);
};

inline void FragileCannonBlockMesh::Flush() {
    this->cannonBlocks.clear();
}

inline void FragileCannonBlockMesh::AddFragileCannonBlock(const FragileCannonBlock* block) {
    assert(block != NULL);
    this->cannonBlocks.insert(block);
}

inline void FragileCannonBlockMesh::RemoveFragileCannonBlock(const FragileCannonBlock* block) {
    assert(block != NULL);
    size_t numRemoved = this->cannonBlocks.erase(block);
    UNUSED_VARIABLE(numRemoved);
    assert(numRemoved == 1);
}

inline void FragileCannonBlockMesh::SetAlphaMultiplier(float alpha) {
    this->basePostMesh->SetAlpha(alpha);
    this->baseBarMesh->SetAlpha(alpha);
    this->barrelMesh->SetAlpha(alpha);
}

#endif // __FRAGILECANNONBLOCKMESH_H__