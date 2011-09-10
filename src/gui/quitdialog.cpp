/*
 *  The ManaPlus Client
 *  Copyright (C) 2004-2009  The Mana World Development Team
 *  Copyright (C) 2009-2010  The Mana Developers
 *  Copyright (C) 2011  The ManaPlus Developers
 *
 *  This file is part of The ManaPlus Client.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gui/quitdialog.h"

#include "client.h"

#include "gui/chatwindow.h"
#include "gui/npcdialog.h"
#include "gui/sdlinput.h"
#include "gui/viewport.h"

#include "gui/widgets/checkbox.h"
#include "gui/widgets/layout.h"
#include "gui/widgets/button.h"
#include "gui/widgets/radiobutton.h"

#include "net/charhandler.h"
#include "net/gamehandler.h"
#include "net/npchandler.h"
#include "net/net.h"

#include "utils/gettext.h"

#include "debug.h"

QuitDialog::QuitDialog(QuitDialog** pointerToMe):
    Window(_("Quit"), true, 0, "quit.xml"),
    mMyPointer(pointerToMe)
{
    mForceQuit = new RadioButton(_("Quit"), "quitdialog");
    mLogoutQuit = new RadioButton(_("Quit"), "quitdialog");
    mSwitchAccountServer = new RadioButton(_("Switch server"), "quitdialog");
    mSwitchCharacter = new RadioButton(_("Switch character"), "quitdialog");
    mOkButton = new Button(_("OK"), "ok", this);
    mCancelButton = new Button(_("Cancel"), "cancel", this);

    addKeyListener(this);

    ContainerPlacer placer = getPlacer(0, 0);

    const State state = Client::getState();

    // All states, when we're not logged in to someone.
    if (state == STATE_CHOOSE_SERVER ||
        state == STATE_CONNECT_SERVER ||
        state == STATE_LOGIN ||
        state == STATE_LOGIN_ATTEMPT ||
        state == STATE_UPDATE ||
        state == STATE_LOAD_DATA)
    {
        placeOption(placer, mForceQuit);
    }
    else
    {
        // Only added if we are connected to an accountserver or gameserver
        placeOption(placer, mLogoutQuit);
        placeOption(placer, mSwitchAccountServer);

        // Only added if we are connected to a gameserver
        if (state == STATE_GAME)
            placeOption(placer, mSwitchCharacter);
    }

    mOptions[0]->setSelected(true);

    placer = getPlacer(0, 1);

    placer(1, 0, mOkButton, 1);
    placer(2, 0, mCancelButton, 1);

    reflowLayout(200, 0);
    setLocationRelativeTo(getParent());
    setVisible(true);
    requestModalFocus();
    mOkButton->requestFocus();
}

QuitDialog::~QuitDialog()
{
    if (mMyPointer)
        *mMyPointer = 0;
    // Optional widgets, so delete them by hand.
    delete mForceQuit;
    mForceQuit = 0;
    delete mLogoutQuit;
    mLogoutQuit = 0;
    delete mSwitchAccountServer;
    mSwitchAccountServer = 0;
    delete mSwitchCharacter;
    mSwitchCharacter = 0;
}

void QuitDialog::placeOption(ContainerPlacer &placer, gcn::RadioButton *option)
{
    placer(0, static_cast<int>(mOptions.size()), option, 3);
    mOptions.push_back(option);
}

void QuitDialog::action(const gcn::ActionEvent &event)
{
    if (event.getId() == "ok")
    {
        if (viewport)
        {
            Map *map = viewport->getCurrentMap();
            if (map)
                map->saveExtraLayer();
        }

        if (mForceQuit->isSelected())
        {
            Client::setState(STATE_FORCE_QUIT);
        }
        else if (mLogoutQuit->isSelected())
        {
            Client::closeDialogs();
            Client::setState(STATE_EXIT);
        }
        else if (Net::getGameHandler()->isConnected()
                 && mSwitchAccountServer->isSelected())
        {
            Client::closeDialogs();
            Client::setState(STATE_SWITCH_SERVER);
        }
        else if (mSwitchCharacter->isSelected())
        {
            if (Client::getState() == STATE_GAME)
            {
                Net::getCharHandler()->switchCharacter();
                Client::closeDialogs();
            }
        }
    }
    scheduleDelete();
}

void QuitDialog::keyPressed(gcn::KeyEvent &keyEvent)
{
    const gcn::Key &key = keyEvent.getKey();
    int dir = 0;

    switch (key.getValue())
    {
        case Key::ENTER:
            action(gcn::ActionEvent(NULL, mOkButton->getActionEventId()));
            break;
        case Key::ESCAPE:
            action(gcn::ActionEvent(NULL, mCancelButton->getActionEventId()));
            break;
        case Key::UP:
            dir = -1;
            break;
        case Key::DOWN:
            dir = 1;
            break;
        default:
            break;
    }

    if (dir != 0)
    {
        std::vector<gcn::RadioButton*>::const_iterator it = mOptions.begin();

        for (; it < mOptions.end(); ++it)
        {
            if ((*it)->isSelected())
                break;
        }

        if (it == mOptions.end())
        {
            if (mOptions[0])
                mOptions[0]->setSelected(true);
            return;
        }
        else if (it == mOptions.begin() && dir < 0)
            it = mOptions.end();

        it += dir;

        if (it == mOptions.end())
            it = mOptions.begin();

        (*it)->setSelected(true);
    }
}
