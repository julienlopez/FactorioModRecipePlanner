#include "analyzer.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

std::vector<filesystem::path> exploreFiles(const filesystem::path& dir)
{
    assert(filesystem::is_directory(dir));
    std::vector<filesystem::path> res;
    for(auto it = filesystem::directory_iterator(dir); it != filesystem::directory_iterator(); ++it)
    {
        if(filesystem::is_directory(*it))
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

    try
    {
        const auto files = exploreFiles(analyze_dir);
        const Analyzer a(files);
        const std::size_t target_quantity = 15;
        const std::vector<ItemQuantity> sciences
            = {{"automation-science-pack", target_quantity}, {"logistic-science-pack", target_quantity},
               {"military-science-pack", target_quantity},   {"chemical-science-pack", target_quantity},
               {"production-science-pack", target_quantity}, {"utility-science-pack", target_quantity},
               {"space-science-pack", target_quantity}};
        auto reqs = a.computeMultipleRequirements(sciences);
        std::cout << "finally : \n";
        std::cout << "per recipe : \n";
        for(std::size_t i = 0; i < sciences.size(); i++)
        {
            std::cout << "\t" << sciences[i].item << " : \n";
            for(const auto& r : reqs.per_recipe_requirements[i])
                std::cout << "\t\t" << r << std::endl;
        }
        std::cout << "total : \n";
        std::sort(begin(reqs.total_requirements), end(reqs.total_requirements),
                  [](const ItemQuantity& iq1, const ItemQuantity& iq2) { return iq1.quantity < iq2.quantity; });
        std::reverse(begin(reqs.total_requirements), end(reqs.total_requirements));
        for(const auto& r : reqs.total_requirements)
            std::cout << "\t" << r << std::endl;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Exception caught :  " << std::endl << ex.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
