#include "PropertiesFile.hpp"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

tl::expected<PropertiesFileData, std::string> LoadFromJSON(std::string_view filePath) noexcept
{
    if (!std::filesystem::exists(filePath)) {
        return tl::unexpected("Unable to find {}");
    }

    std::ifstream inputFile;
    inputFile.open(filePath, std::ios::in);

    if (inputFile.fail()) {
        return tl::unexpected("Unable to load {}");
    }

    PropertiesFileData propFileData{};

    // Dealing with nlohmann::json without exceptions looks like a pain
    // so we'll try-catch and then return an unexpected
    try {
        nlohmann::json jsonRoot;

        inputFile >> jsonRoot;

    } catch (const nlohmann::json::exception& e) {
        return tl::unexpected(e.what());
    }
    return { propFileData };
}