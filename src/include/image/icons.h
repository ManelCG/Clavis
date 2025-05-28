#pragma once

#include <string>

namespace Clavis::Icons {
	class IconDefinition {
	public:
		IconDefinition() : IconDefinition("Placeholders/16x16.png", false){}
		explicit constexpr IconDefinition(const char* c, bool inv = true) : Name(c), Inversible(inv) {}
		std::string_view Name;
		bool Inversible;
	};

	constexpr IconDefinition Minus = IconDefinition("MinusIcon.png");
	constexpr IconDefinition Plus = IconDefinition("PlusIcon.png");
	constexpr IconDefinition UpCaret = IconDefinition("UpCaretIcon.png");
	constexpr IconDefinition UpArrow = IconDefinition("UpArrowIcon.png");
	constexpr IconDefinition Check = IconDefinition("Check.png");
	constexpr IconDefinition Cross = IconDefinition("Cross.png");

	namespace WindowControls {
		constexpr IconDefinition CloseWindow = IconDefinition("WindowControls/WindowCloseIcon.png");
		constexpr IconDefinition MaximizeWindow = IconDefinition("WindowControls/WindowMaximizeIcon.png");
		constexpr IconDefinition UnMaximizeWindow = IconDefinition("WindowControls/WindowUnmaximizeIcon.png");
		constexpr IconDefinition MinimizeWindow = IconDefinition("WindowControls/WindowMinimizeIcon.png");
	}

	namespace Logos {
		constexpr IconDefinition ClavisLogo = IconDefinition("../logo.png", false);
		constexpr IconDefinition Logo32px = IconDefinition("Logos/32px.png", false);
		constexpr IconDefinition Logo16px = IconDefinition("Logos/16px.png", false);
	}

	namespace Actions {
		constexpr IconDefinition Document = IconDefinition("Actions/Document.png");
		constexpr IconDefinition NewDocument = IconDefinition("Actions/NewDocument.png");
		constexpr IconDefinition Folder = IconDefinition("Actions/Folder.png");
		constexpr IconDefinition NewFolder = IconDefinition("Actions/NewFolder.png");

		constexpr IconDefinition Password = IconDefinition("Actions/Password.png");

		constexpr IconDefinition SettingsCog = IconDefinition("Actions/SettingsCog.png");
		constexpr IconDefinition SettingsSliders = IconDefinition("Actions/SettingsSliders.png");

		constexpr IconDefinition FolderUp = IconDefinition("Actions/FolderUp.png");
		constexpr IconDefinition FolderDown = IconDefinition("Actions/FolderDown.png");
		constexpr IconDefinition FolderLeft = IconDefinition("Actions/FolderLeft.png");
		constexpr IconDefinition FolderRight = IconDefinition("Actions/FolderRight.png");

		constexpr IconDefinition Hidden = IconDefinition("Actions/Hidden.png");
		constexpr IconDefinition Visible = IconDefinition("Actions/Visible.png");

		constexpr IconDefinition Copy = IconDefinition("Actions/Copy.png");
		constexpr IconDefinition Save = IconDefinition("Actions/Save.png");
		constexpr IconDefinition SaveAll = IconDefinition("Actions/SaveAll.png");

		constexpr IconDefinition Export = IconDefinition("Actions/Export.png");
		constexpr IconDefinition Import = IconDefinition("Actions/Import.png");

		constexpr IconDefinition Draw = IconDefinition("Actions/Draw.png");
		constexpr IconDefinition Build = IconDefinition("Actions/Build.png");

		constexpr IconDefinition NewWindow = IconDefinition("Actions/NewWindow.png");
		constexpr IconDefinition Refresh = IconDefinition("Actions/Refresh.png");

		constexpr IconDefinition Cancel = IconDefinition("Actions/Cancel.png", false);
		constexpr IconDefinition Trash = IconDefinition("Actions/Trash.png", false);

		constexpr IconDefinition Render = IconDefinition("Actions/Render.png", false);
	}

	namespace Type {
		constexpr IconDefinition CollapseArrowDown = IconDefinition("Type/CollapseArrowDown.png");
		constexpr IconDefinition CollapseArrowRight = IconDefinition("Type/CollapseArrowRight.png");
	}

	namespace Git {
		constexpr IconDefinition Git = IconDefinition("Git/Git.png");
		constexpr IconDefinition Push = IconDefinition("Git/Push.png");
		constexpr IconDefinition Pull = IconDefinition("Git/Pull.png");
		constexpr IconDefinition Sync = IconDefinition("Git/Sync.png");
	}

	namespace MediaControls {
		constexpr IconDefinition Play = IconDefinition("MediaControls/Play.png");
		constexpr IconDefinition Pause = IconDefinition("MediaControls/Pause.png");
		constexpr IconDefinition Stop = IconDefinition("MediaControls/Stop.png");
	}

	namespace Currencies {
		constexpr IconDefinition XMR = IconDefinition("Currencies/XMR.png", false);
		constexpr IconDefinition BTC = IconDefinition("Currencies/BTC.png", false);

		constexpr IconDefinition XMR_32x32 = IconDefinition("Currencies/XMR_32px.png", false);
		constexpr IconDefinition BTC_32x32 = IconDefinition("Currencies/BTC_32px.png", false);

		constexpr IconDefinition XMR_QR = IconDefinition("Currencies/XMR_QR.png", false);
		constexpr IconDefinition BTC_QR = IconDefinition("Currencies/BTC_QR.png", false);
	}

	namespace Placeholders {
		constexpr IconDefinition Placeholder32x32 = IconDefinition("Placeholders/32x32.png");
		constexpr IconDefinition Placeholder16x16 = IconDefinition("Placeholders/16x16.png");
		constexpr IconDefinition Placeholder8x8 = IconDefinition("Placeholders/8x8.png");
		constexpr IconDefinition Placeholder32x32sub16x16 = IconDefinition("Placeholders/32x32sub16x16.png");
		constexpr IconDefinition Placeholder16x16sub8x8 = IconDefinition("Placeholders/16x16sub8x8.png");
	}
}