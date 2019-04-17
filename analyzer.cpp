#include "analyzer.hpp"

#include "json/json.h"

#include <cassert>
#include <fstream>

#include <gsl/gsl_utils>

namespace
{

std::string readFileAsString(const filesystem::path& file)
{
    std::ifstream stream(file.string());
    return {(std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>()};
}

auto parseItemQuantities(const nlohmann::json::array_t& json)
{
    std::vector<ItemQuantity> res;
    for(const auto& iq : json)
    {
        std::cout << iq.dump(2) << std::endl;
        res.push_back(ItemQuantity{iq["name"], gsl::narrow<std::size_t>(iq["amount"])});
	}
    return res;
}

Recipe parseRecipe(const nlohmann::json& json)
{
	// std::cout << json.dump(2) << std::endl;
    const auto& inputs = json["ingredients"];
    const auto& outputs = json["results"];
    return {json["name"], json["energy_required"], parseItemQuantities(inputs), parseItemQuantities(outputs)};
}

auto loadRecipes(const nlohmann::json& json)
{
    const auto recipe_node = json["recipes"];
    std::vector<Recipe> res;
    for(const auto& recipe : recipe_node)
    {
        res.push_back(parseRecipe(recipe));
    }
    return res;
}

} // namespace

Analyzer::Analyzer(const std::vector<filesystem::path>& files)
    : m_recipes(loadRecipeFiles(files))
{
    std::cout << m_recipes.size() << " recipes loaded" << std::endl;
}

void Analyzer::computeRequirements(const ItemQuantity& iq) const
{
    std::cout << "looking for " << iq.item << " (x" << iq.quantity << ")" << std::endl;
    const auto& recipe = findRecipeProducing(iq.item);
    if(!recipe) throw std::runtime_error("Unable to find recipe for " + iq.item);
}

std::optional<const Recipe> Analyzer::findRecipeProducing(const Item& item) const
{
    for(const auto& recipe : m_recipes)
    {
        const auto it = std::find_if(begin(recipe.outputs), end(recipe.outputs),
                                     [&item](const ItemQuantity& iq) { return iq.item == item; });
        if(it != end(recipe.outputs)) return recipe;
    }
    return std::nullopt;
}

auto Analyzer::loadRecipeFiles(const std::vector<filesystem::path>& files) -> RecipeContainer_t
{
    RecipeContainer_t res;
    for(const auto& file : files)
    {
        auto recipes = loadRecipeFile(file);
        res.reserve(res.size() + recipes.size());
        std::copy(std::make_move_iterator(begin(recipes)), std::make_move_iterator(end(recipes)),
                  std::back_inserter(res));
    }
    return res;
}

auto Analyzer::loadRecipeFile(const filesystem::path& file) -> RecipeContainer_t
{
    auto content = readFileAsString(file);
    const auto json = nlohmann::json::parse(content);
    std::cout << file << " : \n";
    return loadRecipes(json);
}
