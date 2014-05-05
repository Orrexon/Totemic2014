#include "String.h"

namespace String
{
	std::string replace(const std::string &original, const std::string &exp, const std::string &replacement)
	{
		std::tr1::regex regexp(exp);
		return std::tr1::regex_replace(original, regexp, replacement);
	}
}