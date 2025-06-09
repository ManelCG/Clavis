#include <iostream>
#include <GUI/App.h>

#include <settings/Settings.h>
#include <password_store/PasswordStore.h>

int main(int argc, char* argv[])
{
	Clavis::Settings::SettingsInitialize();

	Clavis::GUI::App().Run(argc, argv);

	return 0;
}
