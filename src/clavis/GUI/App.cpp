#include <GUI/App.h>

#include <settings/Settings.h>

#include <gtkmm/settings.h>

#include <GUI/MainWindow.h>
#include <GUI/workflows/NewItemWorkflow.h>

namespace Clavis::GUI {
    App::App()
    {
    }

    void App::Run(int argc, char* argv[])
    {
        if (Settings::IS_FIRST_RUN.GetValue())
            if (!FirstRun(argc, argv))
                return;

        auto app = InstantiateApp();

        app->make_window_and_run<MainWindow>(0, nullptr);
    }

    bool App::FirstRun(int argc, char *argv[]) {
        auto app = InstantiateApp(); // This returns a Glib::RefPtr<App> or your App subclass

        bool success = false;

        app->signal_startup().connect([app, &success]() {
            success = Workflows::FirstRunWorkflow(app);
        });

        app->run(argc, argv); // launch main loop

        return success;
    }

    Glib::RefPtr<Gtk::Application> App::InstantiateApp() {
        if (Settings::WINDOW_DECORATIONS.GetValue() == Settings::WINDOW_DECORATIONS.FORCE_NO_CSD)
            g_setenv("GTK_CSD", "0", false);

#ifdef __DEBUG__
        if (Settings::RUN_GTK_CSS_INSPECTOR.GetValue())
            g_setenv("GTK_DEBUG", "interactive", true);
#endif
        auto app = Gtk::Application::create("com.Clavis");

        auto settings = Gtk::Settings::get_default();

#ifdef __WINDOWS__
        settings->property_gtk_decoration_layout().set_value("menu:minimize,maximize,close");
        settings->property_gtk_xft_antialias().set_value(1);

        auto theme = Settings::CLAVIS_THEME.GetValue();
        auto font = Settings::CLAVIS_FONT.GetValue();

        settings->property_gtk_font_name().set_value(font.c_str());
        settings->property_gtk_theme_name().set_value(theme.c_str());
#endif

        settings->property_gtk_application_prefer_dark_theme().set_value(Settings::DO_USE_DARK_THEME.GetValue());

        return app;
    }
}

