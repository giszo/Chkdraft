#include "MapSettings.h"
#include "GuiAccel.h"
#include "Maps.h"
#include <strsafe.h>
extern MAPS maps;
using namespace std;

u32 currTab(0);
UINT WM_DRAGNOTIFY(WM_NULL);
u8 playerBeingDragged(255);
u32 currSelString(0);

WNDPROC wpStringEdit; // Used to redefine default edit window input handling
LRESULT CALLBACK StringEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_KEYDOWN:
			if ( GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A' ) // Select all
				SendMessage(hWnd, EM_SETSEL, 0, -1);
			else if ( wParam == VK_RETURN )
				SendMessage(hWnd, EM_REPLACESEL, TRUE, (LPARAM)"\r\n");
			break;

		case WM_CHAR:
			if ( (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 1) || wParam == VK_RETURN ) // Prevent ctrl + key from causing beeps
				return 0;
			break;
	}
	return CallWindowProc(wpStringEdit, hWnd, msg, wParam, lParam);
}

BOOL CALLBACK MapSettingsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			{
				const char* tabs[7] = {
					"Map Properties", "Forces", "Unit Settings", "Upgrade Settings",
					"Tech Settings", "String Editor", "Wav Editor"
				};

				HWND hTabs = GetDlgItem(hWnd, IDC_MAPSETTINGSTABS);
				LPARAM icon = (LPARAM)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM_ICON), IMAGE_ICON, 16, 16, 0);
				SendMessage(hTabs, WM_SETFONT, (WPARAM)defaultFont, FALSE);
				SendMessage(hWnd, WM_SETICON, ICON_SMALL, icon);

				TCITEM item = { };
				item.mask = TCIF_TEXT;
				item.iImage = -1;
				
				for ( int i=0; i<7; i++ )
				{
					item.pszText = (LPSTR)tabs[i];
					item.cchTextMax = strlen(tabs[i]);
					TabCtrl_InsertItem(hTabs, i, &item);
				}

				WNDCLASSEX wcMapProperties = { },
						   wcForces = { },
						   wcUnitSettings = { },
						   wcUpgradeSettings = { },
						   wcTechSettings = { },
						   wcStringPreview = { },
						   wcStringGuide = { },
						   wcStringSettings = { },
						   wcWavSettings = { };

				wcMapProperties.cbSize		  = sizeof(WNDCLASSEX);
				wcMapProperties.lpfnWndProc   = MapPropertiesProc;
			    wcMapProperties.hInstance     = GetModuleHandle(NULL);
				wcMapProperties.lpszClassName = "MapProperties";

				wcForces = wcUnitSettings = wcUpgradeSettings = wcTechSettings = wcStringPreview =
				wcStringGuide = wcStringSettings = wcWavSettings = wcMapProperties;

				wcForces.lpfnWndProc = ForcesProc;						wcForces.lpszClassName = "Forces";
				wcUnitSettings.lpfnWndProc = UnitSettingsProc;			wcUnitSettings.lpszClassName = "UnitSettings";
				wcUpgradeSettings.lpfnWndProc = UpgradeSettingsProc;	wcUpgradeSettings.lpszClassName = "UpgradeSettings";
				wcTechSettings.lpfnWndProc = TechSettingsProc;			wcTechSettings.lpszClassName = "TechSettings";
				wcStringPreview.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
				wcStringPreview.lpfnWndProc = StringPreviewProc;		wcStringPreview.lpszClassName = "StringPreview";
				wcStringGuide.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
				wcStringGuide.lpfnWndProc = StringGuideProc;			wcStringGuide.lpszClassName = "StringGuide";
				wcStringSettings.lpfnWndProc = StringEditorProc;		wcStringSettings.lpszClassName = "StringEditor";
				wcWavSettings.lpfnWndProc = WavEditorProc;				wcWavSettings.lpszClassName = "WavEditor";

				RegisterClassEx(&wcMapProperties);
				RegisterClassEx(&wcForces);
				RegisterClassEx(&wcUnitSettings);
				RegisterClassEx(&wcUpgradeSettings);
				RegisterClassEx(&wcTechSettings);
				RegisterClassEx(&wcStringPreview);
				RegisterClassEx(&wcStringGuide);
				RegisterClassEx(&wcStringSettings);
				RegisterClassEx(&wcWavSettings);
				
				CreateMapProperties(hWnd);
				CreateForces(hWnd);
				CreateUnitSettings(hWnd);
				CreateUpgradeSettings(hWnd);
				CreateTechSettings(hWnd);
				CreateStringEditor(hWnd);
				CreateWavEditor(hWnd);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
			}
			break;

		case WM_MOUSEWHEEL:
			if ( currTab == ID_TAB_STRINGEDITOR )
				SendMessage(GetDlgItem(hWnd, ID_STRINGEDITOR), WM_MOUSEWHEEL, wParam, lParam);
			break;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;
				
				default:
					return DefWindowProc(hWnd, msg, wParam, lParam);
					break;
			}
			break;

		case WM_NOTIFY:
			{
				NMHDR* notification = (NMHDR*)lParam;
				switch ( notification->code )
				{
					case TCN_SELCHANGE:
						{
							HWND hTabs = GetDlgItem(hWnd, IDC_MAPSETTINGSTABS);
							u32 selectedTab = TabCtrl_GetCurSel(hTabs);
							switch ( selectedTab )
							{
								case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hWnd, ID_MAPPROPERTIES), SW_SHOW); break;
								case ID_TAB_FORCES: ShowWindow(GetDlgItem(hWnd, ID_FORCES), SW_SHOW); break;
								case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UNITSETTINGS), SW_SHOW); break;
								case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UPGRADESETTINGS), SW_SHOW); break;
								case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_TECHSETTINGS), SW_SHOW); break;
								case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hWnd, ID_STRINGEDITOR), SW_SHOW); break;
								case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hWnd, ID_WAVEDITOR), SW_SHOW); break;
							}
						}
						break;

					case TCN_SELCHANGING:
						{
							HWND hTabs = GetDlgItem(hWnd, IDC_MAPSETTINGSTABS);
							u32 selectedTab = TabCtrl_GetCurSel(hTabs);
							switch ( selectedTab )
							{
								case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hWnd, ID_MAPPROPERTIES), SW_HIDE); break;
								case ID_TAB_FORCES: ShowWindow(GetDlgItem(hWnd, ID_FORCES), SW_HIDE); break;
								case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UNITSETTINGS), SW_HIDE); break;
								case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UPGRADESETTINGS), SW_HIDE); break;
								case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_TECHSETTINGS), SW_HIDE); break;
								case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hWnd, ID_STRINGEDITOR), SW_HIDE); break;
								case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hWnd, ID_WAVEDITOR), SW_HIDE); break;
							}
						}
						break;
				}
			}
			break;

		case REFRESH_WINDOW:
			SendMessage(GetDlgItem(hWnd, ID_MAPPROPERTIES), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_FORCES), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_UNITSETTINGS), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_UPGRADESETTINGS), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_TECHSETTINGS), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_STRINGEDITOR), REFRESH_WINDOW, NULL, NULL);
			SendMessage(GetDlgItem(hWnd, ID_WAVEDITOR), REFRESH_WINDOW, NULL, NULL);
			break;
	}

	return FALSE;
}

