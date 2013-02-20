
#include "RegenBlockMesh.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"

#include "../GameModel/RegenBlock.h"

#include "../ResourceManager.h"

const float RegenBlockMesh::CENTER_TO_DISPLAY_DEPTH = 0.8f;
const float RegenBlockMesh::LIFE_DISPLAY_WIDTH = 1.7f;
const float RegenBlockMesh::HALF_LIFE_DISPLAY_WIDTH = LIFE_DISPLAY_WIDTH / 2.0f;
const float RegenBlockMesh::LIFE_DISPLAY_HEIGHT = 0.7f;
const float RegenBlockMesh::HALF_LIFE_DISPLAY_HEIGHT = LIFE_DISPLAY_HEIGHT / 2.0f;
const float RegenBlockMesh::X_DISPLAY_BORDER = 0.1f;

#define DISPLAY_FONT GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small)

RegenBlockMesh::RegenBlockMesh() : blockMesh(NULL), infinityTex(NULL),
font(DISPLAY_FONT) {
    this->LoadMesh();
    this->infinityTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_INFINITY_CHAR, Texture::Trilinear);
    assert(this->infinityTex != NULL);
}

RegenBlockMesh::~RegenBlockMesh() {
    this->Flush();

    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->blockMesh);
	assert(success);
    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->infinityTex);
    assert(success);
    UNUSED_VARIABLE(success);
}

float RegenBlockMesh::GenerateFontDisplayScale() {
    return (LIFE_DISPLAY_WIDTH - 2*X_DISPLAY_BORDER) / RegenBlockMesh::GetDisplayFont()->GetWidth("100%");
}

const TextureFontSet* RegenBlockMesh::GetDisplayFont() {
    return DISPLAY_FONT;
}

void RegenBlockMesh::Flush() {
    // Clean up the block data...
    for (BlockCollectionIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
        BlockData* blockData = iter->second;
        delete blockData;
        blockData = NULL;
    }
    this->blocks.clear();
}

void RegenBlockMesh::AddRegenBlock(RegenBlock* block) {
	assert(block != NULL);

    BlockData* data = NULL;
    if (block->HasInfiniteLife()) {
        data = new RegenBlockInfiniteData(this, *block);
    }
    else {
        data = new RegenBlockFiniteData(this, *block);
    }

    std::pair<BlockCollectionIter, bool> insertResult = this->blocks.insert(std::make_pair(block, data));
	assert(insertResult.second);
}

void RegenBlockMesh::RemoveRegenBlock(const RegenBlock* block) {
    BlockCollectionConstIter findIter = this->blocks.find(const_cast<RegenBlock*>(block));
    assert(findIter != this->blocks.end());

    BlockData* blockData = findIter->second;
    delete blockData;
    blockData = NULL;
    this->blocks.erase(findIter);
}

void RegenBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
                          const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(keyLight);
    UNUSED_PARAMETER(fillLight);
    UNUSED_PARAMETER(ballLight);

    if (this->blocks.empty()) {
        return;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   

	// Go through each block and draw the life info displays and any effects
    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
    	
        RegenBlock* currBlock = iter->first;
        BlockData* currBlockData = iter->second;

        // Draw information on the block (life amount or infinity symbol)
        currBlockData->DrawLifeInfo();

        // Regenerate the block...
        currBlock->Regen(dT);
    }

    glPopAttrib();
}

void RegenBlockMesh::SetAlphaMultiplier(float alpha) {
    this->blockMesh->SetAlpha(alpha);

    //for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
    //    BlockData* currBlockData = iter->second;
    //    currBlockData->SetAlpha(alpha);
    //}
}

void RegenBlockMesh::UpdateRegenBlock(const RegenBlock* block) {
    BlockCollectionConstIter findIter = this->blocks.find(const_cast<RegenBlock*>(block));
    assert(findIter != this->blocks.end());
    BlockData* data = findIter->second;
    assert(data != NULL);
    data->Update();
}

void RegenBlockMesh::LoadMesh() {

    assert(this->blockMesh == NULL);
    this->blockMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->REGEN_BLOCK_MESH);
    assert(this->blockMesh != NULL);

    const std::map<std::string, MaterialGroup*>& matGrps = this->blockMesh->GetMaterialGroups();
    this->materialGroups.insert(matGrps.begin(), matGrps.end());
    assert(this->materialGroups.size() > 0);
}


RegenBlockMesh::RegenBlockFiniteData::RegenBlockFiniteData(RegenBlockMesh* regenMesh, 
                                                           const RegenBlock& block) :
