/**
 * TextLabel.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
	float scale;
	float lastRasterWidth;
	ColourRGBA colour;
	DropShadow dropShadow;
	Point2D topLeftCorner;
	const TextureFontSet* font;
	std::string text;

public:
	TextLabel2D();
	TextLabel2D(const TextureFontSet* font, const std::string& text);
	~TextLabel2D();

	void SetScale(float scale) {
		assert(scale != 0.0f);
		this->scale = scale;
	}
	float GetScale() const {
		return this->scale;
	}

	// Set the font of this label
	void SetFont(const TextureFontSet* font) {
		this->font = font;
		this->lastRasterWidth = this->font->GetWidth(this->text);
	}
	// Set the text colour for this label
	void SetColour(const Colour& c) {
		this->colour = ColourRGBA(c, 1.0f);
	}
	void SetColour(const ColourRGBA& c) {
		this->colour = c;
	}
	void SetAlpha(float alpha) {
		this->colour[3] = alpha;
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
		this->lastRasterWidth = this->font->GetWidth(this->text);
	}
	// Set the top left corner location where this label will be drawn
	void SetTopLeftCorner(const Point2D& p) {
		this->topLeftCorner = p;
	}
	Point2D GetTopLeftCorner() const {
		return this->topLeftCorner;
	}

	// Obtain the height of this label
	unsigned int GetHeight() const {
		return this->scale * this->font->GetHeight();
	}
	float GetLastRasterWidth() const {
		return this->scale * this->lastRasterWidth;
	}

	const TextureFontSet* GetFont() const {
		return this->font;
	}
	const DropShadow& GetDropShadow() const {
		return this->dropShadow;
	}
	const ColourRGBA& GetColour() const {
		return this->colour;
	}
	std::string GetText() const {
		return this->text;
	}

	void Draw(bool depthTestOn = false, float depth = 0.0);
};

#endif