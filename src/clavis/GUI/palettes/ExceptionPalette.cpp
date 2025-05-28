#include <GUI/palettes/ExceptionPalette.h>

#include <system/Extensions.h>
#include <gtkmm/separator.h>

#include <regex>

#include <language/Language.h>

using ExceptionData = Clavis::Error::ClavisException::ClavisExceptionData;

namespace Clavis::GUI {
	ExceptionPalette::ExceptionPalette(ExceptionData exceptionData) :
		Palette(_(UNEXPECTED_ERROR_MESSAGE)),

		MainVBox(Gtk::Orientation::VERTICAL)
	{
		constexpr int margin = 10;

		Data = exceptionData;
		set_child(MainVBox);


		Gtk::Label header;
		header.set_markup("<big><b>" + _(CLAVIS_RAN_INTO_AN_ERROR_MESSAGE) + "</b></big>");
		MainVBox.append(header);
		header.set_margin_top(margin);

		Gtk::Label errorLabel;
		errorLabel.set_markup(Data.exceptionText);
		MainVBox.append(errorLabel);
		errorLabel.set_margin_start(margin);
		errorLabel.set_margin_end(margin);
		errorLabel.set_margin_top(margin*3);
		errorLabel.set_halign(Gtk::Align::CENTER);

		Gtk::Label extraInfo(_(PLEASE_TRY_AGAIN_ERROR_MESSAGE));
		MainVBox.append(extraInfo);
		extraInfo.set_margin_start(margin);
		extraInfo.set_margin_end(margin);
		extraInfo.set_margin_top(margin * 3);
		extraInfo.set_margin_bottom(margin * 3);
		extraInfo.set_halign(Gtk::Align::CENTER);

		BuildDataTable.set_margin_start(margin);
		BuildDataTable.set_margin_end(margin);
		BuildDataTable.set_vexpand(true);
		BuildDataTable.set_valign(Gtk::Align::END);

		BuildDataTable.AppendRow("Version", Data.version);
		BuildDataTable.AppendRow("Platform", Data.platform);
		BuildDataTable.AppendRow("Build Mode", Data.buildMode);
		BuildDataTable.AppendRow("Build Date", Data.buildDate);
		BuildDataTable.AppendRow("File", Data.file);
		BuildDataTable.AppendRow("Line", std::to_string(Data.line));
		BuildDataTable.AppendRow("Function", Data.function);
		MainVBox.append(BuildDataTable);



		CopyErrorInfoButton.set_label(_(COPY_ERROR_INFORMATION_BUTTON_LABEL));
		CopyErrorInfoButton.set_margin(margin);
		Footer.AppendBegin(CopyErrorInfoButton);

		CloseButton.set_label(_(MISC_CLOSE_BUTTON));
		CloseButton.set_margin(margin);
		Footer.AppendEnd(CloseButton);

		MainVBox.append(Footer);

		signal_hide().connect([this]() {
			this->destroy();
			delete(this);
		});

		CopyErrorInfoButton.signal_clicked().connect([this]() {
			if (!System::CopyToClipboard(FormatExceptionDetails()))
				RaiseClavisError(_(ERROR_COPYING_TO_CLIPBOARD_TEXT));
		});

		CloseButton.signal_clicked().connect([this]() {
			close();
		});
	}

	std::string ExceptionPalette::FormatExceptionDetails() {
		std::string ret = Data.message;

		// ret += "Error message: " + Data.message + "\n";
		// ret += "Version: " + Data.version + "\n";
		// ret += "Platform: " + Data.platform + "\n";
		// ret += "Build Mode: " + Data.buildMode + "\n";
		// ret += "Build Date: " + Data.buildDate + "\n";
		// ret += "File: " + Data.file + "\n";
		// ret += "Line: " + std::to_string(Data.line) + "\n";
		// ret += "Function: " + Data.function + "\n";

		ret = std::regex_replace(ret, std::regex("<.*?>"), "");

		return ret;
	}
}