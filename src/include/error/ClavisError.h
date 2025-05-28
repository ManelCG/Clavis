#pragma once

#include <exception>
#include <string>
#include <vector>

namespace Clavis::Error {
    class ClavisException : public std::exception {
    public:
        typedef struct ClavisExceptionData {
			std::vector<std::string> unformattedMessage;
			bool isMessageFormatted;

            std::string version;
            std::string platform;
            std::string buildMode;
            std::string buildDate;

            std::string file;
            std::string function;
            int line;

            std::string exceptionText;
        	std::string message;
        } ClavisExceptionData;

        explicit ClavisException(ClavisExceptionData exceptiondata);

        [[nodiscard]] const char* what() const noexcept override;
    	[[nodiscard]] const char* getExceptionText() const noexcept;

        static void LogException(ClavisExceptionData exc);
        static ClavisExceptionData GetLastException();

        ClavisExceptionData Data;

    protected:

    private:
    };
}

#define RaiseClavisError(...) { auto exc = Clavis::Error::ClavisException({\
		std::vector<std::string>{__VA_ARGS__},\
		false,\
		\
		CLAVIS_VERSION,\
		__RUNNING_PLATFORM__,\
		__BUILD_MODE__,\
		__DATE__,\
		__FILENAME__,\
		__FUNCTION__,\
		__LINE__,\
		\
		""\
	}); \
    Clavis::Error::ClavisException::LogException(exc.Data); \
	throw exc; \
}
