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

TextLabel2D::TextLabel2D() : 
colour(ColourRGBA(0, 0, 0, 1)), topLeftCorner(Point2D(0, 0)), font(NULL), text(""),
lastRasterWidth(0.0f) {
}

TextLabel2D::TextLabel2D(const TextureFontSet* font, const std::string& text) : 
colour(ColourRGBA(0, 0, 0, 1)), topLeftCorner(Point2D(0, 0)), font(font), text(text),
lastRasterWidth(font->GetWidth(text)) {
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
		this->font->OrthoPrint(Point3D(this->topLeftCorner, depth - 0.01f) + Vector3D(dropAmt, -dropAmt, 0), this->text, depthTestOn);
	}

	// Draw coloured text part
	glColor4f(this->colour.R(), this->colour.G(), this->colour.B(), this->colour.A());
	this->font->OrthoPrint(Point3D(this->topLeftCorner, depth), this->text, depthTestOn);
	this->lastRasterWidth = this->font->GetWidth(this->text);
}