#include "Forces.h"
#include "Chkdraft.h"
#include <string>

enum ID {
    EDIT_F1NAME = ID_FIRST,
    EDIT_F4NAME = (EDIT_F1NAME+3),
    LB_F1PLAYERS,
    LB_F4PLAYERS = (LB_F1PLAYERS+3),
    CHECK_F1ALLIED,
    CHECK_F2ALLIED,
    CHECK_F3ALLIED,
    CHECK_F4ALLIED,
    CHECK_F1RANDOM,
    CHECK_F2RANDOM,
    CHECK_F3RANDOM,
    CHECK_F4RANDOM,
    CHECK_F1VISION,
    CHECK_F2VISION,
    CHECK_F3VISION,
    CHECK_F4VISION,
    CHECK_F1AV,
    CHECK_F2AV,
    CHECK_F3AV,
    CHECK_F4AV
};

UINT WM_DRAGNOTIFY(WM_NULL);

ForcesWindow::ForcesWindow() : playerBeingDragged(255)
{
    for ( int i=0; i<4; i++ )
        possibleForceNameUpdate[i] = false;
}

bool ForcesWindow::CreateThis(HWND hParent, u32 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "Forces", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "Forces", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        HWND hForces = getHandle();
        textAboutForces.CreateThis(hForces, 5, 10, 587, 20,
            "Designate player forces, set force names, and force properties. It is recommended to separate computer and human players", 0);

        const char* forceGroups[] = { "Force 1", "Force 2", "Force 3", "Force 4" };
        for ( int y=0; y<2; y++ )
        {
            for ( int x=0; x<2; x++ )
            {
                int force = x+y*2;
                ChkdString forceName = "";
                bool allied = false, vision = false, random = false, av = false;
                if ( CM != nullptr )
                {
                    CM->getForceString(forceName, force);
                    CM->getForceInfo(force, allied, vision, random, av);
                }

                groupForce[force].CreateThis(hForces, 5+293*x, 50+239*y, 288, 234, forceGroups[force], 0);
                editForceName[force].CreateThis(hForces, 20+293*x, 70+239*y, 268, 20, false, EDIT_F1NAME+force);
                editForceName[force].SetText(forceName.c_str());
                dragForces[force].CreateThis(hForces, 20+293*x, 95+239*y, 268, 121, LB_F1PLAYERS+force);
                checkAllied[force].CreateThis(hForces, 15+293*x, 232+239*y, 100, 20, allied, "Allied", CHECK_F1ALLIED+force);
                checkSharedVision[force].CreateThis(hForces, 15+293*x, 252+239*y, 100, 20, vision, "Share Vision", CHECK_F1VISION+force);
                checkRandomizeStart[force].CreateThis(hForces, 125+293*x, 232+239*y, 150, 20, random, "Randomize Start Location", CHECK_F1RANDOM+force);
                checkAlliedVictory[force].CreateThis(hForces, 125+293*x, 252+239*y, 150, 20, av, "Enable Allied Victory", CHECK_F1AV+force);
            }
        }

        if ( WM_DRAGNOTIFY == WM_NULL )
            WM_DRAGNOTIFY = RegisterWindowMessage(DRAGLISTMSGSTRING);

        return true;
    }
    else
        return false;
}

bool ForcesWindow::DestroyThis()
{
    playerBeingDragged = 255;
    return true;
}

