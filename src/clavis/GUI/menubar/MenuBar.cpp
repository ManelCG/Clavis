#include <GUI/menubar/MenuBar.h>

#include <extensions/GUIExtensions.h>
#include <settings/Settings.h>

#include <gtkmm.h>
#include <gdkmm.h>

namespace Clavis::GUI::MenuBar {
	_MenuElement::_MenuElement(MenuElementType type) {
		Type = type;
	}

	_MenuItem::_MenuItem(Glib::ustring label, Glib::ustring action, Glib::ustring accel) :
		_MenuElement(MET_ITEM)
	{
		Label = label;
		Action = action;
		Accel = accel;
	}

	Glib::ustring _MenuItem::ToXML(Glib::ustring actionsNamespace)
	{
//	        <item>
//	          <attribute name='label' translatable='yes'>Label</attribute>
//	          <attribute name='action'>namespace.action</attribute>
//	          <attribute name='accel'>&lt;Primary&gt;s</attribute>
//	        </item>

		Glib::ustring ret = "";

		ret += "<item>";
		ret += "<attribute name='label' translatable='no'>" + Label + "</attribute>";
		ret += "<attribute name='action'>" + actionsNamespace + "." + Action + "</attribute>";
		ret += "<attribute name='accel'>" + Accel + "</attribute>";
		ret += "</item>";


		return ret;
	}

	_MenuSection::_MenuSection(std::vector<MenuElement> elements) :
		_MenuElement(MET_SECTION)
	{
		Elements = elements;
	}

	Glib::ustring _MenuSection::ToXML(Glib::ustring actionsNamespace) {
		//	      <section>
		//	        <item>...</item>
		//	        <item>...</item>
		//	      </section>

		Glib::ustring ret = "";

		ret += "<section>";
		for (auto elem : Elements)
			ret += elem->ToXML(actionsNamespace);

		ret += "</section>";

		return ret;
	}

	_MenuSubmenu::_MenuSubmenu(Glib::ustring label, std::vector<MenuElement> elements) :
		_MenuElement(MET_SUBMENU)
	{
		Label = label;
		Elements = elements;
	}

	Glib::ustring _MenuSubmenu::ToXML(Glib::ustring actionsNamespace) {
	//	    <submenu>
	//	      <attribute name='label' translatable='no'>Label</attribute>
	//	      <section> ...</section>
	//	      <section> ...</section>
	//	      <section> ...</section>
	//	    </submenu>
		Glib::ustring ret = "";

		ret += "<submenu>";
		ret += "<attribute name='label' translatable='no'>" + Label + "</attribute>";

		for (auto& elem : Elements)
			ret += elem->ToXML(actionsNamespace);

		ret += "</submenu>";

		return ret;
	}

	_MenuBarDefinition::_MenuBarDefinition(Glib::ustring id, Glib::ustring actionsNamespace, std::vector<std::shared_ptr<_MenuSubmenu>> submenus) {
		Id = id;
		ActionsNamespace = actionsNamespace;
		Submenus = submenus;
	}

	Glib::ustring _MenuBarDefinition::ToXML() {
		//	<interface>
		//	  <menu id='id'>
		//	    <submenu> ... </submenu>
		//	    <submenu> ... </submenu>
		//	  </menu>
		//	</interface>

		Glib::ustring ret = "";

		ret += "<interface>";
		ret += "<menu id='" + Id + "'>";

		for (auto& menu : Submenus)
			ret += menu->ToXML(ActionsNamespace);

		ret += "</menu>";
		ret += "</interface>";

		return ret;
	}

	void _MenuBarDefinition::AddAction(Glib::ustring id, std::function<void(void)> action) {
		Actions.push_back({ id, action });
	}

	MenuBar _MenuBarDefinition::BuildMenubar(Gtk::Window& host) {
		auto m_refBuilder = Gtk::Builder::create();

		auto action_group = Gio::SimpleActionGroup::create();

		for (auto& kvp : Actions)
			action_group->add_action(kvp.first, kvp.second);

		host.insert_action_group(ActionsNamespace, action_group);

		auto ui_info = ToXML();
		m_refBuilder->add_from_string(ui_info);

		auto gmenu = m_refBuilder->get_object<Gio::Menu>(Id);
		auto pMenuBar = std::make_shared<_MenuBar>(gmenu);

		return pMenuBar;
	}

	_MenuBar::_MenuBar(Glib::RefPtr<Gio::Menu> gmenu) : Gtk::PopoverMenuBar(gmenu) {
		if (Settings::FORCE_CLAVIS_STYLE.GetValue())
			ApplyCss();
	}

	void _MenuBar::ApplyCss() {
		auto c = Extensions::GetAllChildren(this, true);
		auto menus = Extensions::FilterWidgets<Gtk::PopoverMenu>(c);

		for (auto menu : menus) {
			auto contents = menu->get_last_child();

			std::string css = __STRINGIZE__(
				* {
					border-radius: 0;
					padding: 0;
					margin: 0;
					min-height: 0;
					border-color: #282828;
					border-width: 1px;
			);
			if (Settings::DISABLE_SHADOWS.GetValue())
				css += __STRINGIZE__(
					box-shadow: none;
				);

			css += "}";
			Extensions::ApplyCss(contents, css);

			Extensions::ApplyCss(c, __STRINGIZE__(
				modelbutton {
					border-radius: 0;
					min-height: 0;
				}

				separator{
					background: #ff0000;
				}
			));

			auto labels = Extensions::FilterWidgets<Gtk::Label>(Extensions::GetAllChildren(contents, true));
			for (auto label : labels)
				label->set_margin(2);

			auto images = Extensions::FilterWidgets<Gtk::Image>(c);
			for (auto im : images)
				im->hide();

			//auto separators = Extensions::FilterWidgets<Gtk::Separator>(Extensions::GetAllChildren(contents, true));
			//for (auto separator : separators) {
			//	separator->set_margin_top(1);
			//	separator->set_margin_bottom(1);
			//}
		}
	}

}