LRESULT CALLBACK MapPropertiesProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			{
				GuiMap* map = maps.curr;
				if ( map != nullptr )
				{
					string mapTitle, mapDescription;
					map->getMapTitle(mapTitle);
					map->getMapDescription(mapDescription);
					u16 tileset = map->getTileset(),
						currWidth = map->XSize(),
						currHeight = map->YSize();

					char sCurrWidth[12], sCurrHeight[12];
					_itoa_s(currWidth, sCurrWidth, 10);
					_itoa_s(currHeight, sCurrHeight, 10);

					SetWindowText(GetDlgItem(hWnd, ID_EDIT_MAPTITLE), mapTitle.c_str());
					SetWindowText(GetDlgItem(hWnd, ID_EDIT_MAPDESCRIPTION), mapDescription.c_str());
					SendMessage(GetDlgItem(hWnd, ID_CB_MAPTILESET), CB_SETCURSEL, tileset, NULL);
					PostMessage(GetDlgItem(hWnd, ID_CB_MAPTILESET), CB_SETEDITSEL, NULL, (-1, 0));
					SendMessage(GetDlgItem(hWnd, ID_CB_NEWMAPTERRAIN), CB_SETCURSEL, 0, NULL);
					PostMessage(GetDlgItem(hWnd, ID_CB_NEWMAPTERRAIN), CB_SETEDITSEL, NULL, (-1, 0));
					SetWindowText(GetDlgItem(hWnd, ID_EDIT_NEWMAPWIDTH), sCurrWidth);
					SetWindowText(GetDlgItem(hWnd, ID_EDIT_NEWMAPHEIGHT), sCurrHeight);
					
					for ( int player=0; player<12; player++ )
					{
						u8 displayOwner(map->getDisplayOwner(player)), race(0), color(0);
						map->getPlayerRace(player, race);
						SendMessage(GetDlgItem(hWnd, ID_CB_P1OWNER+player), CB_SETCURSEL, displayOwner, NULL);
						PostMessage(GetDlgItem(hWnd, ID_CB_P1OWNER+player), CB_SETEDITSEL, NULL, (-1, 0));
						SendMessage(GetDlgItem(hWnd, ID_CB_P1RACE+player), CB_SETCURSEL, race, NULL);
						PostMessage(GetDlgItem(hWnd, ID_CB_P1RACE+player), CB_SETEDITSEL, NULL, (-1, 0));

						if ( player < 8 )
						{
							map->getPlayerColor(player, color);
							SendMessage(GetDlgItem(hWnd, ID_CB_P1COLOR+player), CB_SETCURSEL, 0, NULL);
							PostMessage(GetDlgItem(hWnd, ID_CB_P1COLOR+player), CB_SETEDITSEL, NULL, (-1, 0));
						}
					}
				}
			}
			break;

		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
				SendMessage(hWnd, REFRESH_WINDOW, NULL, NULL);
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case WM_COMMAND:
			{
				switch ( LOWORD(wParam) )
				{
					case ID_EDIT_MAPTITLE:
						if ( HIWORD(wParam) == EN_KILLFOCUS )
						{
							string newMapTitle;
							if ( GetEditText(GetDlgItem(hWnd, ID_EDIT_MAPTITLE), newMapTitle) )
							{
								u16* mapTitleString;
								if ( maps.curr->SPRP().getPtr<u16>(mapTitleString, 0, 2) &&
									 parseEscapedString(newMapTitle) &&
									 maps.curr->replaceString(newMapTitle, *mapTitleString, false, true) )
								{
									maps.curr->notifyChange(false);
								}
							}
						}
						break;

					case ID_EDIT_MAPDESCRIPTION:
						if ( HIWORD(wParam) == EN_KILLFOCUS )
						{
							string newMapDescription;
							if ( GetEditText(GetDlgItem(hWnd, ID_EDIT_MAPDESCRIPTION), newMapDescription) )
							{
								u16* mapDescriptionString;
								if ( maps.curr->SPRP().getPtr<u16>(mapDescriptionString, 2, 2) &&
									 parseEscapedString(newMapDescription) &&
									 maps.curr->replaceString(newMapDescription, *mapDescriptionString, false, true) )
								{
									maps.curr->notifyChange(false);
								}
							}
						}
						break;

					case ID_BUTTON_APPLY:
						{
							if ( HIWORD(wParam) == BN_CLICKED )
							{
								HWND hButton = GetDlgItem(hWnd, ID_BUTTON_APPLY);
								DestroyWindow(hButton); // Seems to be the best way to remove stubborn highlighting
								CreateButton(hWnd, 494, 205, 91, 20, "Apply", ID_BUTTON_APPLY);
								LRESULT newTileset = SendMessage(GetDlgItem(hWnd, ID_CB_MAPTILESET), CB_GETCURSEL, NULL, NULL);
								maps.curr->setTileset((u16)newTileset);
								u16 newWidth, newHeight;
								if ( GetEditNum<u16>(hWnd, ID_EDIT_NEWMAPWIDTH, newWidth) && GetEditNum<u16>(hWnd, ID_EDIT_NEWMAPHEIGHT, newHeight) )
									maps.curr->setDimensions((u16)newWidth, (u16)newHeight);

								// Apply new terrain...

								maps.curr->notifyChange(false);
								maps.curr->Redraw(true);
							}
						}
						break;

					case ID_CB_MAPTILESET:
						if ( HIWORD(wParam) == CBN_SELCHANGE )
						{
							HWND hMapTileset = GetDlgItem(hWnd, ID_CB_MAPTILESET), hMapNewTerrain = GetDlgItem(hWnd, ID_CB_NEWMAPTERRAIN);
							LRESULT currTileset = SendMessage(hMapTileset, CB_GETCURSEL, NULL, NULL);
							if ( currTileset != CB_ERR && currTileset < NUM_TILESETS )
							{
								while ( SendMessage(hMapNewTerrain, CB_DELETESTRING, 0, NULL) != CB_ERR );

								for ( int i=0; i<numTilesetInitTerrains[currTileset]; i++ )
									SendMessage(hMapNewTerrain, CB_ADDSTRING, NULL, (LPARAM)initTerrains[currTileset][i]);

								SendMessage(hMapNewTerrain, WM_SETFONT, (WPARAM)defaultFont, MAKELPARAM(TRUE, 0));
								SendMessage(hMapNewTerrain, CB_SETCURSEL, 0, NULL);
								PostMessage(hMapNewTerrain, CB_SETEDITSEL, NULL, (-1, 0));
							}
						}
						break;
					case ID_CB_P1OWNER: case ID_CB_P2OWNER: case ID_CB_P3OWNER: case ID_CB_P4OWNER:
					case ID_CB_P5OWNER: case ID_CB_P6OWNER: case ID_CB_P7OWNER: case ID_CB_P8OWNER:
					case ID_CB_P9OWNER: case ID_CB_P10OWNER: case ID_CB_P11OWNER: case ID_CB_P12OWNER:
						if ( HIWORD(wParam) == CBN_SELCHANGE )
						{
							u32 player = LOWORD(wParam)-ID_CB_P1OWNER; // 0 based player
							u8 newOwner = 0;
							LRESULT sel = SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL);
							if ( player >= 0 && player < 12 && sel != CB_ERR )
							{
								switch ( sel )
								{
									case 0: newOwner = 4; break;
									case 1: newOwner = 3; break;
									case 2: newOwner = 5; break;
									case 3: newOwner = 6; break;
									case 4: newOwner = 7; break;
								}
								if ( newOwner != 0 )
								{
									maps.curr->setPlayerOwner((u8)player, newOwner);
									maps.curr->notifyChange(false);
								}
							}
						}
						break;

					case ID_CB_P1RACE: case ID_CB_P2RACE: case ID_CB_P3RACE: case ID_CB_P4RACE:
					case ID_CB_P5RACE: case ID_CB_P6RACE: case ID_CB_P7RACE: case ID_CB_P8RACE:
					case ID_CB_P9RACE: case ID_CB_P10RACE: case ID_CB_P11RACE: case ID_CB_P12RACE:
						if ( HIWORD(wParam) == CBN_SELCHANGE )
						{
							u32 player = LOWORD(wParam)-ID_CB_P1RACE; // 0 based player
							LRESULT newRace = SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL);
							if ( player >= 0 && player < 12 && newRace != CB_ERR && newRace >= 0 && newRace < 8 )
							{
								maps.curr->setPlayerRace((u8)player, (u8)newRace);
								maps.curr->notifyChange(false);
							}
						}
						break;

					case ID_CB_P1COLOR: case ID_CB_P2COLOR: case ID_CB_P3COLOR: case ID_CB_P4COLOR:
					case ID_CB_P5COLOR: case ID_CB_P6COLOR: case ID_CB_P7COLOR: case ID_CB_P8COLOR:
						switch ( HIWORD(wParam) )
						{
							case CBN_SELCHANGE:
								{
									u32 player = LOWORD(wParam)-ID_CB_P1COLOR; // 0 based player
									LRESULT newColor = SendMessage((HWND)lParam, CB_GETCURSEL, NULL, NULL);
									if ( player >= 0 && player < 12 && newColor != CB_ERR && newColor >= 0 && newColor < 16 )
									{
										if ( maps.curr->setPlayerColor((u8)player, (u8)newColor) )
											maps.curr->Redraw(true);

										maps.curr->notifyChange(false);
									}
								}
								break;
							case CBN_EDITCHANGE:
								{
									u32 player = LOWORD(wParam)-ID_CB_P1COLOR; // 0 based player
									u8 newColor;
									if ( GetEditNum<u8>((HWND)lParam, newColor) )
									{
										if ( maps.curr->setPlayerColor((u8)player, newColor) )
											maps.curr->Redraw(true);

										maps.curr->notifyChange(false);
									}
								}

						}
						break;
				}
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

