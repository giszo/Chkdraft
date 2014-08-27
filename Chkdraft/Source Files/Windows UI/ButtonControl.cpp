#include "ButtonControl.h"

bool ButtonControl::CreateThis(HWND hParent, s32 x, s32 y, s32 width, s32 height, const char* initText, u32 id)
{
	return WindowControl::CreateControl( WS_EX_CLIENTEDGE, "BUTTON", initText, WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,
										 x, y, width, height, hParent, (HMENU)id, false );
}