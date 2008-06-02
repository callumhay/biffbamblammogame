#include "TextLabel.h"
#include "../Utils/Includes.h"

TextLabel2D::TextLabel2D(const TextureFontSet* font, const std::string& text) : 
colour(Colour(0, 0, 0)), topLeftCorner(Point2D(0, 0)), font(font), text(text){
	assert(font != NULL);
}

TextLabel2D::~TextLabel2D() {
}

/**
 * Draw the text label and any other effects attached to it.
 */
void TextLabel2D::Draw() {
		// Draw drop shadow part
		if (this->dropShadow.isSet) {
			float dropAmt = static_cast<float>(this->GetHeight()) * this->dropShadow.amountPercentage;
			glColor3f(this->dropShadow.colour.R(), this->dropShadow.colour.G(), this->dropShadow.colour.B());
			this->font->OrthoPrint(this->topLeftCorner + Vector2D(dropAmt, -dropAmt), this->text);
		}

		// Draw coloured text part
		glColor3f(this->colour.R(), this->colour.G(), this->colour.B());
		this->font->OrthoPrint(this->topLeftCorner, this->text);
	}