LRESULT CALLBACK ForcesProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			{
				GuiMap* map = maps.curr;
				if ( map != nullptr )
				{
					for ( int force=0; force<4; force++ )
					{
						string forceName;
						bool allied = false, vision = false, random = false, av = false;
						forceName = map->getForceString(force);
						map->getForceInfo(force, allied, vision, random, av);

						SetWindowText(GetDlgItem(hWnd, ID_EDIT_F1NAME+force), forceName.c_str());
						if ( allied ) SendMessage(GetDlgItem(hWnd, ID_CHECK_F1ALLIED+force), BM_SETCHECK, BST_CHECKED  , NULL);
						else		  SendMessage(GetDlgItem(hWnd, ID_CHECK_F1ALLIED+force), BM_SETCHECK, BST_UNCHECKED, NULL);
						if ( vision ) SendMessage(GetDlgItem(hWnd, ID_CHECK_F1VISION+force), BM_SETCHECK, BST_CHECKED  , NULL);
						else		  SendMessage(GetDlgItem(hWnd, ID_CHECK_F1VISION+force), BM_SETCHECK, BST_UNCHECKED, NULL);
						if ( random ) SendMessage(GetDlgItem(hWnd, ID_CHECK_F1RANDOM+force), BM_SETCHECK, BST_CHECKED  , NULL);
						else		  SendMessage(GetDlgItem(hWnd, ID_CHECK_F1RANDOM+force), BM_SETCHECK, BST_UNCHECKED, NULL);
						if ( av		) SendMessage(GetDlgItem(hWnd, ID_CHECK_F1AV	   +force), BM_SETCHECK, BST_CHECKED  , NULL);
						else		  SendMessage(GetDlgItem(hWnd, ID_CHECK_F1AV	   +force), BM_SETCHECK, BST_UNCHECKED, NULL);
					}

					for ( int i=0; i<4; i++ )
					{
						HWND hListBox = GetDlgItem(hWnd, ID_LB_F1PLAYERS+i);
						if ( hListBox != NULL )
							while ( SendMessage(hListBox, LB_DELETESTRING, 0, NULL) != LB_ERR );
					}

					for ( int player=0; player<8; player++ )
					{
						u8 force(0), color(0), race(0), displayOwner(map->getDisplayOwner(player));
						if ( map->getPlayerForce(player, force) )
						{
							map->getPlayerColor(player, color);
							map->getPlayerRace(player, race);
							stringstream ssplayer;
							ssplayer << "Player " << player+1 << " - " << playerColors[color] << " - "
									 << playerRaces[race] << " (" << playerOwners[displayOwner] << ")";
							HWND hListBox = GetDlgItem(hWnd, ID_LB_F1PLAYERS+force);
							if ( hListBox != NULL )
								SendMessage(hListBox, LB_ADDSTRING, NULL, (LPARAM)ssplayer.str().c_str());
						}
					}
				}
			}
			break;

		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
				SendMessage(hWnd, REFRESH_WINDOW, NULL, NULL);
			break;

		case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case BN_CLICKED:
					{
						GuiMap* map = maps.curr;
						LRESULT state = SendMessage(hWnd, BM_GETSTATE, NULL, NULL);
						if ( map != nullptr )
						{
							switch ( LOWORD(wParam) )
							{
								case ID_CHECK_F1ALLIED: case ID_CHECK_F2ALLIED: case ID_CHECK_F3ALLIED: case ID_CHECK_F4ALLIED:
									{
										int force = LOWORD(wParam)-ID_CHECK_F1ALLIED;
										if ( state == BST_CHECKED )
											map->setForceAllied(force, true);
										else
											map->setForceAllied(force, false);

										maps.curr->notifyChange(false);
									}
									break;

								case ID_CHECK_F1VISION: case ID_CHECK_F2VISION: case ID_CHECK_F3VISION: case ID_CHECK_F4VISION:
									{
										int force = LOWORD(wParam)-ID_CHECK_F1ALLIED;
										if ( state == BST_CHECKED )
											map->setForceVision(force, true);
										else
											map->setForceVision(force, false);

										maps.curr->notifyChange(false);
									}
									break;

								case ID_CHECK_F1RANDOM: case ID_CHECK_F2RANDOM: case ID_CHECK_F3RANDOM: case ID_CHECK_F4RANDOM:
									{
										int force = LOWORD(wParam)-ID_CHECK_F1ALLIED;
										if ( state == BST_CHECKED )
											map->setForceRandom(force, true);
										else
											map->setForceRandom(force, false);

										maps.curr->notifyChange(false);
									}
									break;

								case ID_CHECK_F1AV: case ID_CHECK_F2AV: case ID_CHECK_F3AV: case ID_CHECK_F4AV:
									{
										int force = LOWORD(wParam)-ID_CHECK_F1ALLIED;
										if ( state == BST_CHECKED )
											map->setForceAv(force, true);
										else
											map->setForceAv(force, false);

										maps.curr->notifyChange(false);
									}
									break;
							}
						}
					}
					break;
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
								LRESULT length = SendMessage(dragInfo->hWnd, LB_GETTEXTLEN, index, NULL)+1;
								char* str;
								try { str = new char[length]; } catch ( std::bad_alloc ) { return FALSE; }
								length = SendMessage(dragInfo->hWnd, LB_GETTEXT, index, (LPARAM)str);
								if ( length != LB_ERR && length > 8 && str[7] >= '1' && str[7] <= '8' )
								{
									playerBeingDragged = str[7]-'1';
									for ( int id=ID_LB_F1PLAYERS; id<=ID_LB_F4PLAYERS; id++ )
									{
										HWND hForceLb = GetDlgItem(hWnd, id);
										if ( hForceLb != dragInfo->hWnd && hForceLb != NULL )
											SendMessage(GetDlgItem(hWnd, id), LB_SETCURSEL, -1, NULL);
									}
									return TRUE;
								}
								else
									return FALSE;
							}
							break;

						case DL_CANCELDRAG:
							playerBeingDragged = 255;
							return DefWindowProc(hWnd, msg, wParam, lParam);
							break;

						case DL_DRAGGING:
							{
								HWND hUnder = WindowFromPoint(dragInfo->ptCursor);
								if ( hUnder != NULL )
								{
									LONG windowID = GetWindowLong(hUnder, GWL_ID);
									if ( windowID >= ID_LB_F1PLAYERS && windowID <= ID_LB_F4PLAYERS )
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
									if ( windowID >= ID_LB_F1PLAYERS && windowID <= ID_LB_F4PLAYERS )
									{
										int force = windowID-ID_LB_F1PLAYERS;
										GuiMap* map = maps.curr;
										if ( map != nullptr )
										{
											map->setPlayerForce(playerBeingDragged, force);
											SendMessage(hWnd, REFRESH_WINDOW, NULL, NULL);
											stringstream ssPlayer;
											ssPlayer << "Player " << playerBeingDragged+1;
											SendMessage(GetDlgItem(hWnd, ID_LB_F1PLAYERS+force), LB_SELECTSTRING, -1, (LPARAM)ssPlayer.str().c_str());
											map->notifyChange(false);
										}
									}
								}
								playerBeingDragged = 255;
								return DefWindowProc(hWnd, msg, wParam, lParam);
							}
							break;

						default:
							return DefWindowProc(hWnd, msg, wParam, lParam);
							break;
					}
				}
				else
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;
	}
	return 0;
}

