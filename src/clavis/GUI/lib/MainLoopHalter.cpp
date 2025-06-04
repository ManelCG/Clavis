#include <GUI/lib/MainLoopHalter.h>

namespace Clavis::GUI {
    MainLoopHalter::MainLoopHalter() {
        nestedLoop = Glib::MainLoop::create();
    }

    void MainLoopHalter::Halt() {
        if (!nestedLoop->is_running())
            nestedLoop->run();
    }

    void MainLoopHalter::Resume() {
        if (nestedLoop->is_running())
            nestedLoop->quit();
    }

    MainLoopHalter::~MainLoopHalter() {
        if (nestedLoop->is_running())
            nestedLoop->quit();

        nestedLoop = nullptr;
    }



}