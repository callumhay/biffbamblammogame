/**
 * NouveauWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "NouveauWorldAssets.h"
#include "GameViewConstants.h"
#include "DecoSkybox.h"
#include "GameAssets.h"

#include "../ESPEngine/ESP.h"
#include "../ResourceManager.h"

// Basic constructor: Load all the basic assets for the deco world...
NouveauWorldAssets::NouveauWorldAssets(GameAssets* assets) : 
GameWorldAssets(assets, new DecoSkybox(),
        ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BACKGROUND_MESH),
		ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_PADDLE_MESH),
		ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NOUVEAU_BLOCK_MESH)),
fadeEffector() {

    std::vector<ColourRGBA> fadeColours;
    fadeColours.reserve(3);
    fadeColours.push_back(ColourRGBA(1,1,1,0));
    fadeColours.push_back(ColourRGBA(1,1,1,1));
    fadeColours.push_back(ColourRGBA(1,1,1,0));
    this->fadeEffector.SetColours(fadeColours);

    // Change the positions of the background lights to something that compliements the background more
    assets->GetLightAssets()->SetBackgroundLightDefaults(
        BasicPointLight(Point3D(60.0f, 60.0f, 60.0f), Colour(0.4f, 0.45f, 0.45f), 0.06f),
        BasicPointLight(Point3D(30.0f, 11.0f, -15.0f), Colour(0.8f, 0.8f, 0.8f),  0.01f));

    // Change the default values for drawing outlines
    this->outlineMinDistance = 0.01f;
    this->outlineMaxDistance = 21.0f;
    this->outlineContrast    = 2.25f;
    this->outlineOffset      = 0.8f;


    // Colour Pallet for art nouveau...
    std::vector<Colour> nouveauColourPalette;
    nouveauColourPalette.reserve(26);
    nouveauColourPalette.push_back(Colour(0x787E58)); // Light olive
    nouveauColourPalette.push_back(Colour(0xD4CFBC)); // Light Rose
    nouveauColourPalette.push_back(Colour(0x4C4441)); // Dark mauve / grey
    nouveauColourPalette.push_back(Colour(0x916447)); // Cream brown with slight peach tinge
    nouveauColourPalette.push_back(Colour(0x76704C)); // Medium olive
    nouveauColourPalette.push_back(Colour(0x5E210C)); // Dark Rosey Mauve
    nouveauColourPalette.push_back(Colour(0x62383C)); // Purple-Mauve
    nouveauColourPalette.push_back(Colour(0x2F3F59)); // Deep navy blue
    nouveauColourPalette.push_back(Colour(0x535F31)); // Deep olive
    nouveauColourPalette.push_back(Colour(0x859980)); // Light turquoise
    nouveauColourPalette.push_back(Colour(0x9AA69C)); // Light blueish-grey
    nouveauColourPalette.push_back(Colour(0xBDAC80)); // Skin
    nouveauColourPalette.push_back(Colour(0xA78174)); // Rose
    nouveauColourPalette.push_back(Colour(0x8A6618)); // Copper orange
    nouveauColourPalette.push_back(Colour(0x67653C)); // Dark olive
    nouveauColourPalette.push_back(Colour(0x99482A)); // Rosey rich orangish-red
    nouveauColourPalette.push_back(Colour(0x820002)); // Rich Mulberry Crimson
    nouveauColourPalette.push_back(Colour(0x9F5951)); // Pink
    nouveauColourPalette.push_back(Colour(0x383C21)); // Deep navy olive
    nouveauColourPalette.push_back(Colour(0x525A4B)); // Cadet olive-blue-steel
    nouveauColourPalette.push_back(Colour(0xB78965)); // Light pink
    nouveauColourPalette.push_back(Colour(0x676D5F)); // Deep oliveish-grey
    nouveauColourPalette.push_back(Colour(0xB9A568)); // Creamy tan
    nouveauColourPalette.push_back(Colour(0xCBAA41)); // Goldenrod
    nouveauColourPalette.push_back(Colour(0xA88952)); // Light Tan
    nouveauColourPalette.push_back(Colour(0x675733)); // Brownish Tan
    
    this->UpdateColourChangeList(nouveauColourPalette);

    // Setup each of the curves...
    {
        Bezier* curve1 = new Bezier();
        curve1->SetDrawPosition(0,0);
        curve1->MoveToCubicBezier(true, 7.76,11.307,14.208,22.208,25.05,30.902);
        curve1->MoveToCubicBezier(true, 10.231,8.204,22.492,15.894,35.009,20.004);
        curve1->MoveToCubicBezier(true, 22.036,7.237,49.242,2.166,70.194, -6.409);
        curve1->MoveToCubicBezier(true, 42.213, -17.276,69.397,-61.037,69.026,-106.178);
        curve1->MoveToCubicBezier(true, -0.2,-24.406,-11.258,-46.249,-29.815,-61.877);
        curve1->MoveToCubicBezier(true, -19.891,-16.749,-46.759,-20.337,-72.028,-18.108);
        curve1->MoveToCubicBezier(true, -21.038,1.856,-41.652,15.652,-49.703,35.375);
        curve1->MoveToCubicBezier(true, -8.006,19.61,-8.552,48.262,4.466,65.843);
        curve1->MoveToCubicBezier(true, 19.719,26.63,61.016,16.241,79.193,-6.42);
        curve1->MoveToCubicBezier(true, 9.866,-12.3,14.43,-32.399,4.727,-46.088);
        curve1->MoveToCubicBezier(true, -5.931,-8.366,-16.467,-11.394,-26.092,-12.665);
        curve1->MoveToCubicBezier(true, -9.062,-1.197,-20.198,-1.799,-27.939,3.94);
        curve1->MoveToCubicBezier(true, -13.045,9.67,-13.367,28.433,-1.797,39.332);
        curve1->MoveToCubicBezier(true, 5.404,5.092,12.844,6.943,20.078,5.99);
        curve1->MoveToCubicBezier(true, 9.585,-1.264,14.968,-7.558,17.857,-16.413);
        curve1->MoveToCubicBezier(true, 0.114,-0.35,-0.38,-0.512,-0.619,-0.534);
        curve1->MoveToCubicBezier(true, -9.26,-0.869,-18.646,-1.904,-20.297,-12.812);
        curve1->SmartSample(125);
        curve1->Normalize();
        this->curves.push_back(curve1);
    }
    {

        Bezier* curve2 = new Bezier();
        curve2->SetDrawPosition(0,0);
        curve2->MoveToCubicBezier(true, 10.77,-2.98,20.121,-12.031,28.989,-18.283);
        curve2->MoveToCubicBezier(true, 10.665,-7.518,23.071,-13.309,35.545,-16.604);
        curve2->MoveToCubicBezier(true, 8.021,-2.119,15.687,-3.91,23.659,-5.103);
        curve2->MoveToCubicBezier(true, 8.754,-1.31,17.819,-0.41,26.83,-0.92);
        curve2->MoveToCubicBezier(true, 15.398,-0.87,31.702,0.365,45.445,-4.987);
        curve2->MoveToCubicBezier(true, 18.438,-7.183,32.966,-41.397,9.712,-52.118);
        curve2->MoveToCubicBezier(true, -6.973,-3.215,-16.803,-3.819,-23.185,1.077);
        curve2->MoveToCubicBezier(true, -6.736,5.168,-7.226,15.082,-0.924,20.854);
        curve2->MoveToCubicBezier(true, 3.274,2.999,8.043,4.492,12.288,2.756);
        curve2->MoveToCubicBezier(true, 3.782,-1.553,13.098,-10.794,5.366,-13.051);
        curve2->MoveToCubicBezier(true, -4.013,-1.172,-7.978,3.703,-10.545,-1.804);

        curve2->SmartSample(100);
        curve2->Normalize();
        this->curves.push_back(curve2);
    }
    {
        Bezier* curve3 = new Bezier();
        curve3->SetDrawPosition(0,0);
        curve3->MoveToCubicBezier(true, 26.547,2.028,59.37,5.688,83.739,-7.693);
        curve3->MoveToCubicBezier(true, 25.593,-14.052,1.496,-38.267,-11.9,-51.443);
        curve3->MoveToCubicBezier(true, -12.139,-11.938,-24.909,-23.202,-36.797,-35.396);
        curve3->MoveToCubicBezier(true, -9.826,-10.08,-21.617,-20.895,-24.941,-35.137);
        curve3->MoveToCubicBezier(true, -5.436,-23.298,-1.936,-60.845,24.904,-70.001);
        curve3->MoveToCubicBezier(true, 8.705,-2.97,21.024,-3.479,30.019,-1.717);
        curve3->MoveToCubicBezier(true, 11.251,2.204,24.003,11.686,28.158,21.841);
        curve3->MoveToCubicBezier(true, 9.369,22.896,-3.31,61.671,-33.206,56.369);
        curve3->MoveToCubicBezier(true, -18.063,-3.203,-29.003,-27.646,-20.762,-43.553);
        curve3->MoveToCubicBezier(true, 6.094,-11.762,28.765,-12.605,27.849,3.966);
        curve3->MoveToCubicBezier(true, -0.347,6.289,-6.359,6.278,-9.236,10.936);
        curve3->MoveToCubicBezier(true, -7.198,11.654,13.195,20.758,15.811,6.373);

        curve3->SmartSample(100);
        curve3->Normalize();
        this->curves.push_back(curve3);
    }
    {
        Bezier* curve4 = new Bezier();
        curve4->SetDrawPosition(0,0);
        curve4->MoveToCubicBezier(true, 0.375,-15.946,4.697,-32.64,14.152,-45.712);
        curve4->MoveToCubicBezier(true, 13.751,-19.014,39.178,-22.194,60.394,-27.016);
        curve4->MoveToCubicBezier(true, 11.666,-2.652,24.403,-4.834,35.454,-10.455);
        curve4->MoveToCubicBezier(true, 12.203,-6.207,25.533,-14.361,31.519,-27.225);
        curve4->MoveToCubicBezier(true, 5.468,-11.749,6.217,-26.402,4.002,-39.042);
        curve4->MoveToCubicBezier(true, -1.855,-10.589,-7.825,-30.788,-19.993,-34.296);
        curve4->MoveToCubicBezier(true, -10.754,-3.1,-12.217,12.667,-12.063,19.749);
        curve4->MoveToCubicBezier(true, 0.242,11.227,3.636,23.411,11.838,31.466);
        curve4->MoveToCubicBezier(true, 9.449,9.279,23.511,13.951,36.415,15.358);
        curve4->MoveToCubicBezier(true, 7.141,0.778,13.067,0.759,19.546,-2.021);
        curve4->MoveToCubicBezier(true, 10.982,-4.714,20.372,-11.744,24.091,-23.637);
        curve4->MoveToCubicBezier(true, 6.729,-21.519,-3.845,-63.814,-34.101,-56.679);
        curve4->MoveToCubicBezier(true, -4.954,1.168,-13.749,5.548,-16.455,10.099);
        curve4->MoveToCubicBezier(true, -9.209,15.49,4.828,34.806,22.069,34.223);
        curve4->MoveToCubicBezier(true, 3.929,-0.133,7.514,-1.433,9.905,-4.674);
        curve4->MoveToCubicBezier(true, 1.857,-2.523,2.856,-7.695,3.071,-10.713);
        curve4->MoveToCubicBezier(true, -6.146,-1.585,-20.292,5.795,-19.844,-6.245);

        curve4->SmartSample(125);
        curve4->Normalize();
        this->curves.push_back(curve4);
    }


    {
        // Setup each of the background emitters...
        const ESPInterval SPAWN_INTERVAL(0.75f, 1.5f);
        const ESPInterval SPEED_INTERVAL(0.0f, 0.2f);
        const ESPInterval LIFE_INTERVAL(5.0f, 20.0f);
        const ESPInterval SIZE_INTERVAL(5.0f, 28.0f);
        const ESPInterval ROTATION_INTERVAL(0.0f, 359.999f);
        const ESPInterval ANIMATION_INTERVAL(1.0f, LIFE_INTERVAL.minValue);
        
        const int NUM_CURVES_PER_MID_EMITTER  = 10;
        const int NUM_CURVES_PER_SIDE_EMITTER = 12;
        const int NUM_CURVES_PER_GAP_EMITTER  = 5;

        float minMaxLineWidth[2];
        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, minMaxLineWidth);
        const ESPInterval LINE_THICKNESS_INTERVAL(std::max<float>(minMaxLineWidth[0], 1.0f), std::min<float>(minMaxLineWidth[1], 4.0f));

        const float SIDE_EMITTER_MAX_X = 60;
        const float SIDE_EMITTER_MIN_X = 23;
        const float SIDE_EMITTER_MIN_Y = 25;
        const float SIDE_EMITTER_MAX_Y = 55;

        const float MID_EMITTER_MIN_Y = 30;
        const float MID_EMITTER_MAX_Y = 60;

        this->leftSideEmitter.SetSpawnDelta(SPAWN_INTERVAL);
        this->leftSideEmitter.SetInitialSpd(SPEED_INTERVAL);
        this->leftSideEmitter.SetParticleLife(LIFE_INTERVAL);
        this->leftSideEmitter.SetParticleSize(SIZE_INTERVAL);
        this->leftSideEmitter.SetParticleRotation(ROTATION_INTERVAL);
        this->leftSideEmitter.SetEmitVolume(Point3D(-SIDE_EMITTER_MAX_X, SIDE_EMITTER_MIN_Y, -70), Point3D(-SIDE_EMITTER_MIN_X, SIDE_EMITTER_MAX_Y, -50));
        this->leftSideEmitter.SetEmitDirection(Vector3D(0, 1, 0));
        this->leftSideEmitter.SetParticleColour(ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1));
        this->leftSideEmitter.SetRandomCurveParticles(NUM_CURVES_PER_SIDE_EMITTER, LINE_THICKNESS_INTERVAL, this->curves, ANIMATION_INTERVAL);
        this->leftSideEmitter.AddEffector(&this->fadeEffector);

        this->centerEmitter.SetSpawnDelta(SPAWN_INTERVAL);
        this->centerEmitter.SetInitialSpd(SPEED_INTERVAL);
        this->centerEmitter.SetParticleLife(LIFE_INTERVAL);
        this->centerEmitter.SetParticleSize(SIZE_INTERVAL);
        this->centerEmitter.SetParticleRotation(ROTATION_INTERVAL);
        this->centerEmitter.SetEmitVolume(Point3D(-20, MID_EMITTER_MIN_Y, -70), Point3D(20, MID_EMITTER_MAX_Y, -55));
        this->centerEmitter.SetEmitDirection(Vector3D(0, 1, 0));
        this->centerEmitter.SetParticleColour(ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1));
        this->centerEmitter.SetRandomCurveParticles(NUM_CURVES_PER_MID_EMITTER, LINE_THICKNESS_INTERVAL, this->curves, ANIMATION_INTERVAL);
        this->centerEmitter.AddEffector(&this->fadeEffector);

        this->rightSideEmitter.SetSpawnDelta(SPAWN_INTERVAL);
        this->rightSideEmitter.SetInitialSpd(SPEED_INTERVAL);
        this->rightSideEmitter.SetParticleLife(LIFE_INTERVAL);
        this->rightSideEmitter.SetParticleSize(SIZE_INTERVAL);
        this->rightSideEmitter.SetParticleRotation(ROTATION_INTERVAL);
        this->rightSideEmitter.SetEmitVolume(Point3D(SIDE_EMITTER_MIN_X, SIDE_EMITTER_MIN_Y, -70), Point3D(SIDE_EMITTER_MAX_X, SIDE_EMITTER_MAX_Y, -50));
        this->rightSideEmitter.SetEmitDirection(Vector3D(0, 1, 0));
        this->rightSideEmitter.SetParticleColour(ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1));
        this->rightSideEmitter.SetRandomCurveParticles(NUM_CURVES_PER_SIDE_EMITTER, LINE_THICKNESS_INTERVAL, this->curves, ANIMATION_INTERVAL);
        this->rightSideEmitter.AddEffector(&this->fadeEffector);
    
        this->leftGapEmitter.SetSpawnDelta(SPAWN_INTERVAL);
        this->leftGapEmitter.SetInitialSpd(SPEED_INTERVAL);
        this->leftGapEmitter.SetParticleLife(LIFE_INTERVAL);
        this->leftGapEmitter.SetParticleSize(SIZE_INTERVAL);
        this->leftGapEmitter.SetParticleRotation(ROTATION_INTERVAL);
        this->leftGapEmitter.SetEmitVolume(Point3D(-25.0f, -20, -60), Point3D(-19, 30, -45));
        this->leftGapEmitter.SetEmitDirection(Vector3D(0, 1, 0));
        this->leftGapEmitter.SetParticleColour(ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1));
        this->leftGapEmitter.SetRandomCurveParticles(NUM_CURVES_PER_GAP_EMITTER, LINE_THICKNESS_INTERVAL, this->curves, ANIMATION_INTERVAL);
        this->leftGapEmitter.AddEffector(&this->fadeEffector);

        this->rightGapEmitter.SetSpawnDelta(SPAWN_INTERVAL);
        this->rightGapEmitter.SetInitialSpd(SPEED_INTERVAL);
        this->rightGapEmitter.SetParticleLife(LIFE_INTERVAL);
        this->rightGapEmitter.SetParticleSize(SIZE_INTERVAL);
        this->rightGapEmitter.SetParticleRotation(ROTATION_INTERVAL);
        this->rightGapEmitter.SetEmitVolume(Point3D(19, -20, -60), Point3D(25, 30, -47));
        this->rightGapEmitter.SetEmitDirection(Vector3D(0, 1, 0));
        this->rightGapEmitter.SetParticleColour(ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1), ESPInterval(1,1));
        this->rightGapEmitter.SetRandomCurveParticles(NUM_CURVES_PER_GAP_EMITTER, LINE_THICKNESS_INTERVAL, this->curves, ANIMATION_INTERVAL);
        this->rightGapEmitter.AddEffector(&this->fadeEffector);

        // Tick all the emitters for a bit to get them to look like they've been spawning for awhile
        for (unsigned int i = 0; i < 60; i++) {
            this->leftSideEmitter.Tick(0.5);
            this->centerEmitter.Tick(0.5);
            this->rightSideEmitter.Tick(0.5);
            this->leftGapEmitter.Tick(0.5);
            this->rightGapEmitter.Tick(0.5);
        }
    }
}

NouveauWorldAssets::~NouveauWorldAssets() {
    // Delete all the curves...
    for (int i = 0; i < static_cast<int>(this->curves.size()); i++) {
        Bezier* curve = this->curves[i];
        delete curve;
        curve = NULL;
    }
    this->curves.clear();
}

void NouveauWorldAssets::Tick(double dT) {

    this->leftSideEmitter.Tick(dT);
    this->centerEmitter.Tick(dT);
    this->rightSideEmitter.Tick(dT);
    this->leftGapEmitter.Tick(dT);
    this->rightGapEmitter.Tick(dT);

	GameWorldAssets::Tick(dT);
}

void NouveauWorldAssets::DrawBackgroundModel(const Camera& camera, const BasicPointLight& bgKeyLight,
                                             const BasicPointLight& bgFillLight) {

    // Draw the curve emitters behind the background model
    this->leftSideEmitter.Draw(camera);
    this->centerEmitter.Draw(camera);
    this->rightSideEmitter.Draw(camera);
    this->leftGapEmitter.Draw(camera);
    this->rightGapEmitter.Draw(camera);

	const Colour& currBGModelColour = this->currBGMeshColourAnim.GetInterpolantValue();
	glPushAttrib(GL_CURRENT_BIT);
	glColor4f(currBGModelColour.R(), currBGModelColour.G(), currBGModelColour.B(), this->bgFadeAnim.GetInterpolantValue());
	this->background->Draw(camera, bgKeyLight, bgFillLight);
	glPopAttrib();
}

void NouveauWorldAssets::DrawBackgroundEffects(const Camera& camera) {
    UNUSED_PARAMETER(camera);

	//float currBGAlpha = this->bgFadeAnim.GetInterpolantValue();
	//if (currBGAlpha == 0) {
	//	return;
	//}
	
    // TODO?
}
