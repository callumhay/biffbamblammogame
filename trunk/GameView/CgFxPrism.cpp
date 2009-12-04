#include "CgFxPrism.h"
#include "GameViewConstants.h"

const std::string CgFxPrism::DEFAULT_PRISM_TECHNIQUE_NAME = "Prism";

CgFxPrism::CgFxPrism(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PRISM_SHADER, properties) {

	this->currTechnique = this->techniques[DEFAULT_PRISM_TECHNIQUE_NAME];

}

CgFxPrism::~CgFxPrism() {
}

//void CgFxPrism::SetupBeforePasses(const Camera& camera) {
//	CgFxMaterialEffect::SetupBeforePasses(camera);
//}