#include "analyzer.hpp"

#include "json/json.h"

#include <cassert>
#include <fstream>

#include <gsl/gsl_util>

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
        res.push_back(ItemQuantity{iq["name"], (nlohmann::json::number_float_t)iq["amount"]});
    }
    return res;
}

Recipe parseRecipe(const nlohmann::json& json)
{
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

auto compareIqByItem(const Item& item)
{
    return [&item](const ItemQuantity& iq) { return iq.item == item; };
}

bool isNotCompatibleWithProductivityModules(const Item& item)
{
    const std::vector<Item> final_items{"inserter", "transport-belt"};
    return std::find(begin(final_items), end(final_items), item) != end(final_items);
}

double computeRatio(const std::vector<ItemQuantity>& recipe_outputs, const ItemQuantity& production_wanted)
{
    const auto it = find_if(begin(recipe_outputs), end(recipe_outputs), compareIqByItem(production_wanted.item));
    if(it == end(recipe_outputs)) throw std::runtime_error("bad outputs given to computeRatio");
    if(isNotCompatibleWithProductivityModules(production_wanted.item))
        return production_wanted.quantity / it->quantity;
    else
        return production_wanted.quantity / it->quantity / Analyzer::c_productivity_bonus;
}

bool isOnBus(const Item& item)
{
    const std::vector<Item> items_on_the_bus{"copper-plate", "iron-plate", "iron-gear-wheel", "electronic-circuit"};
    return std::find(begin(items_on_the_bus), end(items_on_the_bus), item) != end(items_on_the_bus);
}

std::vector<ItemQuantity> mergeRequirements(std::vector<ItemQuantity> requirements)
{
    std::map<Item, double> map;
    for(auto& req : requirements)
    {
        const auto p = map.insert(std::make_pair(req.item, 0.));
        p.first->second += req.quantity;
    }
    std::vector<ItemQuantity> res;
    for(auto& p : map)
        res.push_back({std::move(p.first), p.second});
    return res;
}

} // namespace

std::ostream& operator<<(std::ostream& o, const ItemQuantity& iq)
{
    return o << iq.item << " x" << iq.quantity << "/s (" << (iq.quantity / 45) << " bb)";
}

ItemQuantity operator*(ItemQuantity iq, double ratio)
{
    return {std::move(iq.item), ratio * iq.quantity};
}

ItemQuantity operator*(double ratio, ItemQuantity iq)
{
    return std::move(iq) * ratio;
}

Analyzer::Analyzer(const std::vector<filesystem::path>& files)
    : m_recipes(loadRecipeFiles(files))
{
    std::cout << m_recipes.size() << " recipes loaded" << std::endl;
}

std::vector<ItemQuantity> Analyzer::computeRequirements(const ItemQuantity& iq) const
{
    std::cout << "looking for " << iq << std::endl;
    const auto& recipe = findRecipeProducing(iq.item);
    if(!recipe) return {iq};
    std::vector<ItemQuantity> requirements;
    const auto ratio = computeRatio(recipe->outputs, iq);
    for(const auto& res : recipe->inputs)
    {
        const auto requirement = ratio * res;
        std::cout << requirement << std::endl;
        if(isOnBus(requirement.item))
        {
            requirements.push_back(requirement);
        }
        else
        {
            auto sub_res = computeRequirements(requirement);
            requirements.reserve(requirements.size() + sub_res.size());
            std::copy(std::make_move_iterator(begin(sub_res)), std::make_move_iterator(end(sub_res)),
                      std::back_inserter(requirements));
        }
    }
    return mergeRequirements(std::move(requirements));
}

std::optional<const Recipe> Analyzer::findRecipeProducing(const Item& item) const
{
    for(const auto& recipe : m_recipes)
    {
        const auto it = std::find_if(begin(recipe.outputs), end(recipe.outputs), compareIqByItem(item));
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
