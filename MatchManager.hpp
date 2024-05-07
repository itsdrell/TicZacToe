#pragma once
#include "Game/TicTacToe/Board.hpp"
#include "Engine/Core/Tools/StopWatch.hpp"

//===============================================================================================
// Forward Declare
//===============================================================================================
class IController;
class AABB2;
class Score;

//===============================================================================================
// Type Defs + Defines
//===============================================================================================


//===============================================================================================
// ENUMS
//===============================================================================================
enum MATCH_TURN
{
	PLAYER_ONE_TURN,
	PLAYER_TWO_TURN,
	NUM_OF_TURNS
};
std::string MatchTurnToString(MATCH_TURN turn);

//===============================================================================================
// Structs
//===============================================================================================


//===============================================================================================
// Classes
//===============================================================================================
class Match
{
public:
	Match(IController* playerOne, IController* playerTwo, const AABB2& boardBounds, bool isPlayer = false);

public:
	void Update(float ds);
	bool DidPlayerOneLose();

private:
	void ChangeTurns();

public:
	Board			m_board;
	MATCH_TURN		m_currentTurn = PLAYER_ONE_TURN;

private:
	IController*	m_playerOne = nullptr;
	IController*	m_playerTwo = nullptr;

	IController*	m_controllers[NUM_OF_TURNS];
	bool			m_isPlayerMatch = false;
};

//===============================================================================================
class MatchManager
{
public:
	MatchManager(const AABB2& boardBounds, bool playerMatch = false);

public:
	void Update(float ds);
	void Render() const;

private:
	void CreateMatch(const AABB2& boardBounds, bool playerMatch = false);
	void UpdateScores();
	void PlayResultSound();

public:
	Score*		m_playerOneScore = nullptr;
	Score*		m_playerTwoScore = nullptr;
	Match*	m_currentMatch = nullptr;

private:
	AABB2	m_boardBounds;
	bool	m_isPlayerMatch = false;

	StopWatch	m_aiCooldownTimer;
	Vector2		m_aiCooldown = Vector2(.75f, 1.25f);

	StopWatch	m_startNextMatchTimer;
	bool		m_waitedForVictory = false;
	bool		m_playedResultSound = false;
};

//===============================================================================================
// Standalone C Functions
//===============================================================================================


//===============================================================================================
// Externs
//===============================================================================================


//===============================================================================================
// Written by Zachary Bracken : [4/30/2021]
//===============================================================================================