LRESULT CALLBACK UnitSettingsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

LRESULT CALLBACK UpgradeSettingsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

LRESULT CALLBACK TechSettingsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

LRESULT CALLBACK StringPreviewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK StringGuideProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_CTLCOLORSTATIC:
			{
				SetBkMode((HDC)wParam, TRANSPARENT);
				int id = GetDlgCtrlID((HWND)lParam);
				if ( id >= ID_TEXT_COLOR_FIRST && id <= ID_TEXT_COLOR_LAST )
					SetTextColor((HDC)wParam, stringColors[id-ID_TEXT_COLOR_FIRST]);
				else
					SetTextColor((HDC)wParam, RGB(16, 252, 24));

				return (BOOL)CreateSolidBrush(RGB(0, 0, 0));
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

LRESULT CALLBACK StringEditorProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case WM_SHOWWINDOW:
			if ( wParam == TRUE )
				SendMessage(hWnd, REFRESH_WINDOW, NULL, NULL);
			else if ( wParam == FALSE )
				updateString(GetDlgItem(hWnd, ID_EDIT_STRING), currSelString);
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;

		case REFRESH_WINDOW:
			{
				HWND hStringList = GetDlgItem(hWnd, ID_LB_STRINGS);
				if ( hStringList != NULL && maps.curr != nullptr )
				{
					SendMessage(hStringList, LB_RESETCONTENT, NULL, NULL);
					bool success = false;
					StringUsageTable strUse(maps.curr->scenario(), false, success);
					if ( success )
					{
						string str;
						u32 lastUsed = strUse.lastUsedString();
						for ( u32 i=0; i<=lastUsed; i++ )
						{
							if ( strUse.isUsed(i) && maps.curr->getString(str, i) && str.size() > 0 )
							{
								int lbIndex = SendMessage(hStringList, LB_ADDSTRING, NULL, (LPARAM)"");
								if ( lbIndex != LB_ERR && lbIndex != LB_ERRSPACE )
									SendMessage(hStringList, LB_SETITEMDATA, lbIndex, i);
							}
						}
					}
				}
			}
			break;

		case WM_MOUSEWHEEL:
			{
				int distanceScrolled = int((s16(HIWORD(wParam)))/WHEEL_DELTA);
				HWND hStringSel = GetDlgItem(hWnd, ID_LB_STRINGS);
				if ( hStringSel != NULL )
					ListBox_SetTopIndex(hStringSel, ListBox_GetTopIndex(hStringSel)-distanceScrolled);
			}
			break;

		case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case LBN_SELCHANGE:
					if ( LOWORD(wParam) == ID_LB_STRINGS ) // Change selection, update info boxes and so fourth
					{
						HWND hEditString = GetDlgItem(hWnd, ID_EDIT_STRING);
						if ( hEditString != NULL && currSelString != 0 )
							updateString(hEditString, currSelString);
						HWND hStringUse = GetDlgItem(hWnd, ID_LB_STRINGUSE);
						if ( hStringUse != NULL )
							SendMessage(hStringUse, LB_RESETCONTENT, NULL, NULL);

						int lbIndex = SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0);
						if ( lbIndex != LB_ERR )
						{
							string str = "";
							currSelString = SendMessage((HWND)lParam, LB_GETITEMDATA, lbIndex, NULL);
							if ( currSelString != 0 && maps.curr != nullptr && maps.curr->getString(str, currSelString) && str.length() > 0 )
							{
								if ( hEditString != NULL )
									SetWindowText(hEditString, str.c_str());
						
								if ( hStringUse != NULL )
								{
									u32 locs, trigs, briefs, props, forces, wavs, units, switches;
									maps.curr->getStringUse(currSelString, locs, trigs, briefs, props, forces, wavs, units, switches);
									addUseItem(hStringUse, "Locations", locs);
									addUseItem(hStringUse, "Triggers", trigs);
									addUseItem(hStringUse, "Briefing Triggers", briefs);
									addUseItem(hStringUse, "Map Properties", props);
									addUseItem(hStringUse, "Forces", forces);
									addUseItem(hStringUse, "WAVs", wavs);
									addUseItem(hStringUse, "Units", units);
									addUseItem(hStringUse, "Switches", switches);
								}
								return 0;
							}
						}
						if ( hEditString != NULL )
							SetWindowText(hEditString, "");
					}
					break;
				case LBN_KILLFOCUS: // String list box item may have lost focus, check if string should be updated
					if ( LOWORD(wParam) == ID_LB_STRINGS )
					{
						HWND hEditString = GetDlgItem(hWnd, ID_EDIT_STRING);
						if ( hEditString != NULL && currSelString != 0 )
							updateString(hEditString, currSelString);
					}
					break;
				case EN_KILLFOCUS: // String edit box may have lost focus, check if string should be updated
					if ( LOWORD(wParam) == ID_EDIT_STRING && currSelString != 0 && updateString((HWND)lParam, currSelString) )
						maps.curr->refreshScenario();
					break;
				case BN_CLICKED:
					if ( LOWORD(wParam) == ID_DELETE_STRING  &&
						 MessageBox(hWnd, "Forcefully deleting a string could cause problems, continue?", "Warning", MB_ICONEXCLAMATION|MB_YESNO) == IDYES &&
						 maps.curr != nullptr && currSelString != 0 && maps.curr->stringExists(currSelString)
					   )
					{
						maps.curr->forceDeleteString(currSelString);
						maps.curr->refreshScenario();
					}
					break;
			}
			return 0;
			break;

		case WM_MEASUREITEM:
			{
				MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
				HWND hStringList = GetDlgItem(hWnd, ID_LB_STRINGS);
				SIZE strSize;
				string str;

				if ( hStringList != NULL && maps.curr->getString(str, mis->itemData) && str.size() > 0 )
				{
					HDC hDC = GetDC(hStringList);
					if ( hDC != NULL )
					{
						HGDIOBJ sel = SelectObject(hDC, defaultFont);
						if ( sel != NULL && sel != HGDI_ERROR &&
							 GetTextExtentPoint32(hDC, (LPCSTR)str.c_str(), str.size(), &strSize) != 0 )
						{
							mis->itemWidth = strSize.cx+5;
							mis->itemHeight = strSize.cy+2;
							ReleaseDC(hWnd, hDC);
							return TRUE;
						}
						ReleaseDC(hWnd, hDC);
					}
				}
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;

		case WM_DRAWITEM:
			{
				PDRAWITEMSTRUCT pdis = (PDRAWITEMSTRUCT)lParam;

				if ( pdis->itemID != -1 && pdis->itemAction == ODA_SELECT || pdis->itemAction == ODA_DRAWENTIRE )
				{
					string str;
					HBRUSH hBlack = CreateSolidBrush(RGB(0, 0, 0));
					if ( hBlack != NULL )
					{
						FillRect(pdis->hDC, &pdis->rcItem, hBlack);
						DeleteObject(hBlack);
					}
					if ( pdis->itemState & ODS_SELECTED )
						DrawFocusRect(pdis->hDC, &pdis->rcItem);
					
					if ( maps.curr != nullptr && maps.curr->getString(str, pdis->itemData) && str.size() > 0 )
					{
						SetBkMode(pdis->hDC, TRANSPARENT);
						SetTextColor(pdis->hDC, RGB(16, 252, 24));
						ExtTextOut(pdis->hDC, pdis->rcItem.left+3, pdis->rcItem.top+1, ETO_CLIPPED, &pdis->rcItem, (LPCSTR)str.c_str(), str.size(), 0);
					}
				}
				return TRUE;
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

LRESULT CALLBACK WavEditorProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch ( msg )
	{
		case REFRESH_WINDOW:
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void ChangeMapSettingsTab(HWND hWnd, u32 tabID)
{
	TabCtrl_SetCurSel(GetDlgItem(hWnd, IDC_MAPSETTINGSTABS), tabID);

	ShowWindow(GetDlgItem(hWnd, ID_MAPPROPERTIES), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_FORCES), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_UNITSETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_UPGRADESETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_TECHSETTINGS), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_STRINGEDITOR), SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_WAVEDITOR), SW_HIDE);

	switch ( tabID )
	{
		case ID_TAB_MAPPROPERTIES: ShowWindow(GetDlgItem(hWnd, ID_MAPPROPERTIES), SW_SHOW); break;
		case ID_TAB_FORCES: ShowWindow(GetDlgItem(hWnd, ID_FORCES), SW_SHOW); break;
		case ID_TAB_UNITSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UNITSETTINGS), SW_SHOW); break;
		case ID_TAB_UPGRADESETTINGS: ShowWindow(GetDlgItem(hWnd, ID_UPGRADESETTINGS), SW_SHOW); break;
		case ID_TAB_TECHSETTINGS: ShowWindow(GetDlgItem(hWnd, ID_TECHSETTINGS), SW_SHOW); break;
		case ID_TAB_STRINGEDITOR: ShowWindow(GetDlgItem(hWnd, ID_STRINGEDITOR), SW_SHOW); break;
		case ID_TAB_WAVEDITOR: ShowWindow(GetDlgItem(hWnd, ID_WAVEDITOR), SW_SHOW); break;
	}

	currTab = tabID;
}

