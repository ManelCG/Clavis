#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <giomm/menu.h>
#include <glibmm/ustring.h>

#include <gtkmm/popovermenubar.h>
#include <gtkmm/window.h>
#include <gtkmm/cssprovider.h>

namespace Clavis::GUI::MenuBar {
	class _MenuBarDefinition;
	class _MenuBar;

	typedef std::shared_ptr<_MenuBarDefinition> MenuBarDefinition;
	typedef std::shared_ptr<_MenuBar> MenuBar;

	class _MenuBar : public Gtk::PopoverMenuBar {
	public:
		_MenuBar(Glib::RefPtr<Gio::Menu> gmenu);
	protected:

	private:
		void ApplyCss();

		Glib::RefPtr<Gtk::CssProvider> Css;
	};




	class _MenuElement {
	public:
		typedef enum MenuElementType {
			MET_ITEM,
			MET_SECTION,
			MET_SUBMENU,
		} MenuElementType;

		_MenuElement(MenuElementType type);

		virtual Glib::ustring ToXML(Glib::ustring actionsNamespace) = 0;

	protected:

	private:
		MenuElementType Type;
	};

	typedef std::shared_ptr<_MenuElement> MenuElement;


	class _MenuItem: public _MenuElement {
	public:
		_MenuItem(Glib::ustring label, Glib::ustring action, Glib::ustring accel = "");

		Glib::ustring ToXML(Glib::ustring actionsNamespace) override;

	protected:

	private:
		Glib::ustring Label;
		Glib::ustring Action;
		Glib::ustring Accel;
	};

	class _MenuSection : public _MenuElement {
	public:
		_MenuSection(std::vector<MenuElement> elements);

		Glib::ustring ToXML(Glib::ustring actionsNamespace) override;
	protected:

	private:
		std::vector<MenuElement> Elements;
	};


	class _MenuSubmenu : public _MenuElement {
	public:
		_MenuSubmenu(Glib::ustring label, std::vector<MenuElement> elements);

		Glib::ustring ToXML(Glib::ustring actionsNamespace) override;
	protected:

	private:
		Glib::ustring Label;
		std::vector<MenuElement> Elements;
	};

	class _MenuBarDefinition {
	public:
		_MenuBarDefinition(Glib::ustring id, Glib::ustring actionsNamespace, std::vector<std::shared_ptr<_MenuSubmenu>> submenus);

		void AddAction(Glib::ustring id, std::function<void(void)> action);

		MenuBar BuildMenubar(Gtk::Window& host);

	protected:

	private:
		Glib::ustring ToXML();

		Glib::ustring Id;
		Glib::ustring ActionsNamespace;
		std::vector<std::shared_ptr<_MenuSubmenu>> Submenus;
		std::vector<std::pair<Glib::ustring, std::function<void(void)>>> Actions;
	};

	typedef std::shared_ptr<_MenuBarDefinition> MenuBarDefinition;

	inline std::shared_ptr<_MenuItem> MenuItem(Glib::ustring label, Glib::ustring action, Glib::ustring accel = "") { return std::make_shared<_MenuItem>(label, action, accel); }
	inline std::shared_ptr<_MenuSection> MenuSection(std::vector<MenuElement> elements) { return std::make_shared<_MenuSection>(elements); }
	inline std::shared_ptr<_MenuSubmenu> MenuSubmenu(Glib::ustring label, std::vector<MenuElement> elements) { return std::make_shared<_MenuSubmenu>(label, elements); }
	inline MenuBarDefinition MakeMenuBarDefinition(Glib::ustring id, Glib::ustring actionsNamespace, std::vector<std::shared_ptr<_MenuSubmenu>> submenus) { return std::make_shared<_MenuBarDefinition>(id, actionsNamespace, submenus); }

}