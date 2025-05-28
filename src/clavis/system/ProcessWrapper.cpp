#include <system/ProcessWrapper.h>

#ifdef __WINDOWS__
#include <windows.h>
#elif defined __LINUX__
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include <error/ClavisError.h>

#include <language/Language.h>
#include <system/Extensions.h>

namespace Clavis::System {
	class ProcessWrapperData {
	public:
		typedef struct PipeData {
#ifdef __WINDOWS__
			HANDLE NamedPipe;
			HANDLE PipeEnd;
			std::shared_ptr<OVERLAPPED> Overlapped;
#elif defined __LINUX__
			int PipeStdin[2];
			int PipeStdout[2];
			int PipeStderr[2];
#endif
		} PipeData;

#ifdef __WINDOWS__
		HANDLE HProcess;
		HANDLE HThread;
#elif defined __LINUX__
		int ProcessPid;
#endif

		PipeData Pipe;

		std::string StdoutBuffer;
		std::string StderrBuffer;

		bool isExitCodeKnown = false;
		int exitCode = -1;
	};

	int ProcessWrapper::GetExitCode() const {
#ifdef __WINDOWS__
		DWORD exitCode = 0;
		if (WaitForSingleObject(ProcessData->HProcess, INFINITE) == WAIT_OBJECT_0) {
			if (GetExitCodeProcess(ProcessData->HProcess, &exitCode))
				return static_cast<int>(exitCode);
		}
		return -1; // wait or exit code retrieval failed

#elif defined __LINUX__
		if (ProcessData->isExitCodeKnown)
			return ProcessData->exitCode;

		int status;

		if ( waitpid(ProcessData->ProcessPid, &status, 0) > 0 && WIFEXITED(status)) {
			ProcessData->isExitCodeKnown = true;
			ProcessData->exitCode = WEXITSTATUS(status);
		}
		else {
	    	ProcessData->isExitCodeKnown = true;
	    	ProcessData->exitCode = WEXITSTATUS(status);
	    }

		return ProcessData->exitCode;
#endif
	}


	ProcessWrapper::ProcessWrapper() {
		ProcessData = std::make_shared<ProcessWrapperData>();
	}

	void ProcessWrapper::Init(WellKnownExecutable executable, std::vector<std::string> args, const std::filesystem::path& workingDir) {
		auto s = GetExecutablePath(executable);
		Init(s, args, workingDir);
	}

	void ProcessWrapper::Init(std::string executable, std::vector<std::string> args, const std::filesystem::path& workingDir) {

	#ifdef __WINDOWS__
		SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		// TODO: ADD A RANDOM SUFFIX TO ALLOW MULTIPLE RENDERING SIMULTANEOUSLY
		srand((unsigned)time(nullptr));
		std::string pipeSuffix = std::to_string(rand());
		std::string pipeName = "\\\\.\\pipe\\clavis_pipe_" + pipeSuffix;

		ProcessData->Pipe.NamedPipe= CreateNamedPipe(
			pipeName.c_str(),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
			1,
			10000000,
			10000000,
			0,
			&saAttr
		);

		if (ProcessData->Pipe.NamedPipe == INVALID_HANDLE_VALUE)
			RaiseClavisError(_(ERROR_CREATING_NAMED_PIPE, std::to_string(GetLastError())));

		ProcessData->Pipe.PipeEnd = CreateFile(
			pipeName.c_str(),
			GENERIC_READ,                           // Desired access (read)
			0,                                      // No sharing mode (exclusive access)
			&saAttr,                                   // Security attributes
			OPEN_EXISTING,                          // Open existing pipe
			0,                                      // No attributes
			NULL                                    // No template file
		);


		ProcessData->Pipe.Overlapped = std::shared_ptr<OVERLAPPED>((OVERLAPPED*)calloc(1, sizeof(OVERLAPPED)));	// Cast so that compiler doesn't cry.
		ConnectNamedPipe(ProcessData->Pipe.NamedPipe, ProcessData->Pipe.Overlapped.get());

		STARTUPINFO si = { sizeof(STARTUPINFO) };
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		si.hStdInput = ProcessData->Pipe.PipeEnd;

		PROCESS_INFORMATION pi = { 0 };


		auto cmd = executable;
		for (auto arg : args)
			cmd += " " + arg;

		auto success = CreateProcess(
			NULL,
			(LPSTR)cmd.c_str(),
			NULL,
			NULL,
			TRUE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&si,
			&pi
		);

		if (!success)
			RaiseClavisError(_(ERROR_UNABLE_TO_START_PROCESS, "<tt>" + executable + "</tt>"));

		ProcessData->HProcess = pi.hProcess;
		ProcessData->HThread = pi.hThread;
	#elif defined __LINUX__
		if (pipe(ProcessData->Pipe.PipeStdin) == -1) RaiseClavisError(_(ERROR_CREATING_PIPE));
		if (pipe(ProcessData->Pipe.PipeStdout) == -1) RaiseClavisError(_(ERROR_CREATING_PIPE));
		if (pipe(ProcessData->Pipe.PipeStderr) == -1) RaiseClavisError(_(ERROR_CREATING_PIPE));
		if ((ProcessData->ProcessPid = fork()) == -1) RaiseClavisError(_(ERROR_CALLING_FORK));

		std::vector<char*> argv;

		// executable = "echo";
		argv.push_back((char*)executable.c_str());
		for (auto& arg : args)
			argv.push_back((char*)arg.c_str());
		argv.push_back(nullptr);

		if (ProcessData->ProcessPid == 0) {
			System::ChDir(workingDir);

			close(0);
			dup(ProcessData->Pipe.PipeStdin[0]);
			close(ProcessData->Pipe.PipeStdin[0]);
			close(ProcessData->Pipe.PipeStdin[1]);

			close(1);
			dup(ProcessData->Pipe.PipeStdout[1]);
			close(ProcessData->Pipe.PipeStdout[1]);
			close(ProcessData->Pipe.PipeStdout[0]);

			close(2);
			dup(ProcessData->Pipe.PipeStderr[1]);
			close(ProcessData->Pipe.PipeStderr[1]);
			close(ProcessData->Pipe.PipeStderr[0]);

			execvp(executable.c_str(), argv.data());
			RaiseClavisError(_(ERROR_UNABLE_TO_START_PROCESS, "<tt>" + executable + "</tt>"));
		}

		close(ProcessData->Pipe.PipeStdin[0]);
	#endif
	}