void CreateMapProperties(HWND hParent)
{
	GuiMap* map = maps.curr;
	HWND hMapProperties = CreateTabWindow(hParent, "MapProperties", ID_MAPPROPERTIES);

	std::string mapTitle(""), mapDescription("");
	u16 currTileset = 0,
		currWidth = 0,
		currHeight = 0;

	if ( map != nullptr )
	{
		map->getMapTitle(mapTitle);
		map->getMapDescription(mapDescription);
		currTileset = map->getTileset(),
		currWidth = map->XSize(),
		currHeight = map->YSize();
	}

	CreateStaticText(hMapProperties, 5, 5, 50, 20, "Map Title");
	CreateEditBox(hMapProperties, 5, 25, 582, 20, mapTitle.c_str(), false, ID_EDIT_MAPTITLE);

	CreateStaticText(hMapProperties, 5, 60, 100, 20, "Map Description");
	CreateEditBox(hMapProperties, 5, 80, 582, 100, mapDescription.c_str(), true, ID_EDIT_MAPDESCRIPTION);

	char sCurrWidth[12], sCurrHeight[12];
	_itoa_s(currWidth, sCurrWidth, 10);
	_itoa_s(currHeight, sCurrHeight, 10);

	CreateStaticText(hMapProperties, 5, 185, 100, 20, "Map Tileset");
	CreateDropdownBox(hMapProperties, 5, 205, 185, 400, (const char**)tilesetNames, NUM_TILESETS, currTileset, false, ID_CB_MAPTILESET);
	CreateStaticText(hMapProperties, 195, 185, 100, 20, "[New] Terrain");
	CreateDropdownBox(hMapProperties, 195, 205, 185, 400, (const char**)initTerrains[currTileset], numTilesetInitTerrains[currTileset], 0, false, ID_CB_NEWMAPTERRAIN);
	CreateStaticText(hMapProperties, 385, 185, 50, 20, "Width");
	CreateEditBox(hMapProperties, 385, 205, 50, 20, sCurrWidth, false, ID_EDIT_NEWMAPWIDTH);
	CreateStaticText(hMapProperties, 440, 185, 50, 20, "Height");
	CreateEditBox(hMapProperties, 440, 205, 50, 20, sCurrHeight, false, ID_EDIT_NEWMAPHEIGHT);
	CreateButton(hMapProperties, 494, 205, 91, 20, "Apply", ID_BUTTON_APPLY);

	const char* sPlayers[] = { "Player 1", "Player 2", "Player 3", "Player 4",
							  "Player 5", "Player 6", "Player 7", "Player 8",
							  "Player 9", "Player 10", "Player 11", "Player 12" };

	for ( int yBox=0; yBox<3; yBox++ )
	{
		for ( int xBox=0; xBox<4; xBox++ )
		{
			int player = yBox*4+xBox;
			u8 displayOwner = 0, race = 0, color = (u8)player;
			if ( map != nullptr )
			{
				displayOwner = map->getDisplayOwner((u8)player);
				map->getPlayerRace((u8)player, race);
			}

			CreateGroupBox(hMapProperties, 5+146*xBox, 242+95*yBox, 141, 91, sPlayers[yBox*4+xBox]);
			CreateStaticText(hMapProperties, 15+146*xBox, 257+95*yBox, 50, 20, "Owner");
			CreateDropdownBox(hMapProperties, 60+146*xBox, 257+95*yBox, 80, 140, playerOwners, numPlayerOwners, displayOwner, false, ID_CB_P1OWNER+player);
			CreateStaticText(hMapProperties, 15+146*xBox, 282+95*yBox, 50, 20, "Race");
			CreateDropdownBox(hMapProperties, 60+146*xBox, 282+95*yBox, 80, 110, playerRaces, numPlayerRaces, race, false, ID_CB_P1RACE+player);

			if ( yBox < 2 )
			{
				if ( map != nullptr )
					map->getPlayerColor((u8)player, color);
				CreateStaticText(hMapProperties, 15+146*xBox, 307+95*yBox, 50, 20, "Color");
				CreateDropdownBox(hMapProperties, 60+146*xBox, 307+95*yBox, 80, 140, playerColors, numPlayerColors, color, true, ID_CB_P1COLOR+player);
			}
		}
	}
}

