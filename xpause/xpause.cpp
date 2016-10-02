#include <windows.h>
#include "pluginsdk\_plugins.h"

#ifndef DLL_EXPORT
#define DLL_EXPORT extern "C" __declspec(dllexport)
#endif //DLL_EXPORT

#define plugin_name "X-Pause"
#define plugin_version 1

int pluginHandle;
HWND hwndDlg;
int hMenu;
int hMenuDisasm;
int hMenuDump;
int hMenuStack;

bool cbXPause(int argc, char* argv[])
{
    DbgCmdExecDirect("createthread DebugBreak");
    DbgCmdExecDirect("mov $XPause_ThreadId, $result");
    return true;
}

void cbCreateThread(CBTYPE cbType, void* info1)
{
    PLUG_CB_CREATETHREAD* info = reinterpret_cast<PLUG_CB_CREATETHREAD*>(info1);
    if(info->dwThreadId == DbgValFromString("$XPause_ThreadId"))
    {
        DbgCmdExec("killthread $XPause_ThreadId");
        _plugin_logputs("Paused!");
    }
}

void cbMenuEntry(CBTYPE cbType, void* info1)
{
    PLUG_CB_MENUENTRY* info = reinterpret_cast<PLUG_CB_MENUENTRY*>(info1);
    if(info->hEntry == 0)
        DbgCmdExec("xpause");
}

DLL_EXPORT bool pluginit(PLUG_INITSTRUCT* initStruct)
{
    initStruct->pluginVersion = plugin_version;
    initStruct->sdkVersion = PLUG_SDKVERSION;
    strcpy_s(initStruct->pluginName, plugin_name);
    pluginHandle = initStruct->pluginHandle;
    return true;
}

DLL_EXPORT bool plugstop()
{
    return true;
}

DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    hwndDlg = setupStruct->hwndDlg;
    hMenu = setupStruct->hMenu;
    hMenuDisasm = setupStruct->hMenuDisasm;
    hMenuDump = setupStruct->hMenuDump;
    hMenuStack = setupStruct->hMenuStack;
    _plugin_registercallback(pluginHandle, CB_CREATETHREAD, cbCreateThread);
    _plugin_registercommand(pluginHandle, "xpause", cbXPause, true);
    _plugin_registercallback(pluginHandle, CB_MENUENTRY, cbMenuEntry);
    _plugin_menuaddentry(hMenu, 0, "XPause");
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    return TRUE;
}
