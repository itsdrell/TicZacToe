#include "ParticleSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Vectors/Vector3.hpp"
#include "TicTacToeMain.hpp"
#include "Engine/Renderer/Images/SpriteSheet.hpp"
#include "Engine/Renderer/Images/Sprite.hpp"
#include "Engine/Core/General/EngineCommon.hpp"

//===============================================================================================
Particle::Particle()
{
	m_rotation = GetRandomFloat(0.f, 360);
	m_rotationSpeed = (10.f, 100.f);
	
	m_symbol = (BOARD_SYMBOL) GetRandomIntRange(1, 2);

	m_dropSpeed = GetRandomFloat(10.f, 100.f);

	m_postion = Vector2(GetRandomFloat(-64.f, 312.f), 300.f);

	AABB2 uvs = g_gameSpriteSheet->GetTexCoordsForSpriteIndex(m_symbol - 1);
	m_sprite = Sprite(g_gameSpriteSheet->m_texture, Vector2(16.f, 16.f), uvs);
}

//-----------------------------------------------------------------------------------------------
void Particle::Update(float ds)
{
	m_postion.y -= (m_dropSpeed * ds);

	m_rotation += (m_rotationSpeed * ds);

	if(m_postion.y < -20.f)
	{
		m_isDead = true;
	}
}

//-----------------------------------------------------------------------------------------------
void Particle::Render() const
{
	Renderer* r = Renderer::GetInstance();
	Rgba color = (m_symbol == X_BOARD_SYMBOL) ? Rgba::BLUE : Rgba::RED;

	r->DrawSpriteRotated2D(Vector3(m_postion.x, m_postion.y, 0.f), m_sprite, m_rotation, color);
}

//===============================================================================================
ParticleSystem::ParticleSystem()
{
	m_spawnRates = Vector2(0.f, .25f);
}

//-----------------------------------------------------------------------------------------------
void ParticleSystem::Update(float ds)
{
	if(m_particleSpawnTimer.HasElapsed())
	{
		m_allParticles.push_back(Particle());

		m_particleSpawnTimer.SetTimer(GetRandomFloat(m_spawnRates.x, m_spawnRates.y));
		m_particleSpawnTimer.Reset();
	}

	// update
	for (int i = 0; i < m_allParticles.size(); i++)
	{
		Particle& current = m_allParticles.at(i);

		current.Update(ds);

		if(current.m_isDead)
		{
			RemoveFast((uint&) i, m_allParticles);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void ParticleSystem::Render() const
{
	for (int i = 0; i < m_allParticles.size(); i++)
	{
		const Particle& current = m_allParticles.at(i);

		current.Render();
	}
}

//-----------------------------------------------------------------------------------------------
void ParticleSystem::Reset()
{
	m_allParticles.clear();
}

