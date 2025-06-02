#include <GUI/signals/UniqueSignalTimeout.h>

#include <extensions/RNG.h>

namespace Clavis::GUI {
    UniqueSignalTimeout::UniqueSignalTimeout() {
        lastCallerID = 0;
    }

    SignalCallerID UniqueSignalTimeout::ConnectOnce(std::function<void()> callback, int ms, int priority) {
        std::lock_guard lock(mutex);

        lastCallerID = Extensions::RNG::GetUInt32();

        int callbackCallerID = lastCallerID;
        Glib::signal_timeout().connect_once([this, callback, callbackCallerID]() {
            std::lock_guard lock(mutex);

            if (lastCallerID != callbackCallerID)
                return;

            callback();
        }, ms, priority);

        return lastCallerID;
    }

    SignalCallerID UniqueSignalTimeout::ConnectOnceSeconds(std::function<void()> callback, int seconds, int priority) {
        return ConnectOnce(callback, seconds*1000, priority);
    }

    SignalCallerID UniqueSignalTimeout::Connect(std::function<bool()> callback, int ms, int priority) {
        std::lock_guard lock(mutex);

        lastCallerID = Extensions::RNG::GetUInt32();
        int callbackCallerID = lastCallerID;

        Glib::signal_timeout().connect([this, callback, callbackCallerID]() {
            std::lock_guard lock(mutex);

            if (lastCallerID != callbackCallerID)
                return false;

            return callback();
        }, ms, priority);

        return lastCallerID;
    }

    SignalCallerID UniqueSignalTimeout::ConnectSeconds(std::function<bool()> callback, int seconds, int priority) {
        return Connect(callback, seconds*1000, priority);
    }




    void UniqueSignalTimeout::Disconnect(const SignalCallerID id) {
        std::lock_guard lock(mutex);

        if (lastCallerID != id)
            return;

        lastCallerID = 0;
    }
}