void ForcesWindow::RefreshWindow()
{
    HWND hWnd = getHandle();
    if ( CM != nullptr )
    {
        for ( int force=0; force<4; force++ )
        {
            ChkdString forceName;
            bool allied = false, vision = false, random = false, av = false;
            CM->getForceString(forceName, force);
            CM->getForceInfo(force, allied, vision, random, av);

            SetWindowText(GetDlgItem(hWnd, EDIT_F1NAME+force), forceName.c_str());
            if ( allied ) SendMessage(GetDlgItem(hWnd, CHECK_F1ALLIED+force), BM_SETCHECK, BST_CHECKED  , 0);
            else          SendMessage(GetDlgItem(hWnd, CHECK_F1ALLIED+force), BM_SETCHECK, BST_UNCHECKED, 0);
            if ( vision ) SendMessage(GetDlgItem(hWnd, CHECK_F1VISION+force), BM_SETCHECK, BST_CHECKED  , 0);
            else          SendMessage(GetDlgItem(hWnd, CHECK_F1VISION+force), BM_SETCHECK, BST_UNCHECKED, 0);
            if ( random ) SendMessage(GetDlgItem(hWnd, CHECK_F1RANDOM+force), BM_SETCHECK, BST_CHECKED  , 0);
            else          SendMessage(GetDlgItem(hWnd, CHECK_F1RANDOM+force), BM_SETCHECK, BST_UNCHECKED, 0);
            if ( av     ) SendMessage(GetDlgItem(hWnd, CHECK_F1AV    +force), BM_SETCHECK, BST_CHECKED  , 0);
            else          SendMessage(GetDlgItem(hWnd, CHECK_F1AV    +force), BM_SETCHECK, BST_UNCHECKED, 0);
        }

        for ( int i=0; i<4; i++ )
        {
            HWND hListBox = GetDlgItem(hWnd, LB_F1PLAYERS+i);
            if ( hListBox != NULL )
                while ( SendMessage(hListBox, LB_DELETESTRING, 0, 0) != LB_ERR );
        }

        for ( int player=0; player<8; player++ )
        {
            u8 force(0), color(0), race(0), displayOwner(CM->GetPlayerOwnerStringIndex(player));
            if ( CM->getPlayerForce(player, force) )
            {
                CM->getPlayerColor(player, color);
                CM->getPlayerRace(player, race);
                std::stringstream ssplayer;
                ssplayer << "Player " << player+1 << " - " << playerColors.at(color) << " - "
                         << playerRaces.at(race) << " (" << playerOwners.at(displayOwner) << ")";
                HWND hListBox = GetDlgItem(hWnd, LB_F1PLAYERS+force);
                if ( hListBox != NULL )
                    SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)ssplayer.str().c_str());
            }
        }
    }
}

