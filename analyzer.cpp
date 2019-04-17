#include "analyzer.hpp"

#include "json/json.h"

#include <cassert>
#include <fstream>

namespace
{

std::string readFileAsString(const filesystem::path& file)
{
    std::ifstream stream(file.string());
    return {(std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>()};
}

bool startsWith(const std::string& str, const std::string& pattern)
{
    if(pattern.size() > str.size()) return false;
    return str.substr(0, pattern.size()) == pattern;
}

}

void Analyzer::loadRecipeFiles(const std::vector<filesystem::path>& files)
{
    for(const auto& file : files)
        loadRecipeFile(file);
}

void Analyzer::loadRecipeFile(const filesystem::path& file)
{
    auto content = readFileAsString(file);
    // std::cout << content << std::endl;
	try
	{
		const auto json = nlohmann::json::parse(content);
		std::cout << file << " : \n";
		std::cout << json.dump() << std::endl;
	}
	catch (const std::exception& ex)
	{
        // std::cerr << "Unable to parse " << file << "\n" << ex.what() << std::endl;
	}
}
