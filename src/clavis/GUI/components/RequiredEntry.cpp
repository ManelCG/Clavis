#include <GUI/components/RequiredEntry.h>

namespace Clavis::GUI {
    RequiredEntry::RequiredEntry() {
        isTextValid = [this]() {
            if (IsEmpty())
                return false;

            return true;
        };
    }
    void RequiredEntry::SetCorrectnessCheck(std::function<bool()> lambda) {
        isTextValid = lambda;
    }

    bool RequiredEntry::IsValid() const {
        return isTextValid();
    }

    bool RequiredEntry::IsEmpty() const {
        return get_text().empty();
    }




}