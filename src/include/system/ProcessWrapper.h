#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Clavis::System {
    class ProcessWrapperData;

    class ProcessWrapper {
    public:
        typedef enum WellKnownExecutable {
            GIT,
        } WellKnownExecutable;

        ProcessWrapper();
        void Init(std::string executable, std::vector<std::string> args, const std::filesystem::path& workingDir = ".");
        void Init(WellKnownExecutable, std::vector<std::string> args, const std::filesystem::path& workingDir = ".");
        void Kill();
        void Cleanup();
        void Wait() const;

        bool WriteData(uint8_t* data, size_t size);
        std::string GetOutput() const;
        std::string GetError() const;
        bool IsAlive() const;

        int GetExitCode() const;



        static std::string GetExecutablePath(WellKnownExecutable executable);

    private:
        std::shared_ptr<ProcessWrapperData> ProcessData;
    };
}
