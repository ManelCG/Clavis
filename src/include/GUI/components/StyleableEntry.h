#pragma once

#include <gtkmm.h>
#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>
#include <string_view>
#include <mutex>

namespace Clavis::GUI {
    class StyleableEntry : public Gtk::Entry {
    public:
        StyleableEntry();

        void DisplayError();
        void DisplaySuccess();

        void SetStyle(const std::string_view& style);
        void RemoveStyle();

    protected:

    private:
        static constexpr std::string_view errorCssClass = "error";
        static constexpr std::string_view successCssClass = "success";

        std::string_view currentStyle = "";
        UniqueSignalTimeoutDispatcher timeoutDispatcher;
        std::mutex mutex;

        void __SetStyleUnsafe(const std::string_view& style);
        void __RemoveStyleUnsafe();
    };
}