#pragma once

#include <gtkmm.h>
#include <GUI/components/StyleableEntry.h>

namespace Clavis::GUI {
    class RequiredEntry : public StyleableEntry {
    public:
        RequiredEntry();
        bool IsValid() const;
        bool IsEmpty() const;
        void SetCorrectnessCheck(std::function<bool()> lambda);

    protected:

    private:
        std::function<bool()> isTextValid;
    };
}