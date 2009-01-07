#ifndef __TEXTLABEL_H__
#define __TEXTLABEL_H__

#include "BasicIncludes.h"
#include "TextureFontSet.h"

#include "Point.h"
#include "Colour.h"

struct DropShadow {
	bool isSet;
	float amountPercentage;
	float scale;
	Colour colour;
	DropShadow(): amountPercentage(0.0f), colour(Colour(0,0,0)), isSet(false), scale(1.0f) {}
};

class TextLabel2D {

private:
	float lastRasterWidth;
	Colour colour;
	DropShadow dropShadow;
	Point2D topLeftCorner;
	const TextureFontSet* font;
	std::string text;

public:
	TextLabel2D();
	TextLabel2D(const TextureFontSet* font, const std::string& text);
	~TextLabel2D();

	// Set the font of this label
	void SetFont(const TextureFontSet* font) {
		this->font = font;
	}
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
	// Set the top left corner location where this label will be drawn
	void SetTopLeftCorner(const Point2D& p) {
		this->topLeftCorner = p;
	}

	// Obtain the height of this label
	unsigned int GetHeight() const {
		return this->font->GetHeight();
	}
	float GetLastRasterWidth() const {
		return this->lastRasterWidth;
	}

	void Draw();
};

#endif