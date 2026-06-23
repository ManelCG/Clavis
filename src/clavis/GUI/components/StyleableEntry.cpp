#include <GUI/components/StyleableEntry.h>

namespace Clavis::GUI {
    StyleableEntry::StyleableEntry() {
        timeoutDispatcher.SetMilliseconds(3000);
        timeoutDispatcher.SetAction([this]() {
            std::lock_guard lock(mutex);
            __RemoveStyleUnsafe();
        });
    }

    void StyleableEntry::SetStyle(const std::string_view &style) {
        std::lock_guard lock(mutex);
        __RemoveStyleUnsafe();
        __SetStyleUnsafe(style);
    }

    void StyleableEntry::RemoveStyle() {
        std::lock_guard lock(mutex);
        __RemoveStyleUnsafe();
    }

    void StyleableEntry::DisplayError() {
        SetStyle(errorCssClass);
    }

    void StyleableEntry::DisplaySuccess() {
        SetStyle(successCssClass);
    }

    void StyleableEntry::__SetStyleUnsafe(const std::string_view &style) {
        add_css_class(std::string(style));
        currentStyle = style;
        timeoutDispatcher.StartTimeout();
    }

    void StyleableEntry::__RemoveStyleUnsafe() {
        if (currentStyle.empty())
            return;

        remove_css_class(std::string(currentStyle));
        currentStyle = "";
    }
}