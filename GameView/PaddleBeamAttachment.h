/**
 * PaddleBeamAttachment.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLEBEAMATTACHMENT_H__
#define __PADDLEBEAMATTACHMENT_H__

#include "../BlammoEngine/Mesh.h"

class PlayerPaddle;

class PaddleBeamAttachment {
public:
    PaddleBeamAttachment();
    ~PaddleBeamAttachment();

	inline void Draw(const PlayerPaddle& p, const Camera& camera, CgFxEffectBase* replacementMat,
                     const BasicPointLight& paddleKeyLight, const BasicPointLight& paddleFillLight, 
                     const BasicPointLight& ballLight) {

        this->paddleBeamAttachmentMesh->Draw(camera, replacementMat, paddleKeyLight, paddleFillLight, ballLight);
        this->DrawEffects(p);
	}

private:
    static const float BIG_TO_SMALL_RAY_RATIO;
    const Colour DARK_WASHED_OUT_BEAM_COLOUR;
    const Colour BRIGHT_WASHED_OUT_BEAM_COLOUR;
    Mesh* paddleBeamAttachmentMesh;

    void DrawEffects(const PlayerPaddle& paddle);
    void DrawHighlightQuadVertices(float alpha, const Vector2D& rayUpDir, float halfOuterRayWidth, float rayLength);

    DISALLOW_COPY_AND_ASSIGN(PaddleBeamAttachment);
};
#endif // __PADDLEBEAMATTACHMENT_H__