// M2ModRedux.cpp : main project file.

#include <M2.h>
#include "Form1.h"

using namespace M2ModRedux;

[STAThreadAttribute]
int __stdcall WinMain(void *var1, unsigned long var2, void *var3)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	// Create the main window and run it
	Application::Run(gcnew Form1());
	return 0;
}
