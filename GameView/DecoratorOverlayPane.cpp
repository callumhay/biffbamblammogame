/**
 * DecoratorOverlayPane.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "DecoratorOverlayPane.h"
#include "GameFontAssetsManager.h"
#include "GameMenu.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/GeometryMaker.h"

const float DecoratorOverlayPane::X_BORDER     = 12;
const float DecoratorOverlayPane::Y_BORDER     = 12;
const float DecoratorOverlayPane::ITEM_SPACING = 20;
const float DecoratorOverlayPane::X_OPTION_GAP = 50;


DecoratorOverlayPane::DecoratorOverlayPane(OverlayPaneEventHandler* handler,
                                           size_t width, const Colour& bgColour) :
eventHandler(handler), width(width), currYPos(-Y_BORDER + ITEM_SPACING), selectedIdx(-1), optionActive(false),
OPTION_IDLE_COLOUR(0.75f, 0.75f, 0.75f),
OPTION_SEL_COLOUR(1.0f, 1.0f, 1.0f),
OPTION_ACTIVE_COLOUR(1, 0.8f, 0),
bgColour(bgColour) {

    assert(handler != NULL);

    this->fgFadeAnimation.ClearLerp();
    this->fgFadeAnimation.SetInterpolantValue(0.0f);
    this->fgFadeAnimation.SetRepeat(false);
    this->bgFadeAnimation.ClearLerp();
    this->bgFadeAnimation.SetInterpolantValue(0.0f);
    this->bgFadeAnimation.SetRepeat(false);
    this->scaleAnimation.ClearLerp();
    this->scaleAnimation.SetInterpolantValue(0.001f);
    this->scaleAnimation.SetRepeat(false);

    std::vector<double> timeVals;
    timeVals.reserve(3);
    timeVals.push_back(0);
    timeVals.push_back(0.5);
    timeVals.push_back(1.0);
    std::vector<float> pulseVals;
    pulseVals.reserve(3);
    pulseVals.push_back(0.85f);
    pulseVals.push_back(1.15f);
    pulseVals.push_back(0.85f);

    this->arrowPulseAnimation.SetLerp(timeVals, pulseVals);
    this->arrowPulseAnimation.SetRepeat(true);

    timeVals[1] = 0.4;
    timeVals[2] = 0.8;

    std::vector<float> upAndDownVals;
    upAndDownVals.reserve(3);
    upAndDownVals.push_back(-5);
    upAndDownVals.push_back(5);
    upAndDownVals.push_back(-5);

    this->upAndDownAnimation.SetLerp(timeVals, upAndDownVals);
    this->upAndDownAnimation.SetRepeat(true);
}

DecoratorOverlayPane::~DecoratorOverlayPane() {
    this->ClearTextLabels();
    this->ClearImages();
    this->ClearSelectableOptions();
}

void DecoratorOverlayPane::AddText(const std::string& text, const Colour& colour, float scale) {
    this->currYPos -= ITEM_SPACING;

    TextLabel2DFixedWidth* newLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
        this->width - 2*X_BORDER, text);
    
    newLabel->SetTopLeftCorner(0, this->currYPos);
    newLabel->SetColour(colour);
    newLabel->SetDropShadow(Colour(0,0,0), 0.10f);
    newLabel->SetScale(scale);
    newLabel->SetAlignment(TextLabel2DFixedWidth::CenterAligned);

    this->textLabels.push_back(newLabel);
    this->currYPos -= newLabel->GetHeight();
}

void DecoratorOverlayPane::AddImage(size_t width, size_t height, Texture* image) {
    this->currYPos -= ITEM_SPACING;
    Image* newImage = new Image((this->width - width) / 2.0f, this->currYPos, width, height, image);
    this->images.push_back(newImage);
    this->currYPos -= height;
}

void DecoratorOverlayPane::SetSelectableOptions(const std::vector<std::string>& options, int defaultIdx) {
    assert(defaultIdx >= 0 && defaultIdx < static_cast<int>(options.size()));
    assert(!options.empty());

    this->currYPos -= 2*ITEM_SPACING;

    float totalWidth = 0.0f;
    int count = 0;
    for (std::vector<std::string>::const_iterator iter = options.begin();
         iter != options.end(); ++iter, ++count) {

        const std::string& currText = *iter;
        TextLabel2D* label = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
            GameFontAssetsManager::Medium), currText);
        label->SetColour(this->OPTION_IDLE_COLOUR);
        label->SetDropShadow(Colour(0,0,0), 0.10f);
        totalWidth += label->GetLastRasterWidth();
            
        this->selectableOptions.push_back(label);

        TextLabel2D* selLabel = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose,
            GameFontAssetsManager::Big), currText);
        selLabel->SetScale(0.9f);
        selLabel->SetColour(this->OPTION_SEL_COLOUR);
        selLabel->SetDropShadow(Colour(0,0,0), 0.125f);

        this->selectedSelectableOptions.push_back(selLabel);

        if (count != static_cast<int>(options.size())-1) {
            totalWidth += X_OPTION_GAP;
        }
    }

    // Set the positions of the labels
    float currXPos = (this->width - totalWidth) / 2.0f;
    std::vector<TextLabel2D*>::iterator iterSel = this->selectedSelectableOptions.begin();
    std::vector<TextLabel2D*>::iterator iter = this->selectableOptions.begin();

    for (; iter != this->selectableOptions.end() && iterSel != this->selectedSelectableOptions.end(); ++iter, ++iterSel) {

        TextLabel2D* label = *iter;
        TextLabel2D* selLabel = *iterSel;
        label->SetTopLeftCorner(currXPos, this->currYPos);
        selLabel->SetTopLeftCorner(currXPos - (selLabel->GetLastRasterWidth() - label->GetLastRasterWidth()) / 2.0f,
            this->currYPos + (static_cast<float>(selLabel->GetHeight()) - static_cast<float>(label->GetHeight())) / 2.0f);

        currXPos += label->GetLastRasterWidth() + X_OPTION_GAP;
    }

    this->currYPos -= this->selectableOptions[0]->GetHeight();
    this->selectedIdx = defaultIdx;
}

void DecoratorOverlayPane::Show(double timeInSecs) {
    this->fgFadeAnimation.SetLerp(timeInSecs*0.6f, timeInSecs, 0.0f, 1.0f);
    this->bgFadeAnimation.SetLerp(0.0, timeInSecs, 0.0f, 1.0f);
    this->scaleAnimation.SetLerp(0.0, timeInSecs*0.6f, 0.001f, 1.0f); 
}

void DecoratorOverlayPane::Hide(double timeInSecs) {
    this->fgFadeAnimation.SetLerp(timeInSecs*0.5f, 0.0f);
    this->bgFadeAnimation.SetLerp(timeInSecs*0.5f, timeInSecs, this->bgFadeAnimation.GetInterpolantValue(), 0.0f);
    this->scaleAnimation.SetLerp(timeInSecs*0.5f, timeInSecs, this->scaleAnimation.GetInterpolantValue(), 0.001f); 
}

void DecoratorOverlayPane::Draw(double dT, size_t windowWidth, size_t windowHeight) {

    float totalHeight       = abs(this->currYPos) + 2 * Y_BORDER;
    float halfTotalHeight   = totalHeight / 2.0f;
    float halfTotalWidth    = this->width / 2.0f;
    float halfWindowWidth   = windowWidth / 2.0f;
    float halfWindowHeight  = windowHeight / 2.0f;
    float labelXTranslation = (windowWidth - this->width) / 2.0f;
    float labelYTranslation = halfTotalHeight + halfWindowHeight;

    float fgAlpha = this->fgFadeAnimation.GetInterpolantValue();
    float bgAlpha = this->bgFadeAnimation.GetInterpolantValue();
    float scale = this->scaleAnimation.GetInterpolantValue();

    if (fgAlpha > 0.0f || bgAlpha > 0.0f) {
        Camera::PushWindowCoords();
	    glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
        glLoadIdentity();
        glTranslatef(halfWindowWidth, halfWindowHeight, 0.0f);

        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_POLYGON_BIT);
        
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    	
        glPushMatrix();
        glScalef(scale, scale, 1.0f);

        // Draw the background of the pane
        glPolygonMode(GL_FRONT, GL_FILL);
        glColor4f(this->bgColour.R(), this->bgColour.G(), this->bgColour.B(), bgAlpha);
        GameMenu::DrawBackgroundQuad(halfTotalWidth, halfTotalHeight);
        
	    // Draw the outline of the background
	    glEnable(GL_LINE_SMOOTH);
	    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	    glPolygonMode(GL_FRONT, GL_LINE);
	    glLineWidth(6.0f);
	    glColor4f(0.0f, 0.0f, 0.0f, bgAlpha);
	    GameMenu::DrawBackgroundQuad(halfTotalWidth, halfTotalHeight);
    	
	    // Draw points at each corner to get rid of ugly hard edges
	    glEnable(GL_POINT_SMOOTH);
	    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	    glPolygonMode(GL_FRONT, GL_POINT);
	    glPointSize(5.0f);
	    GameMenu::DrawBackgroundQuad(halfTotalWidth, halfTotalHeight);

        glPopMatrix();

        glPopAttrib();

        // Draw each of the decorating components of the pane
        Point2D tempPt;
        for (std::vector<TextLabel2DFixedWidth*>::iterator iter = this->textLabels.begin();
            iter != this->textLabels.end(); ++iter) {

            TextLabel2DFixedWidth* label = *iter;
            tempPt = label->GetTopLeftCorner();
            label->SetTopLeftCorner(tempPt[0] + labelXTranslation + (this->width - label->GetWidth()) / 2.0f, 
                tempPt[1] + labelYTranslation);
            label->SetAlpha(fgAlpha);
            label->Draw();
            label->SetTopLeftCorner(tempPt);
        }

        for (std::vector<Image*>::iterator iter = this->images.begin();
             iter != this->images.end(); ++iter) {

            Image* image = *iter;
            image->Draw(fgAlpha);
        }
        
        this->arrowPulseAnimation.Tick(dT);
        this->upAndDownAnimation.Tick(dT);

        assert(this->selectableOptions.size() == this->selectedSelectableOptions.size());
        for (size_t i = 0; i < this->selectableOptions.size(); i++) {
            
            TextLabel2D* label = NULL;
            if (static_cast<int>(i) == this->selectedIdx) {
                label = this->selectedSelectableOptions[i];
            }
            else {
                label = this->selectableOptions[i];
            }
            
            tempPt = label->GetTopLeftCorner();

            if (static_cast<int>(i) == this->selectedIdx) {
                label->SetTopLeftCorner(tempPt[0] + labelXTranslation, tempPt[1] + labelYTranslation + this->upAndDownAnimation.GetInterpolantValue());
            }
            else {
                label->SetTopLeftCorner(tempPt[0] + labelXTranslation, tempPt[1] + labelYTranslation);
            }

            label->SetAlpha(fgAlpha);
            label->Draw();
            
            // Draw arrows on either side of the selection...
            if (static_cast<int>(i) == this->selectedIdx) {
                this->DrawArrow(label->GetTopLeftCorner() - Vector2D(X_OPTION_GAP*0.6f, this->upAndDownAnimation.GetInterpolantValue()), fgAlpha, label->GetHeight(), true);
                this->DrawArrow(label->GetTopLeftCorner() + Vector2D(label->GetLastRasterWidth() + X_OPTION_GAP*0.6f - label->GetHeight()*0.7f, 
                    - this->upAndDownAnimation.GetInterpolantValue()), fgAlpha, label->GetHeight(), false);
            }

            label->SetTopLeftCorner(tempPt);
        }

        glPopMatrix();
        Camera::PopWindowCoords();
    }

    this->fgFadeAnimation.Tick(dT);
    this->bgFadeAnimation.Tick(dT);
    this->scaleAnimation.Tick(dT);
}

void DecoratorOverlayPane::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    assert(!this->selectableOptions.empty());

    // If the user has already selected an option (hit 'enter'), ignore
    // all further input
    if (this->optionActive) {
        return;
    }

    switch (pressedButton) {

        case GameControl::UpButtonAction:
        case GameControl::DownButtonAction:
            break;

        case GameControl::LeftButtonAction:
            this->selectedIdx--;
            if (this->selectedIdx < 0) {
                this->selectedIdx = 0;
            }
            break;
        case GameControl::RightButtonAction:
            this->selectedIdx++;
            if (this->selectedIdx >= static_cast<int>(this->selectableOptions.size())) {
                this->selectedIdx = static_cast<int>(this->selectableOptions.size()) - 1;
            }
            break;

        case GameControl::EnterButtonAction:
            this->optionActive = true;
            this->selectedSelectableOptions[this->selectedIdx]->SetColour(this->OPTION_ACTIVE_COLOUR);
            this->eventHandler->OptionSelected(this->selectedSelectableOptions[this->selectedIdx]->GetText());
            break;

        case GameControl::EscapeButtonAction:
            // Ignore
            break;

        default:
            break;
    }
}

void DecoratorOverlayPane::ClearTextLabels() {
    for (std::vector<TextLabel2DFixedWidth*>::iterator iter = this->textLabels.begin();
        iter != this->textLabels.end(); ++iter) {
        TextLabel2DFixedWidth* label = *iter;
        delete label;
        label = NULL;
    }
    this->textLabels.clear();
}

void DecoratorOverlayPane::ClearImages() {
    for (std::vector<Image*>::iterator iter = this->images.begin();
         iter != this->images.end(); ++iter) {
        Image* img = *iter;
        delete img;
        img = NULL;
    }
    this->images.clear();
}

void DecoratorOverlayPane::ClearSelectableOptions() {
    for (std::vector<TextLabel2D*>::iterator iter = this->selectableOptions.begin();
         iter != this->selectableOptions.end(); ++iter) {
        TextLabel2D* label = *iter;
        delete label;
        label = NULL;
    }
    this->selectableOptions.clear();

    for (std::vector<TextLabel2D*>::iterator iter = this->selectedSelectableOptions.begin();
         iter != this->selectedSelectableOptions.end(); ++iter) {
        TextLabel2D* label = *iter;
        delete label;
        label = NULL;
    }
    this->selectedSelectableOptions.clear();
}

void DecoratorOverlayPane::DrawArrow(const Point2D& topLeftCorner, float alpha, 
                                     float arrowHeight, bool isLeftPointing) {

	const float HALF_ARROW_HEIGHT = arrowHeight / 2.0f;
	const float HALF_ARROW_WIDTH  = arrowHeight*0.7f  / 2.0f;

	// Arrow vertices, centered on the origin
	const Point2D APEX_POINT	= (isLeftPointing ? Point2D(-HALF_ARROW_WIDTH, 0.0f) : Point2D(HALF_ARROW_WIDTH, 0.0f));
	const Point2D TOP_POINT		= (isLeftPointing ? Point2D(HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT) : Point2D(-HALF_ARROW_WIDTH, HALF_ARROW_HEIGHT));
	const Point2D BOTTOM_POINT	= (isLeftPointing ? Point2D(HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT) : Point2D(-HALF_ARROW_WIDTH, -HALF_ARROW_HEIGHT));

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_POLYGON_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
    glLoadIdentity();
	glTranslatef(topLeftCorner[0] + HALF_ARROW_WIDTH, topLeftCorner[1] - HALF_ARROW_HEIGHT, 0.0f);

    float pulse = this->arrowPulseAnimation.GetInterpolantValue();
	glScalef(pulse, pulse, 1.0f);

	// Fill in the arrows
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor4f(1.0f, 0.6f, 0.0f, alpha);
	
	if (isLeftPointing) {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
		glEnd();
	}
	
	// Draw the outlines of the arrows
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(2.0f);
	glColor4f(0.0f, 0.0f, 0.0f, alpha);

	if (isLeftPointing) {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);
			glVertex2f(APEX_POINT[0], APEX_POINT[1]);
			glVertex2f(TOP_POINT[0], TOP_POINT[1]);
			glVertex2f(BOTTOM_POINT[0], BOTTOM_POINT[1]);
		glEnd();
	}

	glPopMatrix();
    glPopAttrib();
}

DecoratorOverlayPane::Image::Image(size_t topLeftX, size_t topLeftY, 
                                   size_t width, size_t height, 
                                   Texture* texture) :

topLeftX(topLeftX), topLeftY(topLeftY), width(width), height(height), texture(texture) {
    assert(texture != NULL);
    assert(width > 0);
    assert(height > 0);
}

void DecoratorOverlayPane::Image::Draw(float alpha) {
    glPushMatrix();
    glTranslatef(this->topLeftX + this->width/2, this->topLeftY - this->height/2, 0.0f);
    glScalef(this->width, this->height, 1);
    
    glColor4f(1,1,1,alpha);
    this->texture->BindTexture();
    GeometryMaker::GetInstance()->DrawQuad();
    this->texture->UnbindTexture();

    glPopMatrix();
}

