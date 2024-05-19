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
    sf::Angle rotation;
    sf::Time aliveTime;
    sf::Time maxAliveTime;
    f32 scale { 1.f };
    f32 scaleMultiplier { 1.f };
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
    bool isPlaying() const { return m_isPlaying; }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void reconstructVertices();
    void emitParticle(sf::Time timeAhead);

    PropertiesFileData* m_propertiesFileData;
    sf::VertexBuffer m_vertexBuffer;
    std::vector<Particle> m_activeParticles;
    sf::Time m_spawnTimer;
    sf::Time m_aliveTime;
    bool m_isPlaying { false };
    bool m_loop { false };
};