void CreateForces(HWND hParent)
{
	GuiMap* map = maps.curr;
	HWND hForces = CreateTabWindow(hParent, "Forces", ID_FORCES);
	CreateStaticText(hForces, 5, 10, 587, 20, "Designate player forces, set force names, and force properties. It is recommended to separate computer and human players");

	const char* forceGroups[] = { "Force 1", "Force 2", "Force 3", "Force 4" };
	for ( int y=0; y<2; y++ )
	{
		for ( int x=0; x<2; x++ )
		{
			int force = x+y*2;
			string forceName = "";
			bool allied = false, vision = false, random = false, av = false;
			if ( map != nullptr )
			{
				forceName = map->getForceString(force);
				map->getForceInfo(force, allied, vision, random, av);
			}

			CreateGroupBox(hForces, 5+293*x, 50+239*y, 288, 234, forceGroups[force]);
			CreateEditBox(hForces, 20+293*x, 70+239*y, 268, 20, forceName.c_str(), false, ID_EDIT_F1NAME+force);
			CreateDragListBox(hForces, 20+293*x, 95+239*y, 268, 121, ID_LB_F1PLAYERS+force);
			CreateCheckBox(hForces, 15+293*x, 232+239*y, 100, 20, allied, "Allied", ID_CHECK_F1ALLIED+force);
			CreateCheckBox(hForces, 15+293*x, 252+239*y, 100, 20, vision, "Share Vision", ID_CHECK_F1VISION+force);
			CreateCheckBox(hForces, 125+293*x, 232+239*y, 150, 20, random, "Randomize Start Location", ID_CHECK_F1RANDOM+force);
			CreateCheckBox(hForces, 125+293*x, 252+239*y, 150, 20, av, "Enable Allied Victory", ID_CHECK_F1AV+force);
		}
	}
	WM_DRAGNOTIFY = RegisterWindowMessage(DRAGLISTMSGSTRING);
}

