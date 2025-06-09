#pragma once

#include <glibmm.h>
#include <mutex>

namespace Clavis::GUI {
    typedef uint32_t SignalCallerID;

    class UniqueSignalTimeout {
    public:
        UniqueSignalTimeout();

        // Returns caller ID. Can be used to disconnect the signal
        SignalCallerID ConnectOnce(std::function<void()> callback, int ms, int priority = Glib::PRIORITY_DEFAULT);
        SignalCallerID ConnectOnceSeconds(std::function<void()> callback, int seconds, int priority = Glib::PRIORITY_DEFAULT);
        SignalCallerID Connect(std::function<bool()> callback, int ms, int priority = Glib::PRIORITY_DEFAULT);
        SignalCallerID ConnectSeconds(std::function<bool()> callback, int seconds, int priority = Glib::PRIORITY_DEFAULT);

        void Disconnect(const SignalCallerID id);

    protected:

    private:
        std::mutex mutex;

        SignalCallerID lastCallerID;
    };
}
