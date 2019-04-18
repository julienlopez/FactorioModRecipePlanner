#pragma once

#include <filesystem>
#include <optional>
#include <vector>

namespace filesystem = std::filesystem;

using Item = std::string;

struct ItemQuantity
{
    Item item;
    double quantity;
};

std::ostream& operator<<(std::ostream& o, const ItemQuantity& iq);

ItemQuantity operator*(ItemQuantity iq, double ratio);

ItemQuantity operator*(double ratio, ItemQuantity iq);

struct Recipe
{
    std::string name;
    double energy;
    std::vector<ItemQuantity> inputs;
    std::vector<ItemQuantity> outputs;
};

class Analyzer
{
public:
    Analyzer(const std::vector<filesystem::path>& files);

    ~Analyzer() = default;

    std::vector<ItemQuantity> computeRequirements(const ItemQuantity& iq) const;

    static constexpr double c_productivity_bonus = 1.4;
    static const std::string c_default_productivity_module;

private:
    using RecipeContainer_t = std::vector<Recipe>;
    const RecipeContainer_t m_recipes;
    const std::vector<Item> m_modulable_items;

    std::optional<const Recipe> findRecipeProducing(const Item& item) const;

    static RecipeContainer_t loadRecipeFiles(const std::vector<filesystem::path>& files);

    static RecipeContainer_t loadRecipeFile(const filesystem::path& file);

    static std::vector<Item> loadModulableItems(const filesystem::path& file);

    bool isCompatibleWithProductivityModules(const Item& item) const;

    double computeRatio(const std::vector<ItemQuantity>& recipe_outputs, const ItemQuantity& production_wanted) const;
};
