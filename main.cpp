#include <cassert>
#include <iostream>
#include <filesystem>

namespace filesystem = std::filesystem;

std::vector<filesystem::path> exploreFiles(const filesystem::path& dir)
{
    assert(filesystem::is_directory(dir));
    std::vector<filesystem::path> res;
	for (auto it = filesystem::directory_iterator(dir); it != filesystem::directory_iterator(); ++it)
	{
		if (filesystem::is_directory(*it))
		{
            auto sub_res = exploreFiles(*it);
            std::copy(std::make_move_iterator(begin(sub_res)), std::make_move_iterator(end(sub_res)),
                      std::back_inserter(res));
		}
		else
		{
            res.push_back(it->path());
		}
	}
	return res;
}


int main(int argc, char* argv[])
{
    const auto exec_dir = filesystem::path(argv[0]).parent_path();
    const auto analyze_dir = exec_dir / "recipe";
    if(!filesystem::is_directory(analyze_dir))
    {
        std::cerr << "Unable to find recipe directory" << std::endl;
        return EXIT_FAILURE;
    }

    const auto files = exploreFiles(analyze_dir);
    std::cout << files.size() << std::endl;
	for(const auto& file : files)
        std::cout << file << std::endl;

    return EXIT_SUCCESS;
}