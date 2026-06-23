#include <iostream>
#include <GUI/App.h>

#include <settings/Settings.h>
#include <system/Extensions.h>
#include <password_store/PasswordStore.h>

int main(int argc, char* argv[])
{
	Clavis::System::ChDir(Clavis::System::GetExecutableLocation());

	Clavis::Settings::SettingsInitialize();

	Clavis::GUI::App().Run(argc, argv);

	return 0;
}
