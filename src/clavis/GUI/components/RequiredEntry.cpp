#include <GUI/components/RequiredEntry.h>

namespace Clavis::GUI {
    RequiredEntry::RequiredEntry() {
        timeoutDispatcher.SetMilliseconds(3000);
        timeoutDispatcher.SetAction([this]() {
            remove_css_class("error");
        });

        isTextValid = [this]() {
            if (get_text().empty())
                return false;

            return true;
        };
    }
    void RequiredEntry::DisplayError() {
        add_css_class("error");
        timeoutDispatcher.StartTimeout();
    }

    void RequiredEntry::SetCorrectnessCheck(std::function<bool()> lambda) {
        isTextValid = lambda;
    }

    bool RequiredEntry::IsValid() {
        return isTextValid();
    }



}