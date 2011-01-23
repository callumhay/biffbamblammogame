/**
 * TextLabel.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
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
	this->lastRasterWidth = this->font->GetWidth(this->text);
}

TextLabel2DFixedWidth::TextLabel2DFixedWidth(const TextureFontSet* font, 
                                             float width, const std::string& text) : 
TextLabel(font), fixedWidth(width), lineSpacing(8.0f) {
    assert(font != NULL);
    this->SetText(text);
}

TextLabel2DFixedWidth::~TextLabel2DFixedWidth() {
}

void TextLabel2DFixedWidth::SetText(const std::string& text) {
    // Parse the text based on the current width...
    assert(this->font != NULL);
    this->textLines = this->font->ParseTextToWidth(text, this->fixedWidth, this->scale);
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
    for (std::vector<std::string>::const_iterator iter = this->textLines.begin(); 
         iter != this->textLines.end(); ++iter) {

        const std::string& currLineTxt = *iter;
        this->font->OrthoPrint(currTextTopLeftPos, currLineTxt, false, this->scale);
        currTextTopLeftPos[1] -= (this->scale * this->font->GetHeight() + this->lineSpacing);
    }
}