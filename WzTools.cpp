#include "WzTools.h"

auto WzTools::split(const std::string &str, std::vector<std::string> &ret, std::string sep, bool allowNull) -> int
{
	if (str.empty())
	{
		return 0;
	}
	std::string tmp;
	std::string::size_type pos_begin = str.find_first_not_of(sep);
	std::string::size_type comma_pos = 0;
	while (pos_begin != std::string::npos)
	{
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != std::string::npos)
		{
			tmp = str.substr(pos_begin, comma_pos - pos_begin);
			pos_begin = comma_pos + sep.length();
		}
		else
		{
			tmp = str.substr(pos_begin);
			pos_begin = comma_pos;
		}

		if (!tmp.empty())
		{
			if (tmp == "" && !allowNull) {
				ret.push_back(tmp);
			}
			else {
				ret.push_back(tmp);
			}
			tmp.clear();
		}
	}
	return 0;
}
auto WzTools::endWith(const std::string &str, const std::string &endStr) ->bool
{
	
		if (str.empty() || endStr.empty()) {
			return false;
		}
		if (str.size() < endStr.size()) {
			return false;
		}
		return str.compare(str.size() - endStr.size(), endStr.size(), endStr) == 0 ? true : false;
	
}

auto WzTools::getIndexInTypes(std::string &str) -> int
{
	int i = 0;
	for (auto a : WzTools::types)
	{
		if (a == str) {
			return i;
		}
		i++;
	}
	return -1;
}

std::string WzTools::types[6] = {
	"Shape2D#Convex2D",
	"Shape2D#Vector2D",
	"Sound_DX8",
	"Property",
	"Canvas",
	"UOL"
};