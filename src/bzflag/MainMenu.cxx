/* bzflag
 * Copyright (c) 1993-2025 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "MainMenu.h"

#include "FontManager.h"
#include "HelpMenu.h"
#include "HUDDialogStack.h"
#include "LocalPlayer.h"
#include "JoinMenu.h"
#include "OptionsMenu.h"
#include "QuitMenu.h"
#include "playing.h"
#include "HUDui.h"

MainMenu::MainMenu() : HUDDialog(),
    join(), options(), help(), leave(), quit(),
    joinMenu(NULL), optionsMenu(NULL), quitMenu(NULL)
{
}

void MainMenu::createControls()
{
    std::vector<HUDuiControl*>& listHUD = getControls();
    HUDuiControl* label;

    for (unsigned int i = 0; i < listHUD.size(); i++)
        delete listHUD[i];
    listHUD.erase(listHUD.begin(), listHUD.end());

    // Do not use the inherited BZFlag title texture.  Keeping the product title
    // as real text also guarantees it uses the same Unicode TTF path as the UI.
    label = createLabel("IrFlags");
    listHUD.push_back(label);

    label = createLabel("DeathAmir And IrAutoX");
    listHUD.push_back(label);

    join = createLabel("Join Game");
    listHUD.push_back(join);

    options = createLabel("Options");
    listHUD.push_back(options);

    help = createLabel("Help");
    listHUD.push_back(help);

    LocalPlayer* myTank = LocalPlayer::getMyTank();
    if (myTank != NULL)
    {
        leave = createLabel("Leave Game");
        listHUD.push_back(leave);
    }
    else
        leave = NULL;

    quit = createLabel("Quit");
    listHUD.push_back(quit);

    resize(HUDDialog::getWidth(), HUDDialog::getHeight());
    initNavigation(listHUD, 2, listHUD.size() - 1);
    listHUD[2]->setFocus();
}

HUDuiControl* MainMenu::createLabel(const char* string)
{
    HUDuiLabel* control = new HUDuiLabel;
    control->setFontFace(getFontFace());
    control->setString(string);
    return control;
}

MainMenu::~MainMenu()
{
    std::vector<HUDuiControl *>& listHUD = getControls();
    for (unsigned int i = 0; i < listHUD.size(); i++)
        delete listHUD[i];
    listHUD.erase(listHUD.begin(), listHUD.end());
    delete joinMenu;
    delete optionsMenu;
    delete quitMenu;
    HelpMenu::done();
}

int MainMenu::getFontFace()
{
    return FontManager::instance().getFaceID(BZDB.get("sansSerifFont"));
}

HUDuiDefaultKey* MainMenu::getDefaultKey()
{
    return MenuDefaultKey::getInstance();
}

void MainMenu::execute()
{
    HUDuiControl* _focus = HUDui::getFocus();
    if (_focus == join)
    {
        if (!joinMenu) joinMenu = new JoinMenu;
        HUDDialogStack::get()->push(joinMenu);
    }
    else if (_focus == options)
    {
        if (!optionsMenu) optionsMenu = new OptionsMenu;
        HUDDialogStack::get()->push(optionsMenu);
    }
    else if (_focus == help)
        HUDDialogStack::get()->push(HelpMenu::getHelpMenu());
    else if (_focus == leave)
    {
        leaveGame();
        createControls();
    }
    else if (_focus == quit)
    {
        if (!quitMenu) quitMenu = new QuitMenu;
        HUDDialogStack::get()->push(quitMenu);
    }
}

void MainMenu::resize(int _width, int _height)
{
    HUDDialog::resize(_width, _height);

    const float titleFontSize = (float)_height / 8.0f;
    const float tinyFontSize = (float)_height / 54.0f;
    const float fontSize = (float)_height / 22.0f;
    FontManager &fm = FontManager::instance();
    int fontFace = getFontFace();

    std::vector<HUDuiControl*>& listHUD = getControls();
    HUDuiLabel* title = (HUDuiLabel*)listHUD[0];
    title->setFontSize(titleFontSize);
    const float titleWidth = fm.getStrLength(fontFace, titleFontSize, title->getString());
    float x = 0.5f * ((float)_width - titleWidth);
    float y = (float)_height - titleFontSize * 1.5f;
    title->setPosition(x, y);

    HUDuiLabel* creator = (HUDuiLabel*)listHUD[1];
    creator->setFontSize(tinyFontSize);
    const float creatorWidth = fm.getStrLength(fontFace, tinyFontSize, creator->getString());
    y -= 1.25f * fm.getStrHeight(fontFace, tinyFontSize, creator->getString());
    creator->setPosition(0.5f * ((float)_width - creatorWidth), y);
    y -= 2.0f * fm.getStrHeight(fontFace, fontSize, creator->getString());

    const float firstWidth = fm.getStrLength(fontFace, fontSize,
                             ((HUDuiLabel*)listHUD[2])->getString());
    x = 0.5f * ((float)_width - firstWidth);
    const int count = listHUD.size();
    for (int i = 2; i < count; i++)
    {
        HUDuiLabel* item = (HUDuiLabel*)listHUD[i];
        item->setFontSize(fontSize);
        item->setPosition(x, y);
        y -= 1.3f * fm.getStrHeight(fontFace, fontSize, item->getString());
    }
}

// Local Variables: ***
// mode: C++ ***
// tab-width: 4 ***
// c-basic-offset: 4 ***
// indent-tabs-mode: nil ***
// End: ***
// ex: shiftwidth=4 tabstop=4
