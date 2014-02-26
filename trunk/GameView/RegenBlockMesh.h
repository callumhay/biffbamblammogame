/**
 * RegenBlockMesh.h
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

#ifndef __REGENBLOCKMESH_H__
#define __REGENBLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

class RegenBlock;
class GameModel;

class RegenBlockMesh {
public:
    static const float CENTER_TO_DISPLAY_DEPTH;
    static const float LIFE_DISPLAY_WIDTH;
    static const float HALF_LIFE_DISPLAY_WIDTH;
    static const float LIFE_DISPLAY_HEIGHT;
    static const float HALF_LIFE_DISPLAY_HEIGHT;
    static const float X_DISPLAY_BORDER;

    RegenBlockMesh();
	~RegenBlockMesh();

    static float GenerateFontDisplayScale();
    static const TextureFontSet* GetDisplayFont();
    static Colour GetColourFromLifePercentage(float lifePercentage);

	void Flush();

	void AddRegenBlock(RegenBlock* block);
	void RemoveRegenBlock(const RegenBlock* block);

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const GameModel* gameModel, 
        const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
        const BasicPointLight& ballLight);

	void SetAlphaMultiplier(float alpha);

	void UpdateRegenBlock(const RegenBlock* block, bool gotHurt);

private:
    
    class BlockData {
    public:
        BlockData(RegenBlockMesh* regenMesh, const RegenBlock& block);
        virtual ~BlockData() {};

        void SetAlpha(float alpha) { this->alpha = alpha; }
        virtual void Update(bool gotHurt);
        virtual void Tick(double dT);
        virtual void DrawLifeInfo(const Camera& camera, const BasicPointLight& keyLight,
            const BasicPointLight& fillLight, const BasicPointLight& ballLight) = 0;
        virtual Colour GetCurrBaseMaterialColour(const Colour& baseColour) const = 0;

    protected:
        RegenBlockMesh* regenMesh;
        const RegenBlock& block;
        float alpha;
        AnimationMultiLerp<float> hurtRedFlashAnim;

    private:
        DISALLOW_COPY_AND_ASSIGN(BlockData);
    };

    class RegenBlockFiniteData : public BlockData {
    public:
        RegenBlockFiniteData(RegenBlockMesh* regenMesh, const RegenBlock& block);

        void Update(bool gotHurt);
        void Tick(double dT);
        void DrawLifeInfo(const Camera& camera, const BasicPointLight& keyLight,
            const BasicPointLight& fillLight, const BasicPointLight& ballLight);
        Colour GetCurrBaseMaterialColour(const Colour& baseColour) const;

    private:
        static const float MAX_LIFE_PERCENT_FOR_FLASHING;
        float textScale;
        float percentCharWidthWithScale;
        std::stringstream lifeStringStr;

        AnimationMultiLerp<float> aboutToDieRedFlashAnim;

        DISALLOW_COPY_AND_ASSIGN(RegenBlockFiniteData);
    };

    class RegenBlockInfiniteData : public BlockData {
    public:
        RegenBlockInfiniteData(RegenBlockMesh* regenMesh, const RegenBlock& block);

        void Update(bool gotHurt) { RegenBlockMesh::BlockData::Update(gotHurt); }
        void Tick(double dT) { RegenBlockMesh::BlockData::Tick(dT); }
        void DrawLifeInfo(const Camera& camera, const BasicPointLight& keyLight,
            const BasicPointLight& fillLight, const BasicPointLight& ballLight);
        Colour GetCurrBaseMaterialColour(const Colour& baseColour) const;

    private:
        DISALLOW_COPY_AND_ASSIGN(RegenBlockInfiniteData);
    };


    typedef std::map<RegenBlock*, BlockData*> BlockCollection;
    typedef BlockCollection::iterator BlockCollectionIter;
    typedef BlockCollection::const_iterator BlockCollectionConstIter;

	Mesh* blockMesh;
    const TextureFontSet* font;
    Texture* infinityTex;

    std::map<std::string, MaterialGroup*> materialGroups;
    MaterialGroup* baseMetalMaterialGrp;
    Colour initialBaseMetalDiffuseColour;

    BlockCollection blocks;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(RegenBlockMesh);
};

inline const std::map<std::string, MaterialGroup*>& RegenBlockMesh::GetMaterialGroups() const {
    return this->materialGroups;
}

#endif // __REGENBLOCKMESH_H__