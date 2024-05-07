#include "Playing.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Game/TicTacToe/MatchManager.hpp"
#include "Game/TicTacToe/PlayerController.hpp"
#include "Game/TicTacToe/AIController.hpp"
#include "Game/TicTacToe/Score.hpp"
#include "Engine/Core/Tools/StopWatch.hpp"
#include "../TicTacToeMain.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Tools/DebugRendering.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//===============================================================================================
Playing::Playing()
{
}

//-----------------------------------------------------------------------------------------------
Playing::~Playing()
{
	delete m_mainManager;
	m_mainManager = nullptr;

	for(int i = 0; i < m_aiMatches.size(); i++)
	{
		MatchManager* current = m_aiMatches.at(i);
		delete current;
	}
	m_aiMatches.clear();
}

//-----------------------------------------------------------------------------------------------
void Playing::Update(float ds /*= 0.f*/)
{
	if(m_gameStartTimer.HasElapsed() == false)
	{
		return;
	}
	else
	{
		if(m_countdownOver == false)
		{
			m_countdownOver = true;

			AudioSystem::GetInstance()->PlaySound(AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/start.wav"), false, 1.f);
		}
	}
	
	if(m_gameOverTimer.HasElapsed() == false)
	{
		m_mainManager->Update(ds);

		for (int i = 0; i < m_aiMatches.size(); i++)
		{
			MatchManager*& current = m_aiMatches.at(i);
			current->Update(ds);
		}
	}
	else
	{
		if(m_startedGameOver == false)
		{
			m_changeToGameOverTimer.SetTimer(3.f);
			m_changeToGameOverTimer.Reset();

			AudioSystem::GetInstance()->PlaySound(AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/timeup.wav"), false, 1.f);

			m_startedGameOver = true;
		}

		if(m_changeToGameOverTimer.HasElapsed())
		{
			GoToState(TICTACTOE_ENDSCREEN);
			m_startedGameOver = false;
		}
	}

}

//-----------------------------------------------------------------------------------------------
void Playing::Render() const
{
	Renderer* r = Renderer::GetInstance();

	m_mainManager->Render();
	r->DrawAABB2Outline(AABB2(0, 0, 256, 256));

	// Draw Score
	AABB2 scoreBox = AABB2(0, 0, 256, 32);
	std::string scoreText = "Score (W/T/L): " + m_mainManager->m_playerOneScore->GetAsString();

	r->DrawAABB2Outline(scoreBox, Rgba::WHITE);
	r->DrawTextInBox(scoreText, scoreBox, 6.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));

	// Draw whose turn for main match
	AABB2 turnBox = AABB2(0, 224, 192, 256);
	std::string turnText = MatchTurnToString(m_mainManager->m_currentMatch->m_currentTurn);

	r->DrawTextInBox(turnText, turnBox, 6.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));

	if(m_gameOverTimer.HasElapsed() == false && m_gameStartTimer.HasElapsed() == true)
	{
		// Draw Timer
		AABB2 timerBox = AABB2(192, 224, 256, 256);
		int timeLeftAsInt = floorf(m_gameOverTimer.GetElapsedTime());
		std::string timeLeft = std::to_string(timeLeftAsInt);

		r->DrawTextInBox(timeLeft, timerBox, 6.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
		r->DrawAABB2Outline(timerBox, Rgba::WHITE);
	}

	for each(MatchManager* manager in m_aiMatches)
	{
		manager->Render();
	}

	if(m_gameOverTimer.HasElapsed())
	{
		AABB2 timeUpBanner = AABB2(-64, 0, 320, 256);
		timeUpBanner = GetAABB2FromAABB2(Vector2(0.f, .4f), Vector2(1.f, .6f), timeUpBanner);

		r->DrawAABB2Filled(timeUpBanner, Rgba::BLACK);
		r->DrawAABB2Outline(timeUpBanner, Rgba::WHITE);
		r->DrawTextInBox("TIME's UP!", timeUpBanner, 8.f, m_changeToGameOverTimer.GetNormalizedElapsedTime() * 2.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
	}

	if(m_gameStartTimer.HasElapsed() == false)
	{
		float degrees = (m_gameStartTimer.GetNormalizedElapsedTime() * 3) * 360;
		float t = ClampFloat(CosDegrees(degrees), 0.f, 1.f);
		
		float minsLerp = Interpolate(.1f, .2f, t);
		float maxsLerp = Interpolate(.9f, .8f, t);

		AABB2 countDownBox = GetAABB2FromAABB2(Vector2(minsLerp, minsLerp), Vector2(maxsLerp, maxsLerp), AABB2(0.f, 0.f, 256.f, 256.f));
		
		int number = ceilf(m_gameStartTimer.GetElapsedTime());
		std::string timerText = std::to_string(number);

		r->DrawAABB2Filled(countDownBox, Rgba::BLACK);
		r->DrawAABB2Outline(countDownBox, Rgba::WHITE);
		r->DrawTextInBox(timerText, countDownBox, 1000.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
	}
}

//-----------------------------------------------------------------------------------------------
void Playing::OnEnter()
{
	CreateAllMatches();

	m_gameOverTimer.SetTimer(63.f);
	m_gameStartTimer.SetTimer(3.f);
}

//-----------------------------------------------------------------------------------------------
void Playing::OnExit()
{

}

//-----------------------------------------------------------------------------------------------
void Playing::GatherAllScores(std::vector<Score>* outScores)
{
	outScores->push_back(*m_mainManager->m_playerOneScore);
	
	// if you do really bad they can be in first
	//outScores->push_back(*m_mainManager->m_playerTwoScore);

	for (int i = 0; i < m_aiMatches.size(); i++)
	{
		MatchManager*& current = m_aiMatches.at(i);
		
		outScores->push_back(*current->m_playerOneScore);
		outScores->push_back(*current->m_playerTwoScore);
	}
}

//-----------------------------------------------------------------------------------------------
void Playing::CreateAllMatches()
{
	AABB2 mainBox = AABB2(0, 32, 256, 224);
	m_mainManager = new MatchManager(mainBox, true);

	// 10 collumns of 3 rows
	float xStep = 64.f / 3.f;
	float yStep = 256.f / 10.f;

	// left side
	float startPosX = -64.f;
	for(int y = 0; y < 10; y++)
	{
		for(int x = 0; x < 3; x++)
		{
			float minx = startPosX + (x * xStep);
			float miny = 0 + (y * yStep);
			float maxx = startPosX + ((x + 1) * xStep);
			float maxy = 0 + ((y + 1) * yStep);

			AABB2 bounds = AABB2(minx, miny, maxx, maxy);
			MatchManager* newMatch = new MatchManager(bounds);

			m_aiMatches.push_back(newMatch);
		}
	}

	// right side
	startPosX = 256.f;
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			float minx = startPosX + (x * xStep);
			float miny = 0 + (y * yStep);
			float maxx = startPosX + ((x + 1) * xStep);
			float maxy = 0 + ((y + 1) * yStep);

			AABB2 bounds = AABB2(minx, miny, maxx, maxy);
			MatchManager* newMatch = new MatchManager(bounds);

			m_aiMatches.push_back(newMatch);
		}
	}
}

