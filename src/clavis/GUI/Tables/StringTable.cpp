#include <GUI/Tables/StringTable.h>

#include <gtkmm/label.h>

namespace Clavis::GUI {
	void StringTable::AppendRow(std::string s1, std::string s2) {
		auto l1 = std::make_shared<Gtk::Label>(s1);
		l1->set_halign(Gtk::Align::END);
		//l1->set_hexpand(true);

		auto l2 = std::make_shared<Gtk::Label>();
		l2->set_markup(s2);
		l2->set_halign(Gtk::Align::START);

		WidgetTable::AppendRow(l1, l2);
	}

}