void CreateUnitSettings(HWND hParent)
{
	HWND hUnitSettings = CreateTabWindow(hParent, "UnitSettings", ID_UNITSETTINGS);
	CreateStaticText(hUnitSettings, 5, 5, 100, 20, "Unit Settings...");
}

void CreateUpgradeSettings(HWND hParent)
{
	HWND hUpgradeSettings = CreateTabWindow(hParent, "UpgradeSettings", ID_UPGRADESETTINGS);
	CreateStaticText(hUpgradeSettings, 5, 5, 100, 20, "Upgrade Settings...");
}

void CreateTechSettings(HWND hParent)
{
	HWND hTechSettings = CreateTabWindow(hParent, "TechSettings", ID_TECHSETTINGS);
	CreateStaticText(hTechSettings, 5, 5, 100, 20, "Tech Settings...");
}

void CreateStringEditor(HWND hParent)
{
	HWND hStringEditor = CreateTabWindow(hParent, "StringEditor", ID_STRINGEDITOR);
	CreateStaticText(hStringEditor, 5, 5, 100, 20, "String Editor...");

	HWND hStringList = CreateListBox(hStringEditor, 5, 25, 453, 262, ID_LB_STRINGS, true);

	CreateButton(hStringEditor, 130, 290, 200, 20, "Delete String", ID_DELETE_STRING);
	CreateCheckBox(hStringEditor, 20, 294, 100, 10, false, "Extended", ID_CHECK_EXTENDEDSTRING);
	HWND hEditString = CreateEditBox(hStringEditor, 5, 310, 453, 140, "", true, ID_EDIT_STRING);
	wpStringEdit = (WNDPROC)SetWindowLong(hEditString, GWL_WNDPROC, (LONG)&StringEditProc);

	CreateStaticText(hStringEditor, 480, 379, 125, 20, "String Usage:");
	HWND hStringUse = CreateListBox(hStringEditor, 463, 394, 125, 83, ID_LB_STRINGUSE, false);

	HWND hStringPreview = CreateWindow( "StringPreview", NULL, WS_OVERLAPPED|WS_CHILD|WS_VISIBLE,
										5, 455, 583, 70, hStringEditor, (HMENU)ID_PREVIEW_STRING, NULL, NULL );

	HWND hStringGuide = CreateWindow( "StringGuide", NULL, WS_OVERLAPPED|WS_CHILD|WS_VISIBLE,
									  463, 7, 125, 365, hStringEditor, (HMENU)ID_STRING_GUIDE, NULL, NULL );

	HDC hDC = GetDC(hStringGuide);
	SelectObject(hDC, defaultFont);
	SIZE strSize = { };
	for ( int i=0; i<numStringColors; i++ )
	{
		GetTextExtentPoint32(hDC, stringColorPrefixes[i], strlen(stringColorPrefixes[i]), &strSize);
		CreateStaticText(hStringGuide, 2, i*13, strSize.cx, 13, stringColorPrefixes[i]);
		CreateStaticText(hStringGuide, strSize.cx+3, i*13, 100, 13, stringColorStrings[i], ID_TEXT_COLOR_FIRST+i);
	}
	ReleaseDC(hStringGuide, hDC);

	SendMessage(hStringEditor, REFRESH_WINDOW, NULL, NULL);
}

void CreateWavEditor(HWND hParent)
{
	HWND hWavEditor = CreateTabWindow(hParent, "WavEditor", ID_WAVEDITOR);
	CreateStaticText(hWavEditor, 5, 5, 100, 20, "Wav Editor...");
}

void addUseItem(HWND hStringUse, const char* title, u32 amount)
{
	stringstream ss;
	if ( amount > 0 )
	{
		ss << title << ": " << amount;
		SendMessage(hStringUse, LB_ADDSTRING, NULL, (LPARAM)ss.str().c_str());
	}
}

bool updateString(HWND hEditString, u32 stringNum)
{
	string editStr;
	if ( maps.curr != nullptr && GetEditText(hEditString, editStr) && maps.curr->escStringDifference(editStr, stringNum) )
	{
		if ( parseEscapedString(editStr) &&
			 maps.curr->editString<u32>(editStr, stringNum, maps.curr->isExtendedString(stringNum), false) )
		{
			if ( maps.curr->stringUsedWithLocs(currSelString) )
				BuildLocationTree(maps.curr);
			RedrawWindow((HWND)hEditString, NULL, NULL, RDW_INVALIDATE);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}