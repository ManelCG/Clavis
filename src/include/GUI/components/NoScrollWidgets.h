#pragma once

#include <gtkmm.h>

namespace Clavis::GUI {

    namespace {
        inline void removeBuiltinScrollController(Gtk::Widget& widget) {
            GtkWidget* w = widget.gobj();
            GListModel* controllers = gtk_widget_observe_controllers(w);
            guint n = g_list_model_get_n_items(controllers);
            for (guint i = 0; i < n; i++) {
                gpointer obj = g_list_model_get_item(controllers, i);
                if (GTK_IS_EVENT_CONTROLLER_SCROLL(obj)) {
                    gtk_widget_remove_controller(w, GTK_EVENT_CONTROLLER(obj));
                    g_object_unref(obj);
                    break;
                }
                g_object_unref(obj);
            }
            g_object_unref(controllers);
        }
    }

    class NoScrollSpinButton : public Gtk::SpinButton {
    public:
        NoScrollSpinButton(Glib::RefPtr<Gtk::Adjustment> adj, double climb_rate, guint digits)
            : Gtk::SpinButton(adj, climb_rate, digits) { removeBuiltinScrollController(*this); }
    };

    class NoScrollComboBoxText : public Gtk::ComboBoxText {
    public:
        NoScrollComboBoxText() { removeBuiltinScrollController(*this); }
    };

}
