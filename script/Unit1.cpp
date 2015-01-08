////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2001 mamaich
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////

#include <vcl.h>
#include <windows.h>
#pragma hdrstop

#include "UnitFrm.h"

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
    return 1;
}
//---------------------------------------------------------------------------
HWND Tab;
DllInterface *UO=0;

extern "C" void __export __cdecl Init(DllInterface *intf)
{
    if(intf->Size!=sizeof(DllInterface))
        MessageBox(0,"Incompatible version of script.dll and injection.dll!\nThe program may crash","Error",MB_OK|MB_ICONSTOP);
    UO=intf;
    Tab=intf->Window;
    MainForm=new TMainForm(0);
    MainForm->ParentWindow=Tab;
    MainForm->Visible=true;
    RECT R;
    GetClientRect(Tab,&R);
    MainForm->Left=0; MainForm->Top=0;
    MainForm->Height=R.right; MainForm->Width=R.bottom;
}

extern "C" void __export __cdecl Cleanup()
{
    if(MainForm)
    {
        delete MainForm;
    }
    MainForm=0;
}

