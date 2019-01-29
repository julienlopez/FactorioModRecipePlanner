#pragma once

#include <filesystem>
#include <vector>

namespace filesystem = std::filesystem;

class Analyzer
{
public:
    Analyzer() = default;
    ~Analyzer() = default;

	void loadRecipeFiles(const std::vector<filesystem::path>& files);

    void loadRecipeFile(const filesystem::path& file);

private:
};
