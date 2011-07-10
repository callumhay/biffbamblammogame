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

class TextLabel {
public:
    TextLabel();
    TextLabel(const TextureFontSet* font);
    virtual ~TextLabel();

	virtual void SetScale(float scale) {
		assert(scale != 0.0f);
		this->scale = scale;
	}
	float GetScale() const {
		return this->scale;
	}

	// Set drop shadow for the label - the colour and amount it drops
	// from the text as a percentage of the text height
	void SetDropShadow(const Colour& c, float percentAmt) {
		this->dropShadow.isSet = true;
		this->dropShadow.colour = c;
		this->dropShadow.amountPercentage = percentAmt;
	}
	void SetDropShadowAmount(float percentAmt) {
		this->dropShadow.amountPercentage = percentAmt;
	}

	// Set the font of this label
	virtual void SetFont(const TextureFontSet* font) {
		this->font = font;
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

	// Set the top left corner location where this label will be drawn
	void SetTopLeftCorner(const Point2D& p) {
		this->topLeftCorner = p;
	}
    void SetTopLeftCorner(float x, float y) {
        this->topLeftCorner[0] = x;
        this->topLeftCorner[1] = y;
    }
	const Point2D& GetTopLeftCorner() const {
		return this->topLeftCorner;
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

protected:
    float scale;
	ColourRGBA colour;
	DropShadow dropShadow;
	Point2D topLeftCorner;
	const TextureFontSet* font;
};


class TextLabel2D : public TextLabel {

public:
	TextLabel2D();
	TextLabel2D(const TextureFontSet* font, const std::string& text);
	~TextLabel2D();

	// Set the text for this label
    void SetText(const char* text) {
        this->text = std::string(text);
        this->lastRasterWidth = this->font->GetWidth(this->text);
    }
	void SetText(const std::string& text) {
        this->SetText(text.c_str());
	}

	void SetFont(const TextureFontSet* font) {
		this->font = font;
		this->lastRasterWidth = this->font->GetWidth(this->text);
	}

	// Obtain the height of this label
	size_t GetHeight() const {
		return this->scale * this->font->GetHeight();
	}
	float GetLastRasterWidth() const {
		return this->scale * this->lastRasterWidth;
	}

	std::string GetText() const {
		return this->text;
	}

	void Draw(bool depthTestOn = false, float depth = 0.0);
    void Draw(float rotationInDegs);

private:
	float lastRasterWidth;
	std::string text;
};


class TextLabel2DFixedWidth : public TextLabel {

public:
    enum Alignment { LeftAligned, RightAligned };

    TextLabel2DFixedWidth(const TextureFontSet* font, float width, const std::string& text); 
    ~TextLabel2DFixedWidth();

	size_t GetHeight() const {
        if (this->textLines.empty()) { return 0; }
        return this->textLines.size() * (this->scale * this->font->GetHeight()) + 
            (this->textLines.size() - 1) * this->lineSpacing;
	}
	const std::vector<std::string>& GetTextLines() const {
		return this->textLines;
	}
    std::string GetText() const;

    float GetWidth() const {
        return this->currTextWidth;
    }
    float GetFixedWidth() const {
        return this->fixedWidth;
    }
    void SetFixedWidth(float width) {
        assert(width > 0);
        this->fixedWidth = width;
        this->SetText(this->GetText());
    }

    void SetScale(float scale) {
	    assert(scale != 0.0f);
	    this->scale = scale;
        this->SetText(this->GetText());
	}

	void SetFont(const TextureFontSet* font) {
		this->font = font;
        this->SetText(this->GetText());
	}

    void SetText(const std::string& text);
    void SetLineSpacing(float spacing) {
        this->lineSpacing = spacing;
    }

    void SetAlignment(const TextLabel2DFixedWidth::Alignment& alignment) {
        this->alignment = alignment;
    }

    void Draw();

private:
    Alignment alignment;
    float lineSpacing;
    float fixedWidth;
    float currTextWidth;
    std::vector<std::string> textLines;

    void DrawTextLines(float xOffset, float yOffset);

    DISALLOW_COPY_AND_ASSIGN(TextLabel2DFixedWidth);
};

#endif