BlockData(regenMesh, block) {
    this->textScale = RegenBlockMesh::GenerateFontDisplayScale();
    this->percentCharWidthWithScale = this->textScale * RegenBlockMesh::GetDisplayFont()->GetWidth("%");
    this->lifeStringStr << "100";
}

void RegenBlockMesh::RegenBlockFiniteData::Update() {
    this->lifeStringStr.str("");

    // Create a string for the life percentage of the block
    int lifePercentage = static_cast<int>(ceilf(this->block.GetCurrentLifePercent()));
    this->lifeStringStr << lifePercentage;
}

void RegenBlockMesh::RegenBlockFiniteData::DrawLifeInfo() {

    const Point2D& blockCenter = this->block.GetCenter();

    // Lerp the colour between red (0%) and green (100%)
    Colour currColour = NumberFuncs::LerpOverFloat<Colour>(0.0f, 100.0f, Colour(1, 0, 0), 
        Colour(0, 1, 0), this->block.GetCurrentLifePercent());
    glColor4f(currColour.R(), currColour.G(), currColour.B(), this->alpha);

    // Draw the percentage first...
	glPushMatrix();
    glTranslatef(blockCenter[0] + HALF_LIFE_DISPLAY_WIDTH - X_DISPLAY_BORDER - this->percentCharWidthWithScale,
        blockCenter[1] - (HALF_LIFE_DISPLAY_HEIGHT + (this->textScale*this->regenMesh->font->GetHeight() - LIFE_DISPLAY_HEIGHT) / 2.0f),
        RegenBlockMesh::CENTER_TO_DISPLAY_DEPTH + EPSILON);

    // The scale will fill the text to the display section of the block
    glScalef(this->textScale, this->textScale, 1.0f);
    
    // This keeps the % in a constant position despite the actual percentage number
    this->regenMesh->font->BasicPrint("%");
    glPopMatrix();
    
    // Draw the actual percentage number...
    glPushMatrix();
    glTranslatef(blockCenter[0] + HALF_LIFE_DISPLAY_WIDTH - X_DISPLAY_BORDER - this->percentCharWidthWithScale - (this->textScale* this->regenMesh->font->GetWidth(this->lifeStringStr.str())), 
        blockCenter[1] - (HALF_LIFE_DISPLAY_HEIGHT + (this->textScale*this->regenMesh->font->GetHeight() - LIFE_DISPLAY_HEIGHT) / 2.0f),
        RegenBlockMesh::CENTER_TO_DISPLAY_DEPTH + EPSILON);
    
    // The scale will fill the text to the display section of the block
    glScalef(this->textScale, this->textScale, 1.0f);
    this->regenMesh->font->BasicPrint(this->lifeStringStr.str());

    glPopMatrix();
}

RegenBlockMesh::RegenBlockInfiniteData::RegenBlockInfiniteData(RegenBlockMesh* regenMesh,
                                                               const RegenBlock& block) : 
BlockData(regenMesh, block) {
}

void RegenBlockMesh::RegenBlockInfiniteData::DrawLifeInfo() {
    static const float INFINITY_WIDTH       = LIFE_DISPLAY_WIDTH - 2*X_DISPLAY_BORDER;
    static const float HALF_INFINITY_WIDTH  = INFINITY_WIDTH / 2.0f; 
    static const float INFINITY_HEIGHT      = HALF_INFINITY_WIDTH;
    static const float HALF_INFINITY_HEIGHT = INFINITY_HEIGHT / 2.0f;
    
    const Point2D& blockCenter = this->block.GetCenter();

    glPushMatrix();

    // Translate to the bottom left corner of the display section of the block
    glTranslatef(blockCenter[0] - HALF_LIFE_DISPLAY_WIDTH + X_DISPLAY_BORDER + HALF_INFINITY_WIDTH, 
        blockCenter[1] - (HALF_LIFE_DISPLAY_HEIGHT + (INFINITY_HEIGHT - LIFE_DISPLAY_HEIGHT) / 2.0f) + HALF_INFINITY_HEIGHT,
        RegenBlockMesh::CENTER_TO_DISPLAY_DEPTH + EPSILON);
    glScalef(INFINITY_WIDTH, INFINITY_HEIGHT, 1.0f);

    glColor4f(1.0f, 1.0f, 1.0f, this->alpha);
    this->regenMesh->infinityTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    this->regenMesh->infinityTex->UnbindTexture();

    glPopMatrix();
}