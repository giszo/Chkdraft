#include "UnitSettings.h"
#include "Chkdraft.h"
#include <sstream>
#include <string>

enum ID
{
    TREE_UNITSETTINGS = ID_FIRST,
    BUTTON_RESETALLUNITDEFAULTS,
    CHECK_USEUNITDEFAULTS,
    CHECK_ENABLEDBYDEFAULT,
    GROUP_UNITPROPERTIES,
    GROUP_GROUNDWEAPON,
    GROUP_AIRWEAPON,
    GROUP_UNITNAME,
    GROUP_UNITAVAILABILITY,
    TEXT_UNITHITPOINTS,
    EDIT_UNITHITPOINTS,
    EDIT_UNITHITPOINTSBYTE,
    TEXT_UNITSHIELDPOINTS,
    EDIT_UNITSHIELDPOINTS,
    TEXT_UNITARMOR,
    EDIT_UNITARMOR,
    TEXT_UNITBUILDTIME,
    EDIT_UNITBUILDTIME,
    TEXT_UNITMINERALCOST,
    EDIT_UNITMINERALCOST,
    TEXT_UNITGASCOST,
    EDIT_UNITGASCOST,
    TEXT_UNITGROUNDDAMAGE,
    EDIT_UNITGROUNDDAMAGE,
    TEXT_UNITGROUNDBONUS,
    EDIT_UNITGROUNDBONUS,
    TEXT_UNITAIRDAMAGE,
    EDIT_UNITAIRDAMAGE,
    TEXT_UNITAIRBONUS,
    EDIT_UNITAIRBONUS,
    CHECK_USEDEFAULTUNITNAME,
    EDIT_UNITNAME,
    TEXT_P1UNITAVAILABILITY,
    TEXT_P12UNITAVAILABILITY = (TEXT_P1UNITAVAILABILITY + 11),
    DROP_P1UNITAVAILABILITY,
    DROP_P12UNITAVAILABILITY = (DROP_P1UNITAVAILABILITY + 11)
};

UnitSettingsWindow::UnitSettingsWindow() : selectedUnit(-1), possibleUnitNameUpdate(false), isDisabled(true), refreshing(false)
{

}

bool UnitSettingsWindow::CreateThis(HWND hParent, u32 windowId)
{
    if ( getHandle() != NULL )
        return SetParent(hParent);

    if ( ClassWindow::RegisterWindowClass(0, NULL, NULL, NULL, NULL, "UnitSettings", NULL, false) &&
         ClassWindow::CreateClassWindow(0, "UnitSettings", WS_VISIBLE|WS_CHILD, 4, 22, 592, 524, hParent, (HMENU)windowId) )
    {
        CreateSubWindows(getHandle());
        return true;
    }
    else
        return false;
}

bool UnitSettingsWindow::DestroyThis()
{
    return true;
}

