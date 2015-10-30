/**
 * TextLabel.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __TEXTLABEL_H__
#define __TEXTLABEL_H__

#include "BasicIncludes.h"
#include "TextureFontSet.h"

#include "Point.h"
#include "Colour.h"
#include "Camera.h"

struct DropShadow {
	bool isSet;
	float amountPercentage;
	float scale;
	Colour colour;
	DropShadow(): amountPercentage(0.0f), colour(Colour(0,0,0)), isSet(false), scale(1.0f) {}
    DropShadow(const DropShadow& copy) : isSet(copy.isSet), amountPercentage(copy.amountPercentage), scale(copy.scale), colour(copy.colour) {}
};

class TextLabel {
public:
    TextLabel();
    TextLabel(const TextLabel& copy);
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
        if (percentAmt == 0.0) {
            this->dropShadow.isSet = false;
            return;
        }
		this->dropShadow.isSet = true;
		this->dropShadow.colour = c;
		this->dropShadow.amountPercentage = percentAmt;
	}
    void SetDropShadowColour(const Colour& c) {
        this->dropShadow.colour = c;
    }
	void SetDropShadowAmount(float percentAmt) {
		this->dropShadow.amountPercentage = percentAmt;
        if (percentAmt == 0.0) {
            this->dropShadow.isSet = false;
        }
	}

	// Set the font of this label
	virtual void SetFont(const TextureFontSet* font) {
		this->font = font;
	}
	// Set the text colour for this label
	void SetColour(const Colour& c, float alpha = 1.0f) {
		this->colour = ColourRGBA(c, alpha);
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
    float GetAlpha() const {
        return this->colour[3];
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
    TextLabel2D(const TextLabel2D& copy);
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

    void SetCenter(const Point2D& p) {
        float width = this->GetLastRasterWidth();
        this->topLeftCorner = p - Vector2D(width/2.0, -this->GetHeight()/2.0);
    }

	// Obtain the height of this label
	float GetHeight() const {
		return this->scale * static_cast<float>(this->font->GetHeight());
	}
	float GetLastRasterWidth() const {
		return this->scale * this->lastRasterWidth;
	}

	std::string GetText() const {
		return this->text;
	}

	void Draw(bool depthTestOn = false, float depth = 0.0f);
    void Draw(float rotationInDegs);
    void Draw3D(const Camera& camera, float rotationInDegs = 0.0f, float z = 0.0f);

private:
	float lastRasterWidth;
	std::string text;
};


class TextLabel2DFixedWidth : public TextLabel {

public:
    enum Alignment { CenterAligned, LeftAligned, RightAligned };

    TextLabel2DFixedWidth(const TextureFontSet* font, float width, const std::string& text);
    TextLabel2DFixedWidth(const TextLabel2D& label);
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

    void SetScale(float scale, bool reformatText = true) {
	    assert(scale != 0.0f);
	    this->scale = scale;
        if (reformatText) {
            this->SetText(this->GetText());
        }
        else {
            this->currTextWidth *= this->scale;
        }
	}

    void SetScale(float scale, const std::string& text) {
        assert(scale != 0.0f);
        this->scale = scale;
        this->SetText(text);
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