#include "ParticleEmitter.hpp"
#include "PropertiesFile.hpp"
#include "RNG.hpp"
#include "Utils.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <spdlog/spdlog.h>

constexpr sf::Vector2f DEFAULT_PARTICLE_SIZE { 16.f, 16.f };
constexpr u32 VERTS_PER_QUAD { 6 };

ParticleEmitter::ParticleEmitter(PropertiesFileData* propertiesFileData)
    : m_propertiesFileData(propertiesFileData)
{
    assert(m_propertiesFileData->emitterProperties.maxParticles > 0);

    if (!m_vertexBuffer.create(m_propertiesFileData->emitterProperties.maxParticles * VERTS_PER_QUAD))
        throw std::runtime_error("Unable to create vertex buffer for ParticleEmitter");

    m_vertexBuffer.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertexBuffer.setUsage(sf::VertexBuffer::Usage::Dynamic);
}

void ParticleEmitter::update(sf::Time dt)
{
    UNUSED(dt);
    if (!m_isPlaying)
        return;

    // Emitter update
    m_aliveTime += dt;
    m_spawnTimer += dt;

    if (m_aliveTime > m_propertiesFileData->emitterProperties.lifeTime && !m_loop) {
        stop();
        return;
    } else if (m_aliveTime > m_propertiesFileData->emitterProperties.lifeTime && m_loop) {
        stop();
        play();
    }

    if (m_spawnTimer > m_propertiesFileData->emitterProperties.spawnFrequency
        && m_aliveTime < m_propertiesFileData->emitterProperties.lifeTime) {
        emitParticle();
        m_spawnTimer = sf::Time::Zero;
    }

    // Update particles
    std::vector<Particle> toRemove;
    toRemove.reserve(m_activeParticles.size());
    for (auto& p : m_activeParticles) {
        p.aliveTime += dt;

        if (p.acceleration != sf::Vector2f {})
            p.velocity += p.acceleration * dt.asSeconds();

        p.position += p.velocity * dt.asSeconds();

        // Find dead particles
        if (p.aliveTime > p.maxAliveTime)
            toRemove.emplace_back(p);
    }

    for (auto& p : toRemove)
        std::erase(m_activeParticles, p);

    reconstructVertices();
}

void ParticleEmitter::play() { m_isPlaying = true; }

void ParticleEmitter::stop()
{
    m_activeParticles.clear();
    m_aliveTime = sf::Time::Zero;
    m_spawnTimer = sf::Time::Zero;
}

void ParticleEmitter::setLooping(bool looping) { m_loop = looping; }

void ParticleEmitter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_isPlaying)
        return;

    states.transform *= getTransform();
    target.draw(m_vertexBuffer, 0, m_activeParticles.size() * VERTS_PER_QUAD, states);
}

void ParticleEmitter::reconstructVertices()
{
    if (m_activeParticles.empty())
        return;

    assert(m_activeParticles.size() * 6 <= m_vertexBuffer.getVertexCount());
    std::vector<sf::Vertex> vertices;

    vertices.resize(m_activeParticles.size() * VERTS_PER_QUAD);

    for (size_t i { 0 }; i < m_activeParticles.size(); ++i) {
        auto quad = &vertices[i * VERTS_PER_QUAD];
        sf::Vector2f halfSize;
        if (m_propertiesFileData->texture) {
            halfSize = sf::Vector2f(m_propertiesFileData->texture->getSize()) * 0.5f;
        } else {
            halfSize = DEFAULT_PARTICLE_SIZE * 0.5f;
        }

        quad[0].position = m_activeParticles[i].position - halfSize;
        quad[1].position = m_activeParticles[i].position + sf::Vector2f(halfSize.x, -halfSize.y);
        quad[2].position = m_activeParticles[i].position + halfSize;

        quad[3].position = m_activeParticles[i].position + halfSize;
        quad[4].position = m_activeParticles[i].position + sf::Vector2f(-halfSize.x, halfSize.y);
        quad[5].position = m_activeParticles[i].position - halfSize;

        quad[0].color = sf::Color::White;
        quad[1].color = sf::Color::White;
        quad[2].color = sf::Color::White;
        quad[3].color = sf::Color::White;
        quad[4].color = sf::Color::White;
        quad[5].color = sf::Color::White;
    }

    if (!m_vertexBuffer.update(vertices.data(), vertices.size(), 0))
        spdlog::error("Unable to update vertices for Particle Emitter, tried to update with {} vertices",
                      vertices.size());
}

void ParticleEmitter::emitParticle()
{
    Particle p;
    p.maxAliveTime
        = sf::seconds(RNG::RealWithinRange(m_propertiesFileData->particleProperties.lifeTimeMin.asSeconds(),
                                           m_propertiesFileData->particleProperties.lifeTimeMax.asSeconds()));
    m_activeParticles.emplace_back(p);
}
