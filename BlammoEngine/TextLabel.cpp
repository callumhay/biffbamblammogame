/**
 * TextLabel.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TextLabel.h"
#include "BasicIncludes.h"

TextLabel::TextLabel() : font(NULL), colour(ColourRGBA(0, 0, 0, 1)), 
topLeftCorner(Point2D(0, 0)), scale(1.0f) {
}

TextLabel::TextLabel(const TextLabel& copy) : scale(copy.scale), colour(copy.colour), dropShadow(copy.dropShadow),
topLeftCorner(copy.topLeftCorner), font(copy.font) {

}

TextLabel::TextLabel(const TextureFontSet* font) : font(font),
colour(ColourRGBA(0, 0, 0, 1)), topLeftCorner(Point2D(0, 0)), scale(1.0f) {
}

TextLabel::~TextLabel() {
}

TextLabel2D::TextLabel2D() : TextLabel(), text(""),
lastRasterWidth(0.0f) {
}

TextLabel2D::TextLabel2D(const TextureFontSet* font, const std::string& text) : TextLabel(font),
text(text), lastRasterWidth(font->GetWidth(text)) {
	assert(font != NULL);
}

TextLabel2D::TextLabel2D(const TextLabel2D& copy) :
TextLabel(copy), text(copy.text), lastRasterWidth(copy.lastRasterWidth) {
}

TextLabel2D::~TextLabel2D() {
}

/**
 * Draw the text label and any other effects attached to it.
 * Return: The length of the drawn label.
 */
void TextLabel2D::Draw(bool depthTestOn, float depth) {
	assert(this->font != NULL);

	// Draw drop shadow part
	if (this->dropShadow.isSet) {
		float dropAmt = static_cast<float>(this->GetHeight()) * this->dropShadow.amountPercentage;
		glColor4f(this->dropShadow.colour.R(), this->dropShadow.colour.G(), this->dropShadow.colour.B(), this->colour.A());
		this->font->OrthoPrint(Point3D(this->topLeftCorner, depth - 0.01f) + Vector3D(dropAmt, -dropAmt, 0), 
													 this->text, depthTestOn, this->scale);
	}

	// Draw coloured text part
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->colour.A());
	this->font->OrthoPrint(Point3D(this->topLeftCorner, depth), this->text, depthTestOn, this->scale);
	//this->lastRasterWidth = this->font->GetWidth(this->text);
}

void TextLabel2D::Draw(float rotationInDegs) {
	assert(this->font != NULL);

	// Draw drop shadow part
	if (this->dropShadow.isSet) {
		float dropAmt = static_cast<float>(this->GetHeight()) * this->dropShadow.amountPercentage;
		glColor4f(this->dropShadow.colour.R(), this->dropShadow.colour.G(), this->dropShadow.colour.B(), this->colour.A());
		this->font->OrthoPrint(Point3D(this->topLeftCorner, -0.01f) + Vector3D(dropAmt, -dropAmt, 0), 
                               this->text, rotationInDegs, this->scale);
	}

	// Draw coloured text part
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->colour.A());
    this->font->OrthoPrint(Point3D(this->topLeftCorner, 0.0f), this->text, rotationInDegs, this->scale);
	//this->lastRasterWidth = this->font->GetWidth(this->text);
}

void TextLabel2D::Draw3D(const Camera& camera, float rotationInDegs, float z) {

    // Setup a screen-aligned matrix...
	Vector3D alignNormalVec = -camera.GetNormalizedViewVector();
	Vector3D alignUpVec		 = camera.GetNormalizedUpVector();
	Vector3D alignRightVec	 = Vector3D::Normalize(Vector3D::cross(alignUpVec, alignNormalVec));
    Matrix4x4 personalAlignXF(alignRightVec, alignUpVec, alignNormalVec);

    float half3DRenderWidth  = TextureFontSet::TEXT_3D_SCALE * this->lastRasterWidth * 0.5f;
    float half3DRenderHeight = TextureFontSet::TEXT_3D_SCALE * this->GetHeight() * 0.5f;

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

	// If set, draw the drop shadow
	if (this->dropShadow.isSet) {
		float dropAmt = TextureFontSet::TEXT_3D_SCALE * this->lastRasterWidth * this->dropShadow.amountPercentage;
        float dsScale = this->dropShadow.scale * this->scale;
		glPushMatrix();
		glTranslatef(this->topLeftCorner[0] + dropAmt, this->topLeftCorner[1] - dropAmt, 0.0f); 
		glMultMatrixf(personalAlignXF.begin());
        glScalef(dsScale, dsScale, 1.0f);
		glRotatef(rotationInDegs, 0, 0, -1);
		glTranslatef(-half3DRenderWidth, -half3DRenderHeight, z);
        glColor4f(this->dropShadow.colour.R(), this->dropShadow.colour.G(), this->dropShadow.colour.B(), this->colour.A());
        this->font->Print(this->text);
		glPopMatrix();
	}

	// Draw the font itself
	glPushMatrix();
	glTranslatef(this->topLeftCorner[0], this->topLeftCorner[1], 0.0f);
	glMultMatrixf(personalAlignXF.begin());
	glRotatef(rotationInDegs, 0, 0, -1);
	glScalef(this->scale, this->scale, 1.0f);
	glTranslatef(-half3DRenderWidth, -half3DRenderHeight, z);
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->colour.A());
	this->font->Print(this->text);
	glPopMatrix();

    glPopAttrib();
}


