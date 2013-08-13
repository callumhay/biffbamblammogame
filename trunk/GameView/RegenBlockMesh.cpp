/**
 * RegenBlockMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

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

Colour RegenBlockMesh::GetColourFromLifePercentage(float lifePercentage) {
    assert(lifePercentage >= 0.0f && lifePercentage <= RegenBlock::MAX_LIFE_POINTS);
    return NumberFuncs::LerpOverFloat<Colour>(0.0f, RegenBlock::MAX_LIFE_POINTS, Colour(1, 0, 0), 
        Colour(0, 1, 0), lifePercentage);
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

    std::pair<BlockCollectionIter, bool> insertResult = 
        this->blocks.insert(std::make_pair(block, data));
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

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);

	// Draw each regen block
    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {

        RegenBlock* currBlock    = iter->first;
        BlockData* currBlockData = iter->second;

        const Point2D& blockCenter = currBlock->GetCenter();
        
        glPushMatrix();
        glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
        glColor4f(1,1,1,1);
        this->baseMetalMaterialGrp->GetMaterial()->GetProperties()->diffuse = 
            currBlockData->GetCurrBaseMaterialColour(this->initialBaseMetalDiffuseColour);
        this->baseMetalMaterialGrp->Draw(camera, keyLight, fillLight, ballLight);
        glPopMatrix();

    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   

    // Draw the block info (life/infinity symbol)
    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
    	
        RegenBlock* currBlock    = iter->first;
        BlockData* currBlockData = iter->second;
        
        // Draw information on the block (life amount or infinity symbol)
        currBlockData->DrawLifeInfo(camera, keyLight, fillLight, ballLight);

        // Regenerate and tick the block and its visual data...
        currBlock->Regen(dT);
        currBlockData->Tick(dT);
    }

    glPopAttrib();
}

void RegenBlockMesh::SetAlphaMultiplier(float alpha) {
    this->blockMesh->SetAlpha(alpha);
    for (BlockCollectionConstIter iter = this->blocks.begin(); iter != this->blocks.end(); ++iter) {
        BlockData* currBlockData = iter->second;
        currBlockData->SetAlpha(alpha);
    }
}

void RegenBlockMesh::UpdateRegenBlock(const RegenBlock* block, bool gotHurt) {
    BlockCollectionConstIter findIter = this->blocks.find(const_cast<RegenBlock*>(block));
    assert(findIter != this->blocks.end());
    BlockData* data = findIter->second;
    assert(data != NULL);
    data->Update(gotHurt);
}

void RegenBlockMesh::LoadMesh() {

    assert(this->blockMesh == NULL);
    this->blockMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->REGEN_BLOCK_MESH);
    assert(this->blockMesh != NULL);

    const std::map<std::string, MaterialGroup*>& matGrps = this->blockMesh->GetMaterialGroups();
    this->materialGroups.insert(matGrps.begin(), matGrps.end());
    
	std::map<std::string, MaterialGroup*>::iterator findIter = this->materialGroups.find(
        GameViewConstants::GetInstance()->REGEN_BLOCK_BASE_METAL_MATGRP);
	assert(findIter != this->materialGroups.end());
	
	this->baseMetalMaterialGrp = findIter->second;
	this->materialGroups.erase(findIter);
    
    this->initialBaseMetalDiffuseColour = this->baseMetalMaterialGrp->GetMaterial()->GetProperties()->diffuse;

    assert(this->materialGroups.size() > 0);
    assert(this->baseMetalMaterialGrp != NULL);
}

RegenBlockMesh::BlockData::BlockData(RegenBlockMesh* regenMesh, const RegenBlock& block) :
regenMesh(regenMesh), block(block), alpha(1.0f) {

    this->hurtRedFlashAnim.ClearLerp();
    this->hurtRedFlashAnim.SetInterpolantValue(0.0f);
    this->hurtRedFlashAnim.SetRepeat(false);
}

void RegenBlockMesh::BlockData::Update(bool gotHurt) {
    if (gotHurt && this->hurtRedFlashAnim.GetInterpolantValue() == 0.0f) {
        std::vector<float> values;
        values.reserve(3);
        values.push_back(0.0f);
        values.push_back(1.0f);
        values.push_back(0.0f);
        std::vector<double> times;
        times.reserve(3);
        times.push_back(0.0);
        times.push_back(0.15);
        times.push_back(0.3);

        this->hurtRedFlashAnim.SetLerp(times, values);
        this->hurtRedFlashAnim.SetRepeat(false);
        this->hurtRedFlashAnim.SetInterpolantValue(0.0f);
    }
}

void RegenBlockMesh::BlockData::Tick(double dT) {
    this->hurtRedFlashAnim.Tick(dT);
}

const float RegenBlockMesh::RegenBlockFiniteData::MAX_LIFE_PERCENT_FOR_FLASHING = 40.0f;

RegenBlockMesh::RegenBlockFiniteData::RegenBlockFiniteData(RegenBlockMesh* regenMesh, 
                                                           const RegenBlock& block) :
BlockData(regenMesh, block) {

    this->textScale = RegenBlockMesh::GenerateFontDisplayScale();
    this->percentCharWidthWithScale = this->textScale * RegenBlockMesh::GetDisplayFont()->GetWidth("%");
    this->lifeStringStr << "100";

    std::vector<float> values;
    values.reserve(3);
    values.push_back(0.0f);
    values.push_back(0.5f);
    values.push_back(0.0f);
    std::vector<double> times;
    times.reserve(3);
    times.push_back(0.0);
    times.push_back(0.5);
    times.push_back(1.0);
    this->aboutToDieRedFlashAnim.SetLerp(times, values);
    this->aboutToDieRedFlashAnim.SetRepeat(true);
    this->aboutToDieRedFlashAnim.SetInterpolantValue(0.0f);
}

void RegenBlockMesh::RegenBlockFiniteData::Update(bool gotHurt) {
    UNUSED_PARAMETER(gotHurt);

    this->lifeStringStr.str("");

    // Create a string for the life percentage of the block
    int lifePercentage = this->block.GetCurrentLifePercentInt();
    this->lifeStringStr << lifePercentage;

    RegenBlockMesh::BlockData::Update(gotHurt);
}

void RegenBlockMesh::RegenBlockFiniteData::Tick(double dT) {
    if (this->block.GetCurrentLifePercent() <= MAX_LIFE_PERCENT_FOR_FLASHING) {
        this->aboutToDieRedFlashAnim.Tick(dT);
    }
    RegenBlockMesh::BlockData::Tick(dT);
}

void RegenBlockMesh::RegenBlockFiniteData::DrawLifeInfo(const Camera& camera, const BasicPointLight& keyLight,
                                                        const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(ballLight);

    const Point2D& blockCenter = this->block.GetCenter();

    // Lerp the colour between red (0%) and green (100%)
    Colour currColour = RegenBlockMesh::GetColourFromLifePercentage(this->block.GetCurrentLifePercent());
    float redColourAdd = this->hurtRedFlashAnim.GetInterpolantValue();
    float lightMultiplier = std::min<float>(1.0f, fillLight.GetDiffuseColour().GetLuminance() + keyLight.GetDiffuseColour().GetLuminance());

    glColor4f(
        lightMultiplier * std::min<float>(1.0f, redColourAdd + currColour.R()), 
        lightMultiplier * std::max<float>(0.0f, currColour.G() - redColourAdd), 
        lightMultiplier * std::max<float>(0.0f, currColour.B() - redColourAdd), this->alpha);

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
    
    glTranslatef(blockCenter[0] + HALF_LIFE_DISPLAY_WIDTH - X_DISPLAY_BORDER - this->percentCharWidthWithScale - 
        (this->textScale* this->regenMesh->font->GetWidth(this->lifeStringStr.str())), 
        blockCenter[1] - (HALF_LIFE_DISPLAY_HEIGHT + (this->textScale*this->regenMesh->font->GetHeight() - LIFE_DISPLAY_HEIGHT) / 2.0f),
        RegenBlockMesh::CENTER_TO_DISPLAY_DEPTH + EPSILON);
    
    // The scale will fill the text to the display section of the block
    glScalef(this->textScale, this->textScale, 1.0f);
    this->regenMesh->font->BasicPrint(this->lifeStringStr.str());

    glPopMatrix();
}

Colour RegenBlockMesh::RegenBlockFiniteData::GetCurrBaseMaterialColour(const Colour& baseColour) const {
    Colour result = baseColour;
    if (this->block.GetCurrentLifePercent() <= MAX_LIFE_PERCENT_FOR_FLASHING) {
        result = Colour(std::min<float>(1.0f, result.R() + this->aboutToDieRedFlashAnim.GetInterpolantValue()), 0.5f*result.G(), 0.5f*result.B());
    }
    if (this->hurtRedFlashAnim.GetInterpolantValue() > 0.0f) {
        result = Colour(
            std::min<float>(1.0f, result.R() + this->hurtRedFlashAnim.GetInterpolantValue()),
            std::max<float>(0.0f, result.G() - this->hurtRedFlashAnim.GetInterpolantValue()),
            std::max<float>(0.0f, result.B() - this->hurtRedFlashAnim.GetInterpolantValue()));
    }

    return result;
}

RegenBlockMesh::RegenBlockInfiniteData::RegenBlockInfiniteData(RegenBlockMesh* regenMesh,
                                                               const RegenBlock& block) : 
BlockData(regenMesh, block) {
}

void RegenBlockMesh::RegenBlockInfiniteData::DrawLifeInfo(const Camera& camera, const BasicPointLight& keyLight,
                                                          const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    UNUSED_PARAMETER(camera);
    UNUSED_PARAMETER(ballLight);

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

    float lightMultiplier = std::min<float>(1.0f, fillLight.GetDiffuseColour().GetLuminance() + keyLight.GetDiffuseColour().GetLuminance());
    float redColour = this->hurtRedFlashAnim.GetInterpolantValue();
    
    if (redColour > 0.0f) {
        float invRedColour = lightMultiplier*std::max<float>(0.0f, 1.0f - redColour);
        glColor4f(lightMultiplier, invRedColour, invRedColour, this->alpha);
    }
    else {
        glColor4f(lightMultiplier, lightMultiplier, lightMultiplier, this->alpha);
    }

    this->regenMesh->infinityTex->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();

    glPopMatrix();
}

Colour RegenBlockMesh::RegenBlockInfiniteData::GetCurrBaseMaterialColour(const Colour& baseColour) const {
    Colour result = baseColour;
    if (this->hurtRedFlashAnim.GetInterpolantValue() > 0.0f) {
        result = Colour(std::min<float>(1.0f, result.R() + this->hurtRedFlashAnim.GetInterpolantValue()), 0.5f*result.G(), 0.5f*result.B());
    }

    return result;
}