	void ProcessWrapper::Kill() {
		#ifdef __WINDOWS__
		TerminateProcess(ProcessData->HProcess, 1);
		#elif defined __LINUX__
		kill(ProcessData->ProcessPid, SIGTERM);
		#endif
	}

	void ProcessWrapper::Cleanup() {
#ifdef __WINDOWS__
		CloseHandle(ProcessData->Pipe.PipeEnd);
		CloseHandle(ProcessData->Pipe.NamedPipe);
		CloseHandle(ProcessData->HProcess);
		CloseHandle(ProcessData->HThread);
#elif defined __LINUX__
		close(ProcessData->Pipe.PipeStdin[1]);
		close(ProcessData->Pipe.PipeStdout[0]);
		close(ProcessData->Pipe.PipeStderr[0]);
#endif
	}


	bool ProcessWrapper::WriteData(uint8_t* data, size_t size) {
#ifdef __WINDOWS__
		if (!WriteFile(ProcessData->Pipe.NamedPipe, data, (DWORD)size, nullptr, ProcessData->Pipe.Overlapped.get()))
			if (GetLastError() != ERROR_IO_PENDING)
				return false;
#elif defined __LINUX__
		if (write(ProcessData->Pipe.PipeStdin[1], data, size) == -1)
			return false;
#endif

		return true;
	}

#ifdef __LINUX__
	static std::string ReadFromFd(int fd) {
		constexpr size_t BufferSize = 4096;
		char buffer[BufferSize];
		std::string result;

		// Set the file descriptor to non-blocking
		int flags = fcntl(fd, F_GETFL, 0);
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);

		while (true) {
			ssize_t count = read(fd, buffer, BufferSize);
			if (count == -1) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) break; // No more data
				if (errno == EINTR) continue; // Retry
				break; // Some other error
			} else if (count == 0) {
				break; // EOF
			}
			result.append(buffer, count);
		}

		return result;
	}
#endif


	std::string ProcessWrapper::GetOutput() const {
#ifdef __WINDOWS__
		RaiseClavisError(_(ERROR_NOT_IMPLEMENTED_ON_PLATFORM))
#elif defined __LINUX__
		ProcessData->StdoutBuffer += ReadFromFd(ProcessData->Pipe.PipeStdout[0]);
		return ProcessData->StdoutBuffer;
#endif
	}

	std::string ProcessWrapper::GetError() const {
#ifdef __WINDOWS__
		RaiseClavisError(_(ERROR_NOT_IMPLEMENTED_ON_PLATFORM))
#elif defined __LINUX__
		ProcessData->StderrBuffer += ReadFromFd(ProcessData->Pipe.PipeStderr[0]);
		return ProcessData->StderrBuffer;
#endif
	}


	bool ProcessWrapper::IsAlive() const {
#ifdef __WINDOWS__
		DWORD code;
		GetExitCodeProcess(ProcessData->HProcess, &code);
		return code == STILL_ACTIVE;
#elif defined __LINUX__
		waitpid(ProcessData->ProcessPid, nullptr, WNOHANG);	// Eliminate zombie
		return kill(ProcessData->ProcessPid, 0) == 0; 		// Empty signal, success means PidFfmpeg is alive.
#endif
	}

	void ProcessWrapper::Wait() const {
#ifdef __WINDOWS__
		// Wait indefinitely for the process to terminate
		WaitForSingleObject(ProcessData->HProcess, INFINITE);
#elif defined __LINUX__
		(void)GetExitCode();
#endif
	}


	std::string ProcessWrapper::GetExecutablePath(WellKnownExecutable executable) {
		switch (executable) {
		case GIT:
#ifdef __WINDOWS__
			return "git.exe";
#elif defined __LINUX__
			return "git";
#endif

		default:
			RaiseClavisError(_(ERROR_UNKNOWN_EXECUTABLE, std::to_string(executable)));
		}
	}

}