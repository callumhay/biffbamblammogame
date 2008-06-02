#ifndef __TEXTLABEL_H__
#define __TEXTLABEL_H__

#include <string>

#include "TextureFontSet.h"

#include "../Utils/Point.h"
#include "../Utils/Colour.h"

struct DropShadow {
	bool isSet;
	float amountPercentage;
	Colour colour;
	DropShadow(): amountPercentage(0.0f), colour(Colour(0,0,0)), isSet(false) {}
};

class TextLabel2D {

private:
	Colour colour;
	DropShadow dropShadow;
	Point2D topLeftCorner;
	const TextureFontSet* font;
	std::string text;

public:
	TextLabel2D(const TextureFontSet* font, const std::string& text);
	~TextLabel2D();

	// Set the text colour for this label
	void SetColour(const Colour& c) {
		this->colour = c;
	}
	// Set drop shadow for the label - the colour and amount it drops
	// from the text as a percentage of the text height
	void SetDropShadow(const Colour& c, float percentAmt) {
		this->dropShadow.isSet = true;
		this->dropShadow.colour = c;
		this->dropShadow.amountPercentage = percentAmt;
	}
	// Set the text for this label
	void SetText(const std::string& text) {
		this->text = text;
	}

	// Obtain the height of this label
	unsigned int GetHeight() const {
		return this->font->GetHeight();
	}
	// Set the top left corner location where this label will be drawn
	void SetTopLeftCorner(const Point2D& p) {
		this->topLeftCorner = p;
	}

	void Draw();
};

#endif