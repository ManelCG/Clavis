#pragma once

#include <gtkmm/box.h>

#include <gtkmm/sizegroup.h>

namespace Clavis::GUI {
	class WidgetTable : public Gtk::Box {
	public:
		WidgetTable();
		WidgetTable(bool useSeparators);

		void AppendRow(std::shared_ptr<Gtk::Widget> w1, std::shared_ptr<Gtk::Widget> w2);

	protected:

	private:
		bool UseSeparators;

		std::vector<std::shared_ptr<Gtk::Widget>> Widgets1;
		std::vector<std::shared_ptr<Gtk::Widget>> Widgets2;

		Glib::RefPtr<Gtk::SizeGroup> SizeGroupLeft;
		Glib::RefPtr<Gtk::SizeGroup> SizeGroupRight;
	};
}