void UnitSettingsWindow::RefreshWindow()
{
    refreshing = true;
    if ( selectedUnit >= 0 && selectedUnit < 228 && CM != nullptr )
    {
        u16 unitId = (u16)selectedUnit;
        if ( isDisabled )
            EnableUnitEditing();

        bool useDefaultSettings = CM->unitUsesDefaultSettings(unitId);
        if ( useDefaultSettings )
        {
            checkUseUnitDefaults.SetCheck(true);
            DisableUnitProperties();
        }
        else
        {
            checkUseUnitDefaults.SetCheck(false);
            EnableUnitProperties();
        }

        UNITDAT* unitDat = chkd.scData.UnitDat(unitId);
        u32 hitpoints,
            groundWeapon = (u32)unitDat->GroundWeapon,
            airWeapon    = (u32)unitDat->AirWeapon;
        u16 shieldPoints, buildTime, mineralCost, gasCost, baseGroundWeapon, bonusGroundWeapon,
            baseAirWeapon, bonusAirWeapon,
            subUnitId = unitDat->Subunit1;
        u8 armor, hitpointsByte;
        
        if ( subUnitId != 228 ) // If unit has a subunit
        {
            if ( groundWeapon == 130 ) // If unit might have a subunit ground attack
                groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon;
            if ( airWeapon == 130 ) // If unit might have a subunit air attack
                airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon;
        }

        if ( groundWeapon == 130 )
        {
            groupGroundWeapon.SetText("No Ground Weapon");
            groupGroundWeapon.DisableThis();
            editGroundDamage.SetText("");
            editGroundDamage.DisableThis();
            editGroundBonus.SetText("");
            editGroundBonus.DisableThis();
            textGroundDamage.DisableThis();
            textGroundBonus.DisableThis();
        }
        else
        {
            groupGroundWeapon.SetText("Ground Weapon [" + weaponNames.at(groundWeapon) + "]");
        }

        if ( airWeapon == 130 || airWeapon == groundWeapon )
        {
            if ( airWeapon != 130 && airWeapon == groundWeapon )
            {
                groupAirWeapon.SetText("Air Weapon [" + weaponNames.at(airWeapon) + "]");
            }
            else
                groupAirWeapon.SetText("No Air Weapon");

            groupAirWeapon.DisableThis();
            editAirDamage.SetText("");
            editAirDamage.DisableThis();
            editAirBonus.SetText("");
            editAirBonus.DisableThis();
            textAirDamage.DisableThis();
            textAirBonus.DisableThis();
        }
        else
        {
            groupAirWeapon.SetText("Air Weapon [" + weaponNames.at(airWeapon) + "]");
        }

        if ( useDefaultSettings )
        {
            editHitPoints.SetEditNum<u32>(unitDat->HitPoints/256);
            editHitPointsByte.SetEditNum<u32>(0);
            editShieldPoints.SetEditNum<u16>(unitDat->ShieldAmount);
            editArmor.SetEditNum<u8>(unitDat->Armor);
            editBuildTime.SetEditNum<u16>(unitDat->BuildTime);
            editMineralCost.SetEditNum<u16>(unitDat->MineralCost);
            editGasCost.SetEditNum<u16>(unitDat->VespeneCost);
            if ( groundWeapon != 130 )
            {
                editGroundDamage.SetEditNum<u16>(chkd.scData.WeaponDat(groundWeapon)->DamageAmount);
                editGroundBonus.SetEditNum<u16>(chkd.scData.WeaponDat(groundWeapon)->DamageBonus);
            }
            if ( airWeapon != 130 && airWeapon != groundWeapon )
            {
                editAirDamage.SetEditNum<u16>(chkd.scData.WeaponDat(airWeapon)->DamageAmount);
                editAirBonus.SetEditNum<u16>(chkd.scData.WeaponDat(airWeapon)->DamageBonus);
            }
        }
        else // Not default settings
        {
            if ( CM->getUnitSettingsHitpoints(unitId, hitpoints) )
                editHitPoints.SetEditNum<u32>(hitpoints);
            if ( CM->getUnitSettingsHitpointByte(unitId, hitpointsByte) )
                editHitPointsByte.SetEditNum<u8>(hitpointsByte);
            if ( CM->getUnitSettingsShieldPoints(unitId, shieldPoints) )
                editShieldPoints.SetEditNum<u16>(shieldPoints);
            if ( CM->getUnitSettingsArmor(unitId, armor) )
                editArmor.SetEditNum<u8>(armor);
            if ( CM->getUnitSettingsBuildTime(unitId, buildTime) )
                editBuildTime.SetEditNum<u16>(buildTime);
            if ( CM->getUnitSettingsMineralCost(unitId, mineralCost) )
                editMineralCost.SetEditNum<u16>(mineralCost);
            if ( CM->getUnitSettingsGasCost(unitId, gasCost) )
                editGasCost.SetEditNum<u16>(gasCost);
            if ( CM->getUnitSettingsBaseWeapon(groundWeapon, baseGroundWeapon) )
                editGroundDamage.SetEditNum<u16>(baseGroundWeapon);
            if ( CM->getUnitSettingsBonusWeapon(groundWeapon, bonusGroundWeapon) )
                editGroundBonus.SetEditNum<u16>(bonusGroundWeapon);
            if ( airWeapon != groundWeapon && CM->getUnitSettingsBaseWeapon(airWeapon, baseAirWeapon) )
                editAirDamage.SetEditNum<u16>(baseAirWeapon);
            if ( airWeapon != groundWeapon && CM->getUnitSettingsBonusWeapon(airWeapon, bonusAirWeapon) )
                editAirBonus.SetEditNum<u16>(bonusAirWeapon);
        }

        checkEnabledByDefault.SetCheck(CM->unitIsEnabled(unitId));

        for ( int i=0; i<12; i++ )
        {
            UnitEnabledState enabledState = CM->getUnitEnabledState(unitId, (u8)i);
            if ( enabledState == UnitEnabledState::Default )
                dropPlayerAvailability[i].SetSel(0);
            else if ( enabledState == UnitEnabledState::Enabled )
                dropPlayerAvailability[i].SetSel(1);
            else if ( enabledState == UnitEnabledState::Disabled )
                dropPlayerAvailability[i].SetSel(2);
        }

        u16 unitStringNum = 0;
        if ( CM->getUnitStringNum(unitId, unitStringNum) && unitStringNum == 0 )
        {
            editUnitName.DisableThis();
            checkUseDefaultName.SetCheck(true);
        }
        else
        {
            if ( !useDefaultSettings )
                editUnitName.EnableThis();
            checkUseDefaultName.SetCheck(false);
        }
            
        ChkdString unitName;
        CM->getUnitName(unitName, unitId);
        editUnitName.SetText(unitName.c_str());
        chkd.mapSettingsWindow.SetWinText((std::string("Map Settings - [") + DefaultUnitDisplayName[unitId] + ']').c_str());
    }
    else
        DisableUnitEditing();

    refreshing = false;
}

