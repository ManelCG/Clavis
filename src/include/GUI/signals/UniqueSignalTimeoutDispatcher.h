#pragma once

#include <GUI/signals/UniqueSignalTimeout.h>

namespace Clavis::GUI {
    class UniqueSignalTimeoutDispatcher {
    public:
        UniqueSignalTimeoutDispatcher();

        void SetAction(std::function<void()> action);
        void SetMilliseconds(int ms);
        void SetSeconds(int s);

        void StartTimeout();

    protected:

    private:
        UniqueSignalTimeout timeout;
        Glib::Dispatcher dispatcher;

        int milliseconds = 3000;

    };
}