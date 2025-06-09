#include <GUI/Tables/WidgetTable.h>

#include <gtkmm/separator.h>
#include <gtkmm/scrolledwindow.h>

namespace Clavis::GUI {
	WidgetTable::WidgetTable() : WidgetTable(true) {}

	WidgetTable::WidgetTable(bool useSep)
	{
		UseSeparators = useSep;

		set_orientation(Gtk::Orientation::VERTICAL);

		Gtk::Separator sep(Gtk::Orientation::HORIZONTAL);
		append(sep);

		SizeGroupLeft = Gtk::SizeGroup::create(Gtk::SizeGroup::Mode::HORIZONTAL);
		SizeGroupRight = Gtk::SizeGroup::create(Gtk::SizeGroup::Mode::HORIZONTAL);
	}

	void WidgetTable::AppendRow(std::shared_ptr<Gtk::Widget> w1, std::shared_ptr<Gtk::Widget> w2) {
		constexpr int sepmargin = 10;

		Gtk::Separator sepv0(Gtk::Orientation::VERTICAL);
		sepv0.set_margin_end(sepmargin);

		Gtk::Separator sepv1(Gtk::Orientation::VERTICAL);
		sepv1.set_margin_start(sepmargin);
		sepv1.set_margin_end(sepmargin);

		Gtk::Separator sepv2(Gtk::Orientation::VERTICAL);
		sepv2.set_margin_start(sepmargin);

		Gtk::ScrolledWindow scroll1;
		scroll1.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::NEVER);
		scroll1.set_size_request(80);
		scroll1.set_hexpand(false);
		scroll1.set_child(*w1);

		Gtk::ScrolledWindow scroll2;
		scroll2.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::NEVER);
		scroll2.set_size_request(140);
		scroll2.set_child(*w2);
		scroll2.set_hexpand(true);

		Gtk::Box databox(Gtk::Orientation::HORIZONTAL);
		databox.append(sepv0);
		databox.append(scroll1);
		databox.append(sepv1);
		databox.append(scroll2);
		databox.append(sepv2);

		append(databox);

		Gtk::Separator seph(Gtk::Orientation::HORIZONTAL);
		append(seph);

		Widgets1.push_back(w1);
		Widgets2.push_back(w2);

		SizeGroupLeft->add_widget(*w1);
		SizeGroupRight->add_widget(*w2);
	}
}