void UnitSettingsWindow::CreateSubWindows(HWND hParent)
{
    unitTree.UpdateUnitNames(DefaultUnitDisplayName);
    unitTree.CreateThis(hParent, 5, 5, 200, 489, false, TREE_UNITSETTINGS);
    buttonResetUnitDefaults.CreateThis(hParent, 5, 494, 200, 25, "Reset All Units To Default", BUTTON_RESETALLUNITDEFAULTS);
    checkUseUnitDefaults.CreateThis(hParent, 210, 5, 100, 20, false, "Use Unit Defaults", CHECK_USEUNITDEFAULTS);
    checkEnabledByDefault.CreateThis(hParent, 403, 5, 120, 20, false, "Enabled by Default", CHECK_ENABLEDBYDEFAULT);

    groupUnitProperties.CreateThis(hParent, 210, 25, 377, 292, "Unit Properties", GROUP_UNITPROPERTIES);
    textHitPoints.CreateThis(hParent, 215, 40, 75, 20, "Hit Points", TEXT_UNITHITPOINTS);
    editHitPoints.CreateThis(hParent, 303, 40, 63, 20, false, EDIT_UNITHITPOINTS);
    editHitPointsByte.CreateThis(hParent, 371, 40, 15, 20, false, EDIT_UNITHITPOINTSBYTE);
    textShieldPoints.CreateThis(hParent, 215, 65, 83, 20, "Shield Points", TEXT_UNITSHIELDPOINTS);
    editShieldPoints.CreateThis(hParent, 303, 65, 83, 20, false, EDIT_UNITSHIELDPOINTS);
    textArmor.CreateThis(hParent, 215, 90, 75, 20, "Armor", TEXT_UNITARMOR);
    editArmor.CreateThis(hParent, 303, 90, 83, 20, false, EDIT_UNITARMOR);
    textBuildTime.CreateThis(hParent, 411, 40, 75, 20, "Build Time", TEXT_UNITBUILDTIME);
    editBuildTime.CreateThis(hParent, 499, 40, 83, 20, false, EDIT_UNITBUILDTIME);
    textMineralCost.CreateThis(hParent, 411, 65, 75, 20, "Mineral Cost", TEXT_UNITMINERALCOST);
    editMineralCost.CreateThis(hParent, 499, 65, 83, 20, false, EDIT_UNITMINERALCOST);
    textGasCost.CreateThis(hParent, 411, 90, 75, 20, "Gas Cost", TEXT_UNITGASCOST);
    editGasCost.CreateThis(hParent, 499, 90, 83, 20, false, EDIT_UNITGASCOST);

    groupGroundWeapon.CreateThis(hParent, 215, 115, 367, 62, "Ground Weapon [NAME]", GROUP_GROUNDWEAPON);
    textGroundDamage.CreateThis(hParent, 220, 135, 75, 20, "Damage", TEXT_UNITGROUNDDAMAGE);
    editGroundDamage.CreateThis(hParent, 308, 135, 83, 20, false, EDIT_UNITGROUNDDAMAGE);
    textGroundBonus.CreateThis(hParent, 401, 135, 75, 20, "Bonus", TEXT_UNITGROUNDBONUS);
    editGroundBonus.CreateThis(hParent, 489, 135, 83, 20, false, EDIT_UNITGROUNDBONUS);

    groupAirWeapon.CreateThis(hParent, 215, 182, 367, 62, "Air Weapon [NAME]", GROUP_AIRWEAPON);
    textAirDamage.CreateThis(hParent, 220, 202, 75, 20, "Damage", TEXT_UNITAIRDAMAGE);
    editAirDamage.CreateThis(hParent, 308, 202, 83, 20, false, EDIT_UNITAIRDAMAGE);
    textAirBonus.CreateThis(hParent, 401, 202, 75, 20, "Bonus", TEXT_UNITAIRBONUS);
    editAirBonus.CreateThis(hParent, 489, 202, 83, 20, false, EDIT_UNITAIRBONUS);

    groupUnitName.CreateThis(hParent, 215, 249, 367, 62, "Unit Name", GROUP_UNITNAME);
    checkUseDefaultName.CreateThis(hParent, 220, 269, 75, 20, false, "Use Default", CHECK_USEDEFAULTUNITNAME);
    editUnitName.CreateThis(hParent, 342, 269, 234, 20, false, EDIT_UNITNAME);

    for ( int y=0; y<6; y++ )
    {
        for ( int x=0; x<2; x++ )
        {
            int player = y*2+x;
            std::stringstream ssPlayer;
            if ( player < 9 )
                ssPlayer << "Player 0" << player+1;
            else
                ssPlayer << "Player " << player+1;

            textPlayerAvailability[player].CreateThis(hParent, 215+188*x, 336+27*y, 75, 20, ssPlayer.str().c_str(), TEXT_P1UNITAVAILABILITY+player);
        }
    }

    const char* items[] = { "Default", "Yes", "No" };
    int numItems = sizeof(items)/sizeof(const char*);

    for ( int y=0; y<6; y++ )
    {
        for ( int x=0; x<2; x++ )
        {
            int player = y*2+x;
            dropPlayerAvailability[player].CreateThis( hParent, 304+188*x, 336+27*y, 84, 100, false, false,
                DROP_P1UNITAVAILABILITY+player, numItems, items, defaultFont );
        }
    }

    groupUnitAvailability.CreateThis(hParent, 210, 321, 372, 198, "Unit Availability", GROUP_UNITAVAILABILITY);

    DisableUnitEditing();
}

