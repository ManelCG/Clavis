#include <iostream>
#include <string>
#include <GUI/App.h>

#include <crypto/SelfTest.h>
#include <settings/Settings.h>
#include <system/Extensions.h>
#include <password_store/PasswordStore.h>

int main(int argc, char* argv[])
{
	// The OTP stack is validated against the published RFC vectors rather than a test framework,
	// which the project does not have. Runs before anything touches GTK or the settings file.
	if (argc > 1 && std::string(argv[1]) == "--selftest-crypto")
		return Clavis::Crypto::RunSelfTest() ? 0 : 1;

	Clavis::System::ChDir(Clavis::System::GetExecutableLocation());

	Clavis::Settings::SettingsInitialize();

	Clavis::GUI::App().Run(argc, argv);

	return 0;
}
