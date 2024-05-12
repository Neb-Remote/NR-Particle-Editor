#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

struct PropertiesFileData;

class ParticleEmitter : public sf::Drawable, public sf::Transformable {
public:
    explicit ParticleEmitter(PropertiesFileData* propertiesFileData);

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    PropertiesFileData* m_propertiesFileData;
    sf::VertexArray m_vertexArray;
};