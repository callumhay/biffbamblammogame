/**
 * RegenBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __REGENBLOCKMESH_H__
#define __REGENBLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

class RegenBlock;

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

	void Flush();

	void AddRegenBlock(RegenBlock* block);
	void RemoveRegenBlock(const RegenBlock* block);

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);

	void SetAlphaMultiplier(float alpha);

	void UpdateRegenBlock(const RegenBlock* block);

private:
    
    class BlockData {
    public:
        BlockData(RegenBlockMesh* regenMesh, const RegenBlock& block) : regenMesh(regenMesh), block(block), alpha(1.0f) {};
        virtual ~BlockData() {};

        void SetAlpha(float alpha) { this->alpha = alpha; }
        virtual void Update() = 0;
        virtual void DrawLifeInfo() = 0;

    protected:
        RegenBlockMesh* regenMesh;
        const RegenBlock& block;
        float alpha;

    private:
        DISALLOW_COPY_AND_ASSIGN(BlockData);
    };

    class RegenBlockFiniteData : public BlockData {
    public:
        RegenBlockFiniteData(RegenBlockMesh* regenMesh, const RegenBlock& block);

        void Update();
        void DrawLifeInfo();

    private:
        float textScale;
        float percentCharWidthWithScale;

        std::stringstream lifeStringStr;
        DISALLOW_COPY_AND_ASSIGN(RegenBlockFiniteData);
    };

    class RegenBlockInfiniteData : public BlockData {
    public:
        RegenBlockInfiniteData(RegenBlockMesh* regenMesh, const RegenBlock& block);

        void Update() { /* Nothing to update */ };
        void DrawLifeInfo();

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

    BlockCollection blocks;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(RegenBlockMesh);
};

inline const std::map<std::string, MaterialGroup*>& RegenBlockMesh::GetMaterialGroups() const {
    return this->materialGroups;
}

#endif // __REGENBLOCKMESH_H__