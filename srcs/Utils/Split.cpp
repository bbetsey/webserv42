#include "Utils.hpp"

void split(const std::string& str, std::vector<std::string>& tokens, const std::string &delimiters)
{
	size_t pos, lastPos = 0, length = str.length();

	while (lastPos < length + 1)
	{
		pos = str.find_first_of(delimiters, lastPos);
		if (pos == std::string::npos)
			pos = length;
		if (pos != lastPos)
			tokens.push_back(std::string(str.data() + lastPos, pos - lastPos));
		lastPos = pos + 1;
	}
}