void UnitSettingsWindow::DisableUnitEditing()
{
    isDisabled = true;
    chkd.mapSettingsWindow.SetWinText("Map Settings");
    DisableUnitProperties();
    checkUseUnitDefaults.DisableThis();
    checkEnabledByDefault.DisableThis();

    groupUnitAvailability.DisableThis();
    for ( int i=0; i<12; i++ )
    {
        textPlayerAvailability[i].DisableThis();
        dropPlayerAvailability[i].DisableThis();
    }
}

void UnitSettingsWindow::EnableUnitEditing()
{
    isDisabled = false;
    if ( selectedUnit >= 0 && CM->unitUsesDefaultSettings((u16)selectedUnit) == false )
        EnableUnitProperties();

    checkUseUnitDefaults.EnableThis();
    checkEnabledByDefault.EnableThis();

    groupUnitAvailability.EnableThis();
    for ( int i=0; i<12; i++ )
    {
        textPlayerAvailability[i].EnableThis();
        dropPlayerAvailability[i].EnableThis();
    }
}

void UnitSettingsWindow::DisableUnitProperties()
{
    groupUnitProperties.DisableThis();
    textHitPoints.DisableThis();
    editHitPoints.DisableThis();
    editHitPointsByte.DisableThis();
    textShieldPoints.DisableThis();
    editShieldPoints.DisableThis();
    textArmor.DisableThis();
    editArmor.DisableThis();
    textBuildTime.DisableThis();
    editBuildTime.DisableThis();
    textMineralCost.DisableThis();
    editMineralCost.DisableThis();
    textGasCost.DisableThis();
    editGasCost.DisableThis();

    groupGroundWeapon.DisableThis();
    textGroundDamage.DisableThis();
    editGroundDamage.DisableThis();
    textGroundBonus.DisableThis();
    editGroundBonus.DisableThis();

    groupAirWeapon.DisableThis();
    textAirDamage.DisableThis();
    editAirDamage.DisableThis();
    textAirBonus.DisableThis();
    editAirBonus.DisableThis();

    groupUnitName.DisableThis();
    checkUseDefaultName.DisableThis();
    editUnitName.DisableThis();
}

