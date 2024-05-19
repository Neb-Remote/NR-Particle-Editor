#include "PropertiesFile.hpp"
#include "Types.hpp"

#include <filesystem>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

std::optional<sf::Color> ColourFromHex(std::string_view hexCode)
{
    if (hexCode.length() != 7)
        return std::nullopt;

    const auto hexStr = hexCode.substr(1, 6);
    u32 hexAsUint {};
    std::stringstream sstream;
    sstream << std::hex << hexStr;
    sstream >> hexAsUint;

    // Shift the values up and force alpha to be 255 for now
    hexAsUint = hexAsUint << 8;
    hexAsUint = hexAsUint | 0xFF;
    return sf::Color(hexAsUint);
}

tl::expected<PropertiesFileData, std::string> LoadFromJSON(std::string_view filePath) noexcept
{
    if (!std::filesystem::exists(filePath)) {
        return tl::unexpected("Unable to find {}");
    }

    std::ifstream inputFile;
    inputFile.open(filePath.data(), std::ios::in);

    if (inputFile.fail()) {
        return tl::unexpected("Unable to load {}");
    }

    PropertiesFileData propFileData {};

    // Dealing with nlohmann::json without exceptions looks like a pain
    // so we'll try-catch and then return an unexpected
    try {
        nlohmann::json jsonRoot;

        inputFile >> jsonRoot;

        // Load Particle Properites
        propFileData.particleProperties.alphaStart = jsonRoot["alpha"]["start"].get<f32>();
        propFileData.particleProperties.alphaEnd = jsonRoot["alpha"]["end"].get<f32>();

        propFileData.particleProperties.scaleStart = jsonRoot["scale"]["start"].get<f32>();
        propFileData.particleProperties.scaleEnd = jsonRoot["scale"]["end"].get<f32>();
        propFileData.particleProperties.minimumScaleMultiplier = jsonRoot["scale"]["minimumScaleMultiplier"].get<f32>();

        const auto colourStartString = jsonRoot["color"]["start"].get<std::string>();
        const auto colourStartEnd = jsonRoot["color"]["end"].get<std::string>();

        if (const auto colour = ColourFromHex(colourStartString)) {
            propFileData.particleProperties.colourStart = *colour;
        } else {
            return tl::unexpected("Unable to isolate particle start colour from JSON");
        }

        if (const auto colour = ColourFromHex(colourStartEnd)) {
            propFileData.particleProperties.colourEnd = *colour;
        } else {
            return tl::unexpected("Unable to isolate particle end colour from JSON");
        }

        propFileData.particleProperties.speedStart = jsonRoot["speed"]["start"].get<f32>();
        propFileData.particleProperties.speedEnd = jsonRoot["speed"]["end"].get<f32>();
        propFileData.particleProperties.minimumSpeedMultiplier = jsonRoot["speed"]["minimumSpeedMultiplier"].get<f32>();

        propFileData.particleProperties.acceleration.x = jsonRoot["acceleration"]["x"].get<f32>();
        propFileData.particleProperties.acceleration.y = jsonRoot["acceleration"]["y"].get<f32>();

        propFileData.particleProperties.maximumSpeed = jsonRoot["maxSpeed"].get<f32>();

        propFileData.particleProperties.startRotationMin = sf::degrees(jsonRoot["startRotation"]["min"].get<f32>());
        propFileData.particleProperties.startRotationMax = sf::degrees(jsonRoot["startRotation"]["max"].get<f32>());

        propFileData.particleProperties.lifeTimeMin = sf::seconds(jsonRoot["lifetime"]["min"].get<f32>());
        propFileData.particleProperties.lifeTimeMax = sf::seconds(jsonRoot["lifetime"]["max"].get<f32>());

        // Load Emitter Properties
        propFileData.emitterProperties.lifeTime = sf::seconds(jsonRoot["emitterLifetime"].get<f32>());
        propFileData.emitterProperties.maxParticles = jsonRoot["maxParticles"].get<u32>();
    } catch (const nlohmann::json::exception& e) {
        return tl::unexpected(e.what());
    }
    return { propFileData };
}