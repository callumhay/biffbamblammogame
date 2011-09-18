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

const float DecoratorOverlayPane::X_BORDER = 10;
const float DecoratorOverlayPane::Y_BORDER = 10;
const float DecoratorOverlayPane::ITEM_SPACING = 5;

DecoratorOverlayPane::DecoratorOverlayPane(OverlayPaneEventHandler* handler,
                                           size_t width) :
eventHandler(handler), width(width), currYPos(0), selectedIdx(-1) {
    assert(handler != NULL);
}

DecoratorOverlayPane::~DecoratorOverlayPane() {
    this->ClearTextLabels();
    this->ClearImages();
    this->ClearSelectableOptions();
}

void DecoratorOverlayPane::AddText(const std::string& text) {

    TextLabel2DFixedWidth* newLabel = new TextLabel2DFixedWidth(
        GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium),
        this->width - 2*X_BORDER, text);
    
    newLabel->SetTopLeftCorner(X_BORDER, currYPos);
    newLabel->SetColour(Colour(1,1,1));
    newLabel->SetAlignment(TextLabel2DFixedWidth::CenterAligned);

    this->textLabels.push_back(newLabel);
    currYPos += newLabel->GetHeight() + ITEM_SPACING;
}

void DecoratorOverlayPane::AddImage(size_t width, size_t height, Texture* image) {
    Image* newImage = new Image((this->width - width) / 2.0f, currYPos, width, height, image);
    this->images.push_back(newImage);
    currYPos += height + ITEM_SPACING;
}

void DecoratorOverlayPane::SetSelectableOptions(const std::list<std::string>& options, int defaultIdx) {
    assert(defaultIdx >= 0 && defaultIdx < static_cast<int>(options.size()));
    assert(!options.empty());

    static const float OPTION_X_GAP = 20;

    float totalWidth = 0.0f;
    int count = 0;
    for (std::list<std::string>::const_iterator iter = options.begin();
         iter != options.end(); ++iter, ++count) {

        const std::string& currText = *iter;
        TextLabel2D* label = new TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
            GameFontAssetsManager::Medium), currText);
        label->SetColour(Colour(1,1,1));
        totalWidth += label->GetLastRasterWidth();
            
        this->selectableOptions.push_back(label);

        if (count != static_cast<int>(options.size())-1) {
            totalWidth += OPTION_X_GAP;
        }
    }

    // Set the positions of the labels
    float currXPos = this->width - totalWidth;
    for (std::vector<TextLabel2D*>::iterator iter = this->selectableOptions.begin();
         iter != this->selectableOptions.end(); ++iter) {

        TextLabel2D* label = *iter;
        label->SetTopLeftCorner(currXPos, this->currYPos);
        currXPos += label->GetLastRasterWidth() + OPTION_X_GAP;
    }
    this->currYPos += this->selectableOptions[0]->GetHeight() + ITEM_SPACING;

    this->selectedIdx = defaultIdx;
}

void DecoratorOverlayPane::Draw(size_t windowWidth, size_t windowHeight) {
    float totalHeight = this->currYPos + 2 * Y_BORDER;
    float halfTotalHeight = totalHeight / 2.0f;
    float halfTotalWidth  = this->width / 2.0f;
    
    float alpha = 1.0f;

    glPushMatrix();
    glTranslatef((windowWidth - this->width) / 2.0f,
                 windowHeight - (windowHeight - totalHeight) / 2.0f, 0.0f);

    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_POLYGON_BIT);
    
    // Draw the background of the pane
    glPolygonMode(GL_FRONT, GL_FILL);
    glColor4f(0.5f, 0.5f, 0.5f, alpha);
    GameMenu::DrawBackgroundQuad(halfTotalWidth, halfTotalHeight);
    
	// Draw the outline of the background
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(6.0f);
	glColor4f(0.0f, 0.0f, 0.0f, alpha);
	GameMenu::DrawBackgroundQuad(halfTotalWidth, halfTotalHeight);
	
	// Draw points at each corner to get rid of ugly hard edges
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT, GL_POINT);
	glPointSize(5.0f);
	GameMenu::DrawBackgroundQuad(halfTotalWidth, halfTotalHeight);

    glPopAttrib();

    // Draw each of the decorating components of the pane
    for (std::vector<TextLabel2DFixedWidth*>::iterator iter = this->textLabels.begin();
        iter != this->textLabels.end(); ++iter) {

        TextLabel2DFixedWidth* label = *iter;
        label->SetAlpha(alpha);
        label->Draw();
    }

    for (std::vector<Image*>::iterator iter = this->images.begin();
         iter != this->images.end(); ++iter) {

        Image* image = *iter;
        image->Draw(alpha);
    }
    
    for (std::vector<TextLabel2D*>::iterator iter = this->selectableOptions.begin();
         iter != this->selectableOptions.end(); ++iter) {

        TextLabel2D* label = *iter;
        label->SetAlpha(alpha);
        label->Draw();
    }

    glPopMatrix();
}

void DecoratorOverlayPane::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    switch (pressedButton) {

        case GameControl::UpButtonAction:
        case GameControl::DownButtonAction:
            break;

        case GameControl::LeftButtonAction:
            break;
        case GameControl::RightButtonAction:
            break;

        case GameControl::EnterButtonAction:
            break;
        case GameControl::EscapeButtonAction:
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