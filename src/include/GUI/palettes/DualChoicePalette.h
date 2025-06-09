#pragma once

#include <glibmm/main.h>

#include <GUI/palettes/Palette.h>
#include <GUI/components/LabeledIconButton.h>

#include <language/Language.h>
#include <image/icons.h>

#include <extensions/GUIExtensions.h>
#include <GUI/lib/MainLoopHalter.h>

namespace Clavis::GUI {

    template <typename Derived>
    class DualChoicePalette : public Palette {
    public:
        DualChoicePalette(Glib::ustring title, const Gtk::Orientation orientation = Gtk::Orientation::VERTICAL)
            : Palette(title),
              mainVBox(Gtk::Orientation::VERTICAL),
              buttonsHBox(Gtk::Orientation::HORIZONTAL),
              contentBox(orientation)
        {
            buttonsHBox.append(noButton);
            buttonsHBox.append(yesButton);

            yesButton.set_hexpand(true);
            noButton.set_hexpand(true);
            contentBox.set_vexpand(true);
            buttonsHBox.set_valign(Gtk::Align::END);

            yesButton.set_size_request(-1, 30);
            noButton.set_size_request(-1, 30);
            yesButton.SetContentsHAlign(Gtk::Align::CENTER);
            noButton.SetContentsHAlign(Gtk::Align::CENTER);

            yesButton.SetLabel(_(MISC_YES_BUTTON));
            noButton.SetLabel(_(MISC_NO_BUTTON));
            yesButton.SetIcon(Icons::Check);
            noButton.SetIcon(Icons::Actions::Cancel);

            mainVBox.append(contentBox);
            mainVBox.append(buttonsHBox);
            set_child(mainVBox);

            set_resizable(false);

            yesButton.signal_clicked().connect([this]() {
                __DoGiveResponseImpl(true);
            });
            noButton.signal_clicked().connect([this]() {
                __DoGiveResponseImpl(false);
            });
            signal_hide().connect([this]() {
                __DoGiveResponseImpl(false);
            }, true);
        }

        void append(Widget& w) {
            contentBox.append(w);
        }

        void SetYesSuggested() {
            yesButton.add_css_class("suggested-action");
        }
        void SetNoSuggested() {
            noButton.add_css_class("suggested-action");
        }
        void SetYesDestructive() {
            yesButton.add_css_class("destructive-action");
        }
        void SetNoDestructive() {
            noButton.add_css_class("destructive-action");
        }
        void SetYesText(const std::string& text) {
            yesButton.SetLabel(text);
        }
        void SetNoText(const std::string& text) {
            noButton.SetLabel(text);
        }
        void set_margin(int margin) {
            contentBox.set_margin(margin);
        }

        static Derived* Create(Gtk::Widget* parent = nullptr, std::function<Derived*()> constructor = [](){return new Derived();}) {
            auto palette = Extensions::SpawnWindowNoSafeDelete<Derived>(constructor, parent);

            return palette;
        }

        bool Run(std::function<void(Derived*, bool)> callback = [](Derived*, bool){}) {
            halter.Halt();
            bool result = response;

            auto palette = (Derived*)this;
            callback(palette, result);

            delete(this);
            return result;
        }

        /// Static function that creates and returns the result from a modal dialog
        static bool Spawn(Gtk::Widget* parent = nullptr, std::function<void(Derived*, bool)> callback = [](Derived*, bool){}) {
            auto palette = Create(parent);

            return palette->Run(callback);
        }

        static bool Spawn(Gtk::Widget* parent = nullptr, std::function<Derived*()> constructor = [](){return new Derived*();}, std::function<void(Derived*, bool)> callback = [](Derived*, bool){}) {
            auto palette = Create(parent, constructor);

            return palette->Run(callback);
        }

    protected:
        virtual void DoGiveResponse(bool r) {
            response = r;
            isResponseGiven = true;

            close();

            halter.Resume();
        }

        bool isResponseGiven = false;
        bool response = false;

    private:
        void __DoGiveResponseImpl(bool r) {
            if (isResponseGiven)
                return;

            DoGiveResponse(r);
        }

        Gtk::Box mainVBox;
        Gtk::Box buttonsHBox;
        Gtk::Box contentBox;

        LabeledIconButton yesButton;
        LabeledIconButton noButton;

        MainLoopHalter halter;

    };

}