void UnitSettingsWindow::EnableUnitProperties()
{
    groupUnitProperties.EnableThis();
    textHitPoints.EnableThis();
    editHitPoints.EnableThis();
    editHitPointsByte.EnableThis();
    textShieldPoints.EnableThis();
    editShieldPoints.EnableThis();
    textArmor.EnableThis();
    editArmor.EnableThis();
    textBuildTime.EnableThis();
    editBuildTime.EnableThis();
    textMineralCost.EnableThis();
    editMineralCost.EnableThis();
    textGasCost.EnableThis();
    editGasCost.EnableThis();

    groupGroundWeapon.EnableThis();
    textGroundDamage.EnableThis();
    editGroundDamage.EnableThis();
    textGroundBonus.EnableThis();
    editGroundBonus.EnableThis();

    groupAirWeapon.EnableThis();
    textAirDamage.EnableThis();
    editAirDamage.EnableThis();
    textAirBonus.EnableThis();
    editAirBonus.EnableThis();

    groupUnitName.EnableThis();
    checkUseDefaultName.EnableThis();

    if ( SendMessage((HWND)checkUseDefaultName.getHandle(), BM_GETCHECK, 0, 0) == BST_UNCHECKED )
        editUnitName.EnableThis();
}

void UnitSettingsWindow::CheckReplaceUnitName()
{
    if ( possibleUnitNameUpdate && checkUseDefaultName.isChecked() )
        possibleUnitNameUpdate = false;

    RawString rawUnitName;
    ChkdString newUnitName;
    if ( possibleUnitNameUpdate && selectedUnit >= 0 && selectedUnit < 228 && editUnitName.GetWinText(newUnitName) )
    {
        if ( CM->setUnitName(selectedUnit, newUnitName) )
        {
            CM->notifyChange(false);
            chkd.unitWindow.RepopulateList();
            RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
        }
        else
            RefreshWindow();

        possibleUnitNameUpdate = false;
    }
}

void UnitSettingsWindow::SetDefaultUnitProperties()
{
    refreshing = true;
    if ( selectedUnit >= 0 )
    {
        u16 unitId = (u16)selectedUnit;

        // Remove Custom Unit Name
        u16 origName = 0, expName = 0;
        CM->getUnisStringId(selectedUnit, origName);
        CM->getUnixStringId(selectedUnit, expName);
        CM->setUnisStringId(selectedUnit, 0);
        CM->setUnixStringId(selectedUnit, 0);
        ChkdString unitName;
        CM->getUnitName(unitName, (u16)selectedUnit);
        editUnitName.SetText(unitName.c_str());
        checkUseDefaultName.DisableThis();
        editUnitName.DisableThis();
        CM->removeUnusedString(origName);
        CM->removeUnusedString(expName);
        chkd.unitWindow.RepopulateList();
        RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);

        u32 groundWeapon = (u32)chkd.scData.UnitDat(unitId)->GroundWeapon,
            airWeapon    = (u32)chkd.scData.UnitDat(unitId)->AirWeapon;

        u16 subUnitId = chkd.scData.UnitDat(unitId)->Subunit1;
        if ( subUnitId != 228 ) // If unit has a subunit
        {
            if ( groundWeapon == 130 ) // If unit might have a subunit ground attack
                groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon;
            if ( airWeapon == 130 ) // If unit might have a subunit air attack
                airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon;
        }
        
        CM->setUnitSettingsCompleteHitpoints(unitId, chkd.scData.UnitDat(unitId)->HitPoints);
        CM->setUnitSettingsShieldPoints(unitId, chkd.scData.UnitDat(unitId)->ShieldAmount);
        CM->setUnitSettingsArmor(unitId, chkd.scData.UnitDat(unitId)->Armor);
        CM->setUnitSettingsBuildTime(unitId, chkd.scData.UnitDat(unitId)->BuildTime);
        CM->setUnitSettingsMineralCost(unitId, chkd.scData.UnitDat(unitId)->MineralCost);
        CM->setUnitSettingsGasCost(unitId, chkd.scData.UnitDat(unitId)->VespeneCost);

        if ( groundWeapon != 130 )
        {
            u16 defaultBaseDamage = chkd.scData.WeaponDat(groundWeapon)->DamageAmount,
                defaultBonusDamage = chkd.scData.WeaponDat(groundWeapon)->DamageBonus;

            CM->setUnitSettingsBaseWeapon(groundWeapon, defaultBaseDamage);
            CM->setUnitSettingsBonusWeapon(groundWeapon, defaultBonusDamage);
        }

        if ( airWeapon != 130 )
        {
            u16 defaultBaseDamage = chkd.scData.WeaponDat(airWeapon)->DamageAmount,
                defaultBonusDamage = chkd.scData.WeaponDat(airWeapon)->DamageBonus;

            CM->setUnitSettingsBaseWeapon(airWeapon, defaultBaseDamage);
            CM->setUnitSettingsBonusWeapon(airWeapon, defaultBonusDamage);
        }

        CM->notifyChange(false);
    }
    refreshing = false;
}

