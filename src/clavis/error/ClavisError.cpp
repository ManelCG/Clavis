#include <error/ClavisError.h>

#include <regex>
#include <language/Language.h>

#include <stack>

std::stack<Clavis::Error::ClavisException::ClavisExceptionData> ExceptionStack;

namespace Clavis {
	namespace Error {
		ClavisException::ClavisException(ClavisExceptionData exceptionData) {
			Data = exceptionData;

			if (!Data.isMessageFormatted) {
				std::string s;
				for (auto& e : exceptionData.unformattedMessage)
					s += e;

				Data.exceptionText = s;

				std::string formatted;
				formatted += "Caught ClavisException:\n\n";
				formatted += "" + Data.exceptionText + "\n\n";
				formatted += "On version:  " + Data.version + "\n";
				formatted += "On Platform: " + Data.platform + "\n";
				formatted += "Build Mode:  " + Data.buildMode + "\n";
				formatted += "Build Date:  " + Data.buildDate + "\n";
				formatted += "On file:     " + Data.file + "\n";
				formatted += "On Function: " + Data.function + "\n";
				formatted += "On Line:     " + std::to_string(Data.line) + "\n";

				Data.message = formatted;
			}

			//Data.function = std::regex_replace(Data.function, std::regex("<"), "&lt;");
			//Data.function = std::regex_replace(Data.function, std::regex(">"), "&gt;");

			if (std::regex_match(Data.function, std::regex(".*<lambda.*")))
				Data.function = _(UNNAMED_LAMBDA_SUBSITUTION);
			else
				Data.function = "<tt>" + Data.function + "</tt>";

			Data.file = "<tt>" + Data.file + "</tt>";
		}

		const char* ClavisException::what() const noexcept{
			return Data.message.c_str();
		}

		const char *ClavisException::getExceptionText() const noexcept {
			return Data.exceptionText.c_str();
		}


		void ClavisException::LogException(ClavisExceptionData exc){
			ExceptionStack.push(exc);
		}

		ClavisException::ClavisExceptionData ClavisException::GetLastException() {
			auto data = ExceptionStack.top();
			ExceptionStack.pop();
			return data;
		}
	}
}