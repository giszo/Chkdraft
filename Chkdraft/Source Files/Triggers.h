#ifndef TRIGGERS_H
#define TRIGGERS_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include "Windows UI/WindowsUI.h"
#include "TextTrigGenerator.h"
#include "PeerListBox.h"
#include "TrigModify.h"

class TriggersWindow : public ClassWindow
{
	public:
		TriggersWindow();
		bool CreateThis(HWND hParent);
		bool DestroyThis();
		void RefreshWindow();
		void DoSize();

		void DeleteSelection();
		void CopySelection();
		void MoveUp();
		void MoveDown();
		void MoveTo();

	protected:
		bool SelectTrigListItem(int listIndex); // Attempts to select item at listIndex, updating currTrigger

		void CreateSubWindows(HWND hWnd);
		void RefreshGroupList();
		void RefreshTrigList();
		void ButtonNew();
		void ButtonModify();

		/** Attempts to delete the item at listIndex and decrement higher item's trigger indexes
			If this method returns false, parts of the trigger list may be invalid */
		bool DeleteTrigListItem(int listIndex);

		/** Attempts to copy the item at listIndex and increment higher item's trigger indexes
			If this method returns false, parts of the trigger list may be invalid */
		bool CopyTrigListItem(int listIndex);

		/** Attempts to swap the item at listIndex with the item at listIndex-1
			If this method returns false, parts of the trigger list may be invalid */
		bool MoveUpTrigListItem(int listIndex, u32 prevTrigIndex);

		/** Attempt to swap the item at listIndex with the item at listIndex+1
			If this method returns false, parts of the trigger list may be invalid */
		bool MoveDownTrigListItem(int listIndex, u32 nextTrigIndex);

		/** Attempt to move the item at currListIndex to the appropriate space considering
			targetTrigIndex, listIndexMovedTo is determined by the trigIndexes and does not
			need to be set prior to calling this function.
			returns true and sets listIndexMovedTo if successful */
		bool MoveTrigListItemTo(int currListIndex, u32 currTrigIndex, u32 targetTrigIndex, int &listIndexMovedTo);

		bool FindTargetListIndex(int currListIndex, u32 currTrigIndex, u32 targetTrigIndex, int &targetListIndex);

	private:
		u32 currTrigger; // The trig index of the current trigger
		bool groupSelected[NUM_TRIG_PLAYERS];
		bool displayAll;
		u32 numVisibleTrigs;
		bool changeGroupHighlightOnly;

		TrigModifyWindow trigModifyWindow;

		ButtonControl buttonNew, buttonModify, buttonDelete, buttonCopy,
					  buttonMoveUp, buttonMoveDown, buttonMoveTo;

		PeerListBox listGroups;
		ListBoxControl listTriggers;
		TextTrigGenerator tt; // Text trig generator for assisting trigger display
		HDC trigListDC; // Trig list HDC for speeding up trigger measurement

		bool ShowTrigger(Trigger* trigger); // Checks if trigger should currently be shown
		void ClearGroups();
		string GetConditionString(u8 conditionNum, Trigger* trigger);
		string GetActionString(u8 actionNum, Trigger* trigger);
		string GetTriggerString(u32 trigNum, Trigger* trigger);
		bool GetTriggerDrawSize(HDC hDC, UINT &width, UINT &height, Scenario* chk, u32 triggerNum, Trigger* trigger);
		void DrawGroup(HDC hDC, RECT &rcItem, bool isSelected, u8 groupNum);
		void DrawTrigger(HDC hDC, RECT &rcItem, bool isSelected, Scenario* chk, u32 triggerNum, Trigger* trigger);
		void PrepDoubleBuffer(HWND hWnd, HDC hDC);
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif