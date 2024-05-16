#pragma once

#include "Types.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/System/Time.hpp>
#include <vector>

struct PropertiesFileData;

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::Angle rotation;
    sf::Time aliveTime;
    sf::Time maxAliveTime;
    bool operator==(const Particle& p) const = default;
};

class ParticleEmitter : public sf::Drawable, public sf::Transformable {
public:
    explicit ParticleEmitter(PropertiesFileData* propertiesFileData);
    void update(sf::Time dt);

    void play();
    void stop();
    void setLooping(bool looping);

    u32 getAliveParticleCount() const { return static_cast<u32>(m_activeParticles.size()); }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void reconstructVertices();
    void emitParticle();

    PropertiesFileData* m_propertiesFileData;
    sf::VertexBuffer m_vertexBuffer;
    std::vector<Particle> m_activeParticles;
    sf::Time m_spawnTimer;
    sf::Time m_aliveTime;
    bool m_isPlaying { false };
    bool m_loop { false };
};