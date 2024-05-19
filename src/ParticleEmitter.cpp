#include "ParticleEmitter.hpp"
#include "PropertiesFile.hpp"
#include "RNG.hpp"
#include "Utils.hpp"

#include <algorithm>
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
    if (!m_isPlaying)
        return;

    // Emitter update
    m_aliveTime += dt;
    m_spawnTimer += dt;

    if (m_aliveTime > m_propertiesFileData->emitterProperties.lifeTime) {
        if (m_activeParticles.empty()) {
            if (!m_loop) {
                stop();
                return;
            }

            stop();
            play();
        }
    }

    if (m_spawnTimer > m_propertiesFileData->emitterProperties.spawnFrequency
        && m_aliveTime < m_propertiesFileData->emitterProperties.lifeTime) {
        spdlog::debug("Emitting particle, spawn timer is {} and freq is {}",
                      m_spawnTimer.asSeconds(),
                      m_propertiesFileData->emitterProperties.spawnFrequency.asSeconds());
        emitParticle(sf::seconds(0.f));
        const auto difference = m_spawnTimer - m_propertiesFileData->emitterProperties.spawnFrequency;
        const auto missedParticleCount
            = static_cast<i32>(difference / m_propertiesFileData->emitterProperties.spawnFrequency);

        spdlog::debug("Missed count {}", missedParticleCount);
        for (auto i { 1 }; i <= missedParticleCount; ++i)
            emitParticle(sf::seconds(dt.asSeconds() * static_cast<f32>(i)));

        m_spawnTimer = sf::Time::Zero;
    }

    // Update particles
    std::vector<Particle> toRemove;
    toRemove.reserve(m_activeParticles.size());
    for (auto& p : m_activeParticles) {
        p.aliveTime += dt;

        // Find expired particles, we'll skip updating them cause they're going to be purged
        if (p.aliveTime > p.maxAliveTime) {
            toRemove.emplace_back(p);
            continue;
        }

        const auto t = std::clamp(p.aliveTime / p.maxAliveTime, 0.f, 1.f); // Used for lerping

        // If there's a valid acceleration value, we'll apply acceleration to the velocity
        // otherwise we'll lerp the speed value and use the current rotation of the
        // particle as the direction
        if (m_propertiesFileData->particleProperties.acceleration != sf::Vector2f {}) {
            p.velocity += m_propertiesFileData->particleProperties.acceleration * dt.asSeconds();

            // Maximum speed only applies when we're using acceleration
            if (p.velocity.lengthSq() > std::pow(m_propertiesFileData->particleProperties.maximumSpeed, 2.f))
                p.velocity = p.velocity.normalized() * m_propertiesFileData->particleProperties.maximumSpeed;
        } else {
            const auto newSpeed = std::lerp(m_propertiesFileData->particleProperties.speedStart,
                                            m_propertiesFileData->particleProperties.speedEnd,
                                            t);
            p.velocity = sf::Vector2f(newSpeed, p.rotation);
        }

        p.position += p.velocity * dt.asSeconds();

        p.scale = std::lerp(m_propertiesFileData->particleProperties.scaleStart * p.scaleMultiplier,
                            m_propertiesFileData->particleProperties.scaleEnd * p.scaleMultiplier,
                            t);
    }

    // Purge unrequired particles
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
        // TODO: We could probably do this during the update and store the current colour, scale and alpha
        // on the particle itself, thus ensuring reconstructVertices doesn't calculate new values
        // but only constructs the vertices to be ready for rendering
        const auto t = m_activeParticles[i].aliveTime / m_activeParticles[i].maxAliveTime;

        auto quad = &vertices[i * VERTS_PER_QUAD];
        sf::Vector2f halfSize;
        if (m_propertiesFileData->texture) {
            halfSize = sf::Vector2f(m_propertiesFileData->texture->getSize()) * 0.5f;
        } else {
            halfSize = DEFAULT_PARTICLE_SIZE * 0.5f;
        }

        quad[0].position = -halfSize;
        quad[1].position = sf::Vector2f(halfSize.x, -halfSize.y);
        quad[2].position = halfSize;

        quad[3].position = halfSize;
        quad[4].position = sf::Vector2f(-halfSize.x, halfSize.y);
        quad[5].position = -halfSize;

        sf::Transform transform;
        transform = transform.translate(m_activeParticles[i].position);
        transform.scale(sf::Vector2f { m_activeParticles[i].scale, m_activeParticles[i].scale });
        transform = transform.rotate(m_activeParticles[i].rotation);

        quad[0].position = transform * quad[0].position;
        quad[1].position = transform * quad[1].position;
        quad[2].position = transform * quad[2].position;
        quad[3].position = transform * quad[3].position;
        quad[4].position = transform * quad[4].position;
        quad[5].position = transform * quad[5].position;

        const auto colour = LerpColour(m_propertiesFileData->particleProperties.colourStart,
                                       m_propertiesFileData->particleProperties.colourEnd,
                                       t);

        quad[0].color = colour;
        quad[1].color = colour;
        quad[2].color = colour;
        quad[3].color = colour;
        quad[4].color = colour;
        quad[5].color = colour;

        const auto alpha = std::lerp(
            m_propertiesFileData->particleProperties.alphaStart, m_propertiesFileData->particleProperties.alphaEnd, t);

        quad[0].color.a = static_cast<u8>(255.f * alpha);
        quad[1].color.a = static_cast<u8>(255.f * alpha);
        quad[2].color.a = static_cast<u8>(255.f * alpha);
        quad[3].color.a = static_cast<u8>(255.f * alpha);
        quad[4].color.a = static_cast<u8>(255.f * alpha);
        quad[5].color.a = static_cast<u8>(255.f * alpha);
    }

    if (!m_vertexBuffer.update(vertices.data(), vertices.size(), 0))
        spdlog::error("Unable to update vertices for Particle Emitter, tried to update with {} vertices",
                      vertices.size());
}

void ParticleEmitter::emitParticle(sf::Time timeAhead)
{
    Particle p;
    p.maxAliveTime
        = sf::seconds(RNG::RealWithinRange(m_propertiesFileData->particleProperties.lifeTimeMin.asSeconds(),
                                           m_propertiesFileData->particleProperties.lifeTimeMax.asSeconds()));

    // Rotation
    p.rotation
        = sf::degrees(RNG::RealWithinRange(m_propertiesFileData->particleProperties.startRotationMin.asDegrees(),
                                           m_propertiesFileData->particleProperties.startRotationMax.asDegrees()));

    p.velocity = sf::Vector2f(m_propertiesFileData->particleProperties.speedStart, p.rotation);

    p.scale = m_propertiesFileData->particleProperties.scaleStart;

    if (m_propertiesFileData->particleProperties.minimumScaleMultiplier != 1.f) {

        if (m_propertiesFileData->particleProperties.minimumScaleMultiplier > 1.f) {
            p.scaleMultiplier
                = RNG::RealWithinRange(1.f, m_propertiesFileData->particleProperties.minimumScaleMultiplier);
        } else {
            p.scaleMultiplier
                = RNG::RealWithinRange(m_propertiesFileData->particleProperties.minimumScaleMultiplier, 1.f);
        }
    }

    p.aliveTime = timeAhead;
    m_activeParticles.emplace_back(p);
}
