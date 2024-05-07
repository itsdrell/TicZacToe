#include "MatchManager.hpp"
#include "Game/TicTacToe/IController.hpp"
#include "Engine/Core/Tools/DebugRendering.hpp"
#include "PlayerController.hpp"
#include "AIController.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Score.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//==============================================================================================
std::string MatchTurnToString(MATCH_TURN turn)
{
	if (turn == PLAYER_ONE_TURN)
		return "Player One's Turn";

	return "Player Two's Turn";
}


//===============================================================================================
Match::Match(IController* playerOne, IController* playerTwo, const AABB2& boardBounds, bool isPlayer)
{
	m_controllers[PLAYER_ONE_TURN] = playerOne;
	m_controllers[PLAYER_TWO_TURN] = playerTwo;

	// maybe randomize?
	playerOne->m_symbol = X_BOARD_SYMBOL;
	playerTwo->m_symbol = O_BOARD_SYMBOL;

	m_currentTurn = (MATCH_TURN) GetRandomIntRange(0, 1);

	m_board = Board(boardBounds);
	m_isPlayerMatch = isPlayer;
}

//-----------------------------------------------------------------------------------------------
void Match::Update(float ds)
{
	if(m_board.IsOver() == false)
	{
		if (m_board.m_wasUpdated)
		{
			ChangeTurns();
			m_board.m_wasUpdated = false;

			//if(m_isPlayerMatch)
			//	AudioSystem::GetInstance()->PlaySound(AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/place1.wav"), false, 1.f);
		}
		else
		{
			m_controllers[m_currentTurn]->Update(m_board, ds);
		}
	}
	else
	{
		//DebugRenderLog("MATCH OVER", 5.f);
	}
}

//-----------------------------------------------------------------------------------------------
bool Match::DidPlayerOneLose()
{
	if ((m_controllers[PLAYER_ONE_TURN]->m_symbol == X_BOARD_SYMBOL) && (m_board.m_state == PLAYER_O_WIN))
		return true;

	if ((m_controllers[PLAYER_ONE_TURN]->m_symbol == O_BOARD_SYMBOL) && (m_board.m_state == PLAYER_X_WIN))
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------
void Match::ChangeTurns()
{
	if(m_currentTurn == PLAYER_ONE_TURN)
	{
		m_currentTurn = PLAYER_TWO_TURN;
		return;
	}

	m_currentTurn = PLAYER_ONE_TURN;
}

//===============================================================================================
MatchManager::MatchManager(const AABB2& boardBounds, bool playerMatch /*= false*/)
{
	m_boardBounds = boardBounds;
	m_isPlayerMatch = playerMatch;

	m_playerOneScore = new Score(playerMatch);
	m_playerTwoScore = new Score();

	CreateMatch(boardBounds, playerMatch);
}

//-----------------------------------------------------------------------------------------------
void MatchManager::Update(float ds)
{
	if(m_isPlayerMatch == false)
	{
		if (m_aiCooldownTimer.HasElapsed())
		{
			m_currentMatch->Update(ds);

			m_aiCooldownTimer.SetTimer(GetRandomFloat(m_aiCooldown.x, m_aiCooldown.y));
			m_aiCooldownTimer.Reset();
		}
	}
	else
	{
		m_currentMatch->Update(ds);
	}

	if(m_currentMatch->m_board.IsOver())
	{
		if(m_isPlayerMatch && m_playedResultSound == false)
		{
			PlayResultSound();

			m_playedResultSound = true;
		}
		
		if(m_waitedForVictory == false)
		{
			m_startNextMatchTimer.SetTimer(1.f);
			m_startNextMatchTimer.Reset();

			m_waitedForVictory = true;
		}
		else
		{
			if(m_startNextMatchTimer.HasElapsed())
			{
				m_waitedForVictory = false;
				m_playedResultSound = false;

				UpdateScores();
				CreateMatch(m_boardBounds, m_isPlayerMatch);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void MatchManager::Render() const
{
	m_currentMatch->m_board.Render(m_isPlayerMatch);
}

//-----------------------------------------------------------------------------------------------
void MatchManager::CreateMatch(const AABB2& boardBounds, bool playerMatch /*= false*/)
{
	if(m_currentMatch != nullptr)
	{
		delete m_currentMatch;
		m_currentMatch = nullptr;
	}
	
	if (playerMatch)
	{
		m_currentMatch = new Match(new PlayerController(), new AIController(), boardBounds, playerMatch);
	}
	else
	{
		m_currentMatch = new Match(new AIController(), new AIController(), boardBounds);
		m_aiCooldownTimer.SetTimer(GetRandomFloat(m_aiCooldown.x, m_aiCooldown.y));
	}
}

//-----------------------------------------------------------------------------------------------
void MatchManager::UpdateScores()
{
	if(m_currentMatch->m_board.m_state == TIE)
	{
		m_playerOneScore->m_ties++;
		m_playerTwoScore->m_ties++;

		return;
	}

	if(m_currentMatch->DidPlayerOneLose())
	{
		m_playerOneScore->m_loses++;
		m_playerTwoScore->m_wins++;
	}
	else
	{
		m_playerOneScore->m_wins++;
		m_playerTwoScore->m_loses++;
	}
}

//-----------------------------------------------------------------------------------------------
void MatchManager::PlayResultSound()
{
	AudioSystem* audio = AudioSystem::GetInstance();
	
	if (m_currentMatch->m_board.m_state == TIE)
	{
		audio->PlaySound(audio->CreateOrGetSound("Data/Audio/tie.wav"), false, 1.f);
		return;
	}

	if (m_currentMatch->DidPlayerOneLose())
	{
		audio->PlaySound(audio->CreateOrGetSound("Data/Audio/loss.wav"), false, 1.f);
	}
	else
	{
		audio->PlaySound(audio->CreateOrGetSound("Data/Audio/win1.wav"), false, 1.f);
	}
}
