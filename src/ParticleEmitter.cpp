#include "ParticleEmitter.hpp"
#include "PropertiesFile.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

ParticleEmitter::ParticleEmitter(PropertiesFileData* propertiesFileData)
    : m_propertiesFileData(propertiesFileData)
{
    if (m_propertiesFileData->particleProperties.texture != nullptr) {
        m_vertexArray.setPrimitiveType(sf::PrimitiveType::Triangles);
    } else {
        m_vertexArray.setPrimitiveType(sf::PrimitiveType::Points);
    }
}

void ParticleEmitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_vertexArray, states);
}