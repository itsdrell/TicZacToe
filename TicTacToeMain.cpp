#include "TicTacToeMain.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/Renderer/Images/SpriteSheet.hpp"

#include "Game/TicTacToe/States/Playing.hpp"
#include "Game/TicTacToe/AIController.hpp"
#include "GameOver.hpp"
#include "Attract.hpp"
#include "ParticleSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//===============================================================================================
SpriteSheet* g_gameSpriteSheet = nullptr;
TicTacToeMain* g_game = nullptr;
ParticleSystem* g_particleSystem = nullptr;

//===============================================================================================
TicTacToeMain::TicTacToeMain()
{
	Texture* spriteSheetTexture = Renderer::GetInstance()->CreateOrGetTexture("Data/SpriteSheet.png");
	g_gameSpriteSheet = new SpriteSheet(spriteSheetTexture, 16, 16);
	g_particleSystem = new ParticleSystem();

	CreateAllAIBehaviors();
	
	m_states[TICTACTOE_ATTRACT] = (IAppState*) new Attract();

	AudioSystem::GetInstance()->PlaySound(AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/bg.mp3"), true, .3f);
	
	g_game = this;

	m_states[m_currentState]->OnEnter();

}

//-----------------------------------------------------------------------------------------------
TicTacToeMain::~TicTacToeMain()
{
	delete m_states[TICTACTOE_PLAYING];
	m_states[TICTACTOE_PLAYING] = nullptr;

	delete g_gameSpriteSheet;
	g_gameSpriteSheet = nullptr;

	delete g_particleSystem;
	g_particleSystem = nullptr;

	DeleteAllAIBehaviors();
}

//-----------------------------------------------------------------------------------------------
void TicTacToeMain::Update(float ds)
{
	m_states[m_currentState]->Update(ds);
}

//-----------------------------------------------------------------------------------------------
void TicTacToeMain::Render() const
{
	m_states[m_currentState]->Render();
}

//-----------------------------------------------------------------------------------------------
void TicTacToeMain::GoToState(TICTACTOE_STATES state)
{
	m_states[m_currentState]->OnExit();
	m_states[state]->OnEnter();

	m_currentState = state;
}

//-----------------------------------------------------------------------------------------------
void TicTacToeMain::Reset()
{
	if(g_game->m_states[TICTACTOE_PLAYING] != nullptr)
	{
		delete g_game->m_states[TICTACTOE_PLAYING];
	}

	if (g_game->m_states[TICTACTOE_ENDSCREEN] != nullptr)
	{
		delete g_game->m_states[TICTACTOE_ENDSCREEN];
	}
	
	m_states[TICTACTOE_PLAYING] = (IAppState*) new Playing();
	m_states[TICTACTOE_ENDSCREEN] = (IAppState*) new GameOverScreen();
}

//===============================================================================================
void GoToState(TICTACTOE_STATES state)
{
	g_game->GoToState(state);
}


