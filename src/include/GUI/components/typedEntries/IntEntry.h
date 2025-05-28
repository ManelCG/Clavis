#pragma once

#include <GUI/components/typedEntries/TypedEntry.h>

namespace Clavis::GUI {
    class IntEntry : public TypedEntry<int> {
    public:
        virtual void OnEntryInsertText(const Glib::ustring& text, int* position) override;
        virtual int GetValue() override;
        virtual void SetValue(int value) override;
    protected:

    private:
    };
}