LRESULT ForcesWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch ( HIWORD(wParam) )
    {
    case BN_CLICKED:
    {
        LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
        if ( CM != nullptr )
        {
            switch ( LOWORD(wParam) )
            {
            case CHECK_F1ALLIED: case CHECK_F2ALLIED: case CHECK_F3ALLIED: case CHECK_F4ALLIED:
            {
                int force = LOWORD(wParam) - CHECK_F1ALLIED;
                if ( state == BST_CHECKED )
                    CM->setForceAllied(force, true);
                else
                    CM->setForceAllied(force, false);

                CM->notifyChange(false);
            }
            break;

            case CHECK_F1VISION: case CHECK_F2VISION: case CHECK_F3VISION: case CHECK_F4VISION:
            {
                int force = LOWORD(wParam) - CHECK_F1VISION;
                if ( state == BST_CHECKED )
                    CM->setForceVision(force, true);
                else
                    CM->setForceVision(force, false);

                CM->notifyChange(false);
            }
            break;

            case CHECK_F1RANDOM: case CHECK_F2RANDOM: case CHECK_F3RANDOM: case CHECK_F4RANDOM:
            {
                int force = LOWORD(wParam) - CHECK_F1RANDOM;
                if ( state == BST_CHECKED )
                    CM->setForceRandom(force, true);
                else
                    CM->setForceRandom(force, false);

                CM->notifyChange(false);
            }
            break;

            case CHECK_F1AV: case CHECK_F2AV: case CHECK_F3AV: case CHECK_F4AV:
            {
                int force = LOWORD(wParam) - CHECK_F1AV;
                if ( state == BST_CHECKED )
                    CM->setForceAv(force, true);
                else
                    CM->setForceAv(force, false);

                CM->notifyChange(false);
            }
            break;
            }
        }
    }
    break;

    case EN_CHANGE:
        if ( LOWORD(wParam) >= EDIT_F1NAME && LOWORD(wParam) <= EDIT_F4NAME )
            possibleForceNameUpdate[LOWORD(wParam) - EDIT_F1NAME] = true;
        break;

    case EN_KILLFOCUS:
        if ( LOWORD(wParam) >= EDIT_F1NAME && LOWORD(wParam) <= EDIT_F4NAME )
            CheckReplaceForceName(LOWORD(wParam) - EDIT_F1NAME);
        break;
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT ForcesWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == TRUE )
            {
                RefreshWindow();
                chkd.mapSettingsWindow.SetWinText("Map Settings");
            }
            else
            {
                for ( int i=0; i<4; i++ )
                    CheckReplaceForceName(i);
            }
            break;

        default:
            {
                if ( WM_DRAGNOTIFY != WM_NULL && msg == WM_DRAGNOTIFY )
                {
                    DRAGLISTINFO* dragInfo = (DRAGLISTINFO*)lParam;
                    switch ( dragInfo->uNotification )
                    {
                        case DL_BEGINDRAG:
                            {
                                int index = LBItemFromPt(dragInfo->hWnd, dragInfo->ptCursor, FALSE);
                                LRESULT length = SendMessage(dragInfo->hWnd, LB_GETTEXTLEN, index, 0)+1;
                                char* str;
                                try { str = new char[length]; } catch ( std::bad_alloc ) { return FALSE; }
                                length = SendMessage(dragInfo->hWnd, LB_GETTEXT, index, (LPARAM)str);
                                if ( length != LB_ERR && length > 8 && str[7] >= '1' && str[7] <= '8' )
                                {
                                    playerBeingDragged = str[7]-'1';
                                    for ( int id=LB_F1PLAYERS; id<=LB_F4PLAYERS; id++ )
                                    {
                                        HWND hForceLb = GetDlgItem(hWnd, id);
                                        if ( hForceLb != dragInfo->hWnd && hForceLb != NULL )
                                            SendMessage(GetDlgItem(hWnd, id), LB_SETCURSEL, -1, 0);
                                    }
                                    return TRUE;
                                }
                                else
                                    return FALSE;
                            }
                            break;

                        case DL_CANCELDRAG:
                            playerBeingDragged = 255;
                            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
                            break;

                        case DL_DRAGGING:
                            {
                                HWND hUnder = WindowFromPoint(dragInfo->ptCursor);
                                if ( hUnder != NULL )
                                {
                                    LONG windowID = GetWindowLong(hUnder, GWL_ID);
                                    if ( windowID >= LB_F1PLAYERS && windowID <= LB_F4PLAYERS )
                                        return DL_MOVECURSOR;
                                }
                                return DL_STOPCURSOR;
                            }
                            break;

                        case DL_DROPPED:
                            {
                                HWND hUnder = WindowFromPoint(dragInfo->ptCursor);
                                if ( hUnder != NULL && playerBeingDragged < 8 )
                                {
                                    LONG windowID = GetWindowLong(hUnder, GWL_ID);
                                    if ( windowID >= LB_F1PLAYERS && windowID <= LB_F4PLAYERS )
                                    {
                                        int force = windowID-LB_F1PLAYERS;
                                        if ( CM != nullptr )
                                        {
                                            CM->setPlayerForce(playerBeingDragged, force);
                                            RefreshWindow();
                                            std::stringstream ssPlayer;
                                            ssPlayer << "Player " << playerBeingDragged+1;
                                            SendMessage(GetDlgItem(hWnd, LB_F1PLAYERS+force), LB_SELECTSTRING, -1, (LPARAM)ssPlayer.str().c_str());
                                            CM->notifyChange(false);
                                            chkd.trigEditorWindow.RefreshWindow();
                                            SetFocus(getHandle());
                                        }
                                    }
                                }
                                playerBeingDragged = 255;
                                return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
                            }
                            break;

                        default:
                            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
                            break;
                    }
                }
                else
                    return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            }
            break;
    }
    return 0;
}

void ForcesWindow::CheckReplaceForceName(int force)
{
    ChkdString newMapForce;
    if ( force < 4 && possibleForceNameUpdate[force] == true && editForceName[force].GetWinText(newMapForce) )
    {
        if ( CM->setForceName(force, newMapForce) )
        {
            CM->notifyChange(false);
            CM->refreshScenario();
        }
        possibleForceNameUpdate[force] = false;
    }
}