void UnitSettingsWindow::ClearDefaultUnitProperties()
{
    if ( selectedUnit >= 0 )
    {
        u16 unitId = (u16)selectedUnit;
        u32 groundWeapon = (u32)chkd.scData.UnitDat(unitId)->GroundWeapon,
            airWeapon    = (u32)chkd.scData.UnitDat(unitId)->AirWeapon;

        u16 origName = 0, expName = 0;
        CM->getUnisStringId(selectedUnit, origName);
        CM->getUnixStringId(selectedUnit, expName);
        CM->setUnisStringId(selectedUnit, 0);
        CM->setUnixStringId(selectedUnit, 0);
        CM->removeUnusedString(origName);
        CM->removeUnusedString(expName);

        CM->setUnitSettingsCompleteHitpoints(unitId, 0);
        CM->setUnitSettingsShieldPoints(unitId, 0);
        CM->setUnitSettingsArmor(unitId, 0);
        CM->setUnitSettingsBuildTime(unitId, 0);
        CM->setUnitSettingsMineralCost(unitId, 0);
        CM->setUnitSettingsGasCost(unitId, 0);
        CM->notifyChange(false);
    }
}

LRESULT UnitSettingsWindow::Notify(HWND hWnd, WPARAM idFrom, NMHDR* nmhdr)
{
    if ( nmhdr->code == TVN_SELCHANGED && ((NMTREEVIEW*)nmhdr)->action != TVC_UNKNOWN )
    {
        LPARAM itemType = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeTypePortion,
            itemData = (((NMTREEVIEW*)nmhdr)->itemNew.lParam)&TreeDataPortion;

        u16 unitId = (u16)itemData;
        if ( itemType == TreeTypeUnit && unitId < 228 )
        {
            CheckReplaceUnitName();
            selectedUnit = unitId;
            RefreshWindow();
        }
        else
        {
            CheckReplaceUnitName();
            selectedUnit = -1;
            DisableUnitEditing();
        }
    }
    return ClassWindow::Notify(hWnd, idFrom, nmhdr);
}

