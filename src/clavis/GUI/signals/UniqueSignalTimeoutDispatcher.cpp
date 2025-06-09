#include <GUI/signals/UniqueSignalTimeoutDispatcher.h>

namespace Clavis::GUI {
    UniqueSignalTimeoutDispatcher::UniqueSignalTimeoutDispatcher() {

    }

    void UniqueSignalTimeoutDispatcher::SetMilliseconds(int ms) {
        milliseconds = ms;
    }

    void UniqueSignalTimeoutDispatcher::SetSeconds(int s) {
        milliseconds = s*1000;
    }

    void UniqueSignalTimeoutDispatcher::SetAction(std::function<void()> action) {
        dispatcher.connect(action);
    }


    void UniqueSignalTimeoutDispatcher::StartTimeout() {
        timeout.ConnectOnce([this]() {
            dispatcher.emit();
        }, milliseconds);
    }

}