#pragma once

#include <glibmm.h>

namespace Clavis::GUI {
    class MainLoopHalter {
    public:
        MainLoopHalter();
        void Halt();
        void Resume();

        ~MainLoopHalter();

    protected:

    private:
        std::shared_ptr<Glib::MainLoop> nestedLoop = nullptr;
    };
}