TextLabel2DFixedWidth::TextLabel2DFixedWidth(const TextureFontSet* font, 
                                             float width, const std::string& text) : 
TextLabel(font), alignment(TextLabel2DFixedWidth::LeftAligned), fixedWidth(width), lineSpacing(8.0f) {
    assert(font != NULL);
    this->SetText(text);
}

TextLabel2DFixedWidth::TextLabel2DFixedWidth(const TextLabel2D& label) :
TextLabel(label), alignment(TextLabel2DFixedWidth::LeftAligned), fixedWidth(label.GetLastRasterWidth()), lineSpacing(8.0f) {
}

TextLabel2DFixedWidth::~TextLabel2DFixedWidth() {
}

void TextLabel2DFixedWidth::SetText(const std::string& text) {
    // Parse the text based on the current width...
    assert(this->font != NULL);
    this->textLines = this->font->ParseTextToWidth(text, this->fixedWidth, this->scale);
    if (this->textLines.empty()) { return; }

    this->currTextWidth = this->font->GetWidth(this->textLines[0]);
    for (size_t i = 1; i < this->textLines.size(); i++) {
        this->currTextWidth = std::max<float>(this->currTextWidth, this->font->GetWidth(this->textLines[i]));
    }
    this->currTextWidth *= this->scale;
}

std::string TextLabel2DFixedWidth::GetText() const {
    if (this->textLines.empty()) { return std::string(""); }

    std::string text = this->textLines[0];
    for (size_t i = 1; i < this->textLines.size(); i++) {
        text = text + std::string(" ") + this->textLines[i];
    }

    return text;
}

void TextLabel2DFixedWidth::Draw() {
	// Draw drop shadow part
	if (this->dropShadow.isSet) {
        float dropAmt = static_cast<float>(this->font->GetHeight()) * this->dropShadow.amountPercentage;
		glColor4f(this->dropShadow.colour.R(), this->dropShadow.colour.G(), this->dropShadow.colour.B(), this->colour.A());
        this->DrawTextLines(dropAmt, -dropAmt);
	}

	// Draw coloured text part
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->colour.A());
	this->DrawTextLines(0, 0);
}

void TextLabel2DFixedWidth::DrawTextLines(float xOffset, float yOffset) {
    Point3D currTextTopLeftPos(this->topLeftCorner[0] + xOffset, this->topLeftCorner[1] + yOffset, 0);

    switch (this->alignment) {
        case TextLabel2DFixedWidth::CenterAligned:
            {
                float maxWidth = 0.0f;
                for (std::vector<std::string>::const_iterator iter = this->textLines.begin(); 
                     iter != this->textLines.end(); ++iter) {
                    const std::string& currLineTxt = *iter;
                    maxWidth = std::max<float>(this->scale * this->font->GetWidth(currLineTxt), maxWidth);
                }

                Point3D currTopLeftCorner = currTextTopLeftPos;
                for (std::vector<std::string>::const_iterator iter = this->textLines.begin(); 
                     iter != this->textLines.end(); ++iter) {

                    const std::string& currLineTxt = *iter;
                    currTopLeftCorner[0] = currTextTopLeftPos[0] + ((maxWidth - (this->scale * this->font->GetWidth(currLineTxt))) / 2.0f);
                    this->font->OrthoPrint(currTopLeftCorner, currLineTxt, false, this->scale);
                    currTopLeftCorner[1] -= (this->scale * this->font->GetHeight() + this->lineSpacing);
                }
            }
            break;

        case TextLabel2DFixedWidth::LeftAligned:

            for (std::vector<std::string>::const_iterator iter = this->textLines.begin(); 
                 iter != this->textLines.end(); ++iter) {

                const std::string& currLineTxt = *iter;
                this->font->OrthoPrint(currTextTopLeftPos, currLineTxt, false, this->scale);
                currTextTopLeftPos[1] -= (this->scale * this->font->GetHeight() + this->lineSpacing);
            }

            break;

        case TextLabel2DFixedWidth::RightAligned:
            {
                float baseTopLeftX = currTextTopLeftPos[0] + this->fixedWidth;
                for (std::vector<std::string>::const_iterator iter = this->textLines.begin(); 
                     iter != this->textLines.end(); ++iter) {
                    
                    const std::string& currLineTxt = *iter;
                    currTextTopLeftPos[0] = baseTopLeftX - (this->scale * this->font->GetWidth(currLineTxt));
                    this->font->OrthoPrint(currTextTopLeftPos, currLineTxt, false, this->scale);
                    currTextTopLeftPos[1] -= (this->scale * this->font->GetHeight() + this->lineSpacing);
                }
            }

            break;

        default:
            assert(false);
            break;
    }
}