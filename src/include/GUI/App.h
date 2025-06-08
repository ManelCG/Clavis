#pragma once

#include <gtkmm.h>

namespace Clavis::GUI {
    class App {
        public:
            App();
            void Run(int argc, char* argv[]);
            bool FirstRun(int argc, char* argv[]);

        protected:
        private:
        Glib::RefPtr<Gtk::Application> InstantiateApp();
    };
}