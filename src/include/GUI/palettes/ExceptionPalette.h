#pragma once

#include <GUI/palettes/Palette.h>
#include <GUI/Toolbar.h>
#include <GUI/Tables/StringTable.h>
#include <error/ClavisError.h>

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>

#include <vector>
#include <memory>

using ExceptionData = Clavis::Error::ClavisException::ClavisExceptionData;

namespace Clavis::GUI {
	class ExceptionPalette : public Palette {
	public:
		ExceptionPalette(ExceptionData data);

	protected:

	private:
		ExceptionData Data;

		Gtk::Button CloseButton;
		Gtk::Button CopyErrorInfoButton;

		Gtk::Box MainVBox;
		StringTable BuildDataTable;
		Toolbar Footer;

		std::string FormatExceptionDetails();
	};
}
