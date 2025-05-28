#pragma once

#include <GUI/Tables/WidgetTable.h>

#include <memory>
#include <vector>

namespace Clavis::GUI {
	class StringTable : public WidgetTable {
	public:
		void AppendRow(std::string s1, std::string s2);

	protected:

	private:
	};
}