LRESULT UnitSettingsWindow::Command(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if ( refreshing )
        return ClassWindow::Command(hWnd, wParam, lParam);

    switch ( LOWORD(wParam) )
    {
    case BUTTON_RESETALLUNITDEFAULTS:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            if ( MessageBox(hWnd, "Are you sure you want to reset all unit settings?", "Confirm", MB_YESNO) == IDYES )
            {
                buffer newUNIS((u32)SectionId::UNIS), newUNIx((u32)SectionId::UNIx), newPUNI((u32)SectionId::PUNI);
                if ( Get_UNIS(newUNIS) )
                {
                    newUNIS.del(0, 8);
                    CM->ReplaceUNISSection(newUNIS);
                }
                if ( Get_UNIx(newUNIx) )
                {
                    newUNIx.del(0, 8);
                    CM->ReplaceUNIxSection(newUNIx);
                }
                if ( Get_PUNI(newPUNI) )
                {
                    newPUNI.del(0, 8);
                    CM->ReplacePUNISection(newPUNI);
                }

                CM->cleanStringTable(false);
                CM->cleanStringTable(true);
                DisableUnitEditing();
                RefreshWindow();
                CM->notifyChange(false);
            }
        }
        break;

    case CHECK_USEUNITDEFAULTS:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnit != -1 )
            {
                if ( state == BST_CHECKED )
                {
                    ClearDefaultUnitProperties();
                    DisableUnitProperties();
                    CM->setUnitUseDefaults((u8)selectedUnit, true);
                }
                else
                {
                    SetDefaultUnitProperties();
                    EnableUnitProperties();
                    CM->setUnitUseDefaults((u8)selectedUnit, false);
                }

                RefreshWindow();
                chkd.unitWindow.RepopulateList();
                RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
                CM->notifyChange(false);
            }
        }
        break;
    case CHECK_USEDEFAULTUNITNAME:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnit != -1 )
            {
                if ( state == BST_CHECKED )
                {
                    editUnitName.DisableThis();
                    CM->setUnisStringId(selectedUnit, 0);
                    CM->setUnixStringId(selectedUnit, 0);
                    ChkdString unitName;
                    CM->getUnitName(unitName, (u16)selectedUnit);
                    editUnitName.SetText(unitName.c_str());
                    chkd.unitWindow.RepopulateList();
                    RedrawWindow(chkd.unitWindow.getHandle(), NULL, NULL, RDW_INVALIDATE);
                }
                else
                    editUnitName.EnableThis();

                CM->notifyChange(false);
            }
        }
        break;
    case CHECK_ENABLEDBYDEFAULT:
        if ( HIWORD(wParam) == BN_CLICKED )
        {
            LRESULT state = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
            if ( selectedUnit != -1 )
            {
                CM->setUnitEnabled((u16)selectedUnit, state == BST_CHECKED);
                CM->notifyChange(false);
            }
        }
        break;
    case EDIT_UNITNAME:
        if ( HIWORD(wParam) == EN_CHANGE )
            possibleUnitNameUpdate = true;
        else if ( HIWORD(wParam) == EN_KILLFOCUS )
            CheckReplaceUnitName();
        break;
    case EDIT_UNITHITPOINTS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u32 newHitPoints;
            if ( editHitPoints.GetEditNum<u32>(newHitPoints) )
            {
                CM->setUnitSettingsHitpoints((u16)selectedUnit, newHitPoints);
                CM->notifyChange(false);
            }
        }
        break;
    case EDIT_UNITHITPOINTSBYTE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u8 newHitPointByte;
            if ( editHitPointsByte.GetEditNum<u8>(newHitPointByte) )
            {
                CM->setUnitSettingsHitpointByte((u16)selectedUnit, newHitPointByte);
                CM->notifyChange(false);
            }
        }
        break;
    case EDIT_UNITSHIELDPOINTS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newShieldPoints;
            if ( editShieldPoints.GetEditNum<u16>(newShieldPoints) )
            {
                CM->setUnitSettingsShieldPoints((u16)selectedUnit, newShieldPoints);
                CM->notifyChange(false);
            }
        }
        break;
    case EDIT_UNITARMOR:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u8 newArmorByte;
            if ( editArmor.GetEditNum<u8>(newArmorByte) )
            {
                CM->setUnitSettingsArmor((u16)selectedUnit, newArmorByte);
                CM->notifyChange(false);
            }
        }
        break;
    case EDIT_UNITBUILDTIME:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newBuildTime;
            if ( editBuildTime.GetEditNum<u16>(newBuildTime) )
            {
                CM->setUnitSettingsBuildTime((u16)selectedUnit, newBuildTime);
                CM->notifyChange(false);
            }
        }
        break;
    case EDIT_UNITMINERALCOST:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newMineralCost;
            if ( editMineralCost.GetEditNum<u16>(newMineralCost) )
            {
                CM->setUnitSettingsMineralCost((u16)selectedUnit, newMineralCost);
                CM->notifyChange(false);
            }
        }
        break;
    case EDIT_UNITGASCOST:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGasCost;
            if ( editGasCost.GetEditNum<u16>(newGasCost) )
            {
                CM->setUnitSettingsGasCost((u16)selectedUnit, newGasCost);
                CM->notifyChange(false);
            }
        }
        break;
    case EDIT_UNITGROUNDDAMAGE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGroundDamage;
            if ( editGroundDamage.GetEditNum<u16>(newGroundDamage) )
            {
                u32 groundWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnit)->GroundWeapon;
                u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnit)->Subunit1;

                if ( subUnitId != 228 && groundWeapon == 130 ) // If unit has a subunit
                    groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon; // If unit might have a subunit ground attack
                if ( groundWeapon < 130 )
                {
                    CM->setUnitSettingsBaseWeapon(groundWeapon, newGroundDamage);
                    CM->notifyChange(false);
                }
            }
        }
        break;
    case EDIT_UNITGROUNDBONUS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newGroundBonus;
            if ( editGroundBonus.GetEditNum<u16>(newGroundBonus) )
            {
                u32 groundWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnit)->GroundWeapon;
                u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnit)->Subunit1;

                if ( subUnitId != 228 && groundWeapon == 130 ) // If unit has a subunit
                    groundWeapon = chkd.scData.UnitDat(subUnitId)->GroundWeapon; // If unit might have a subunit ground attack
                if ( groundWeapon < 130 )
                {
                    CM->setUnitSettingsBonusWeapon(groundWeapon, newGroundBonus);
                    CM->notifyChange(false);
                }
            }
        }
        break;
    case EDIT_UNITAIRDAMAGE:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newAirDamage;
            if ( editAirDamage.GetEditNum<u16>(newAirDamage) )
            {
                u32 airWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnit)->AirWeapon;
                u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnit)->Subunit1;

                if ( subUnitId != 228 && airWeapon == 130 ) // If unit has a subunit
                    airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon; // If unit might have a subunit ground attack
                if ( airWeapon < 130 )
                {
                    CM->setUnitSettingsBaseWeapon(airWeapon, newAirDamage);
                    CM->notifyChange(false);
                }
            }
        }
        break;
    case EDIT_UNITAIRBONUS:
        if ( HIWORD(wParam) == EN_CHANGE )
        {
            u16 newAirBonus;
            if ( editAirBonus.GetEditNum<u16>(newAirBonus) )
            {
                u32 airWeapon = (u32)chkd.scData.UnitDat((u16)selectedUnit)->AirWeapon;
                u16 subUnitId = chkd.scData.UnitDat((u16)selectedUnit)->Subunit1;

                if ( subUnitId != 228 && airWeapon == 130 ) // If unit has a subunit
                    airWeapon = chkd.scData.UnitDat(subUnitId)->AirWeapon; // If unit might have a subunit ground attack

                CM->setUnitSettingsBonusWeapon(airWeapon, newAirBonus);
                CM->notifyChange(false);
            }
        }
        break;
    default:
        if ( LOWORD(wParam) >= DROP_P1UNITAVAILABILITY && LOWORD(wParam) <= DROP_P12UNITAVAILABILITY &&
            HIWORD(wParam) == CBN_SELCHANGE )
        {
            u32 player = LOWORD(wParam) - DROP_P1UNITAVAILABILITY;
            int sel = dropPlayerAvailability[player].GetSel();
            if ( CM->setUnitEnabledState((u16)selectedUnit, (u8)player, (UnitEnabledState)dropPlayerAvailability[player].GetSel()) )
                CM->notifyChange(false);
            else
                RefreshWindow();
        }
        break;
    }
    return ClassWindow::Command(hWnd, wParam, lParam);
}

LRESULT UnitSettingsWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_SHOWWINDOW:
            if ( wParam == TRUE )
            {
                RefreshWindow();
                if ( selectedUnit != -1 )
                    chkd.mapSettingsWindow.SetWinText((std::string("Map Settings - [") + DefaultUnitDisplayName[selectedUnit] + ']').c_str());
                else
                    chkd.mapSettingsWindow.SetWinText("Map Settings");
            }
            else
                CheckReplaceUnitName();
            break;

        default:
            return ClassWindow::WndProc(hWnd, msg, wParam, lParam);
            break;
    }
    return 0;
}
