#include "TextLabel.h"
#include "BasicIncludes.h"

TextLabel2D::TextLabel2D() : 
colour(Colour(0, 0, 0)), topLeftCorner(Point2D(0, 0)), font(NULL), text(""),
lastRasterWidth(0.0f) {
}

TextLabel2D::TextLabel2D(const TextureFontSet* font, const std::string& text) : 
colour(Colour(0, 0, 0)), topLeftCorner(Point2D(0, 0)), font(font), text(text),
lastRasterWidth(0.0f) {
	assert(font != NULL);
}

TextLabel2D::~TextLabel2D() {
}

/**
 * Draw the text label and any other effects attached to it.
 * Return: The length of the drawn label.
 */
void TextLabel2D::Draw() {
	if (this->font == NULL) {
		return;
	}

	// Draw drop shadow part
	if (this->dropShadow.isSet) {
		float dropAmt = static_cast<float>(this->GetHeight()) * this->dropShadow.amountPercentage;
		glColor3f(this->dropShadow.colour.R(), this->dropShadow.colour.G(), this->dropShadow.colour.B());
		this->font->OrthoPrint(this->topLeftCorner + Vector2D(dropAmt, -dropAmt), this->text);
	}

	// Draw coloured text part
	glColor3f(this->colour.R(), this->colour.G(), this->colour.B());
	this->lastRasterWidth = this->font->OrthoPrint(this->topLeftCorner, this->text);
}