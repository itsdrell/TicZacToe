#include "GameOver.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "ParticleSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "TicTacToeMain.hpp"
#include "States/Playing.hpp"
#include <algorithm>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Audio/AudioSystem.hpp"
//===============================================================================================
std::string placementText[10] = { "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th" };

std::string aiNames[14] = { "Zac   ", "Lloyd ", "Travis", "Kolbe ", "Artyom", "Austin", "Patryk", "Niko  ", "Chris ", "Feng  ", "Peter ", "Jordan", "Lauren", "Mike  " };

//===============================================================================================
GameOverScreen::GameOverScreen()
{
}

//-----------------------------------------------------------------------------------------------
GameOverScreen::~GameOverScreen()
{

}

//-----------------------------------------------------------------------------------------------
void GameOverScreen::Update(float ds /*= 0.f*/)
{
	g_particleSystem->Update(ds);

	if(m_displayTimer.HasElapsed())
	{
		if(InputSystem::GetInstance()->WasKeyJustPressed(KEYBOARD_ENTER))
		{
			AudioSystem::GetInstance()->PlaySound(AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/place1.wav"), false, 1.f);
			g_game->GoToState(TICTACTOE_ATTRACT);
		}
	}
}

//-----------------------------------------------------------------------------------------------
void GameOverScreen::Render() const
{
	Renderer* r = Renderer::GetInstance();

	g_particleSystem->Render();

	r->DrawAABB2Filled(AABB2(0, 32, 256, 256), Rgba::BLACK);
	
	AABB2 resultTitleBox = AABB2(0, 224, 256, 256);
	r->DrawAABB2Outline(resultTitleBox, Rgba::WHITE);
	r->DrawTextInBox("TOP 10 (W/T/L):", resultTitleBox, 8.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
	
	AABB2 mainBox = AABB2(0, 48, 256, 224);
	r->DrawAABB2Outline(mainBox, Rgba::WHITE);

	// could do a timer where we only go to the to
	int amountToShow = floorf(10.f * m_displayTimer.GetNormalizedElapsedTime());

	float yStep = .1;
	for(int i = 0; i < amountToShow; i++)
	{
		float miny = (yStep * i);
		float maxy = yStep * (i + 1);

		AABB2 currentBounds = GetAABB2FromAABB2(Vector2(0.f, miny), Vector2(1.f, maxy), mainBox);

		int scoreIndex = 9 - i;
		Score currentScore = m_allScores.at(scoreIndex);

		std::string name = GetNameFromScore(currentScore, scoreIndex);
		std::string nameSpace = (i == 0) ? " " : "  ";
		std::string currentDisplayLine = placementText[scoreIndex] + nameSpace + name + " : " + currentScore.GetAsString();
		Rgba color = (currentScore.m_type == PLAYER_SCORE) ? Rgba::GREEN : Rgba::WHITE;

		r->DrawAABB2Outline(currentBounds, Rgba::WHITE);
		r->DrawTextInBox(currentDisplayLine, currentBounds, 7.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(0.f, .5f), color);
	}

	if(m_displayTimer.HasElapsed())
	{
		if(m_playerPlace > 10)
		{
			AABB2 playerPlaceBox = AABB2(0, 24, 256, 48);
			std::string playerText = "You: " + m_playerScore.GetAsString();

			r->DrawAABB2Filled(playerPlaceBox, Rgba::BLACK);
			r->DrawAABB2Outline(playerPlaceBox, Rgba::WHITE);

			r->DrawTextInBox(playerText, playerPlaceBox, 8.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f), Rgba::RED);

			AABB2 continueTextBox = AABB2(0, 0, 256, 24);

			r->DrawAABB2Filled(continueTextBox, Rgba::BLACK);
			r->DrawAABB2Outline(continueTextBox, Rgba::WHITE);

			r->DrawTextInBox("Hit Enter to Continue...", continueTextBox, 5.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
		}
		else
		{
			AABB2 continueTextBox = AABB2(0, 0, 256, 48);

			r->DrawAABB2Filled(continueTextBox, Rgba::BLACK);
			r->DrawAABB2Outline(continueTextBox, Rgba::WHITE);

			r->DrawTextInBox("Hit Enter to Continue...", continueTextBox, 5.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
		}
	}
}

//-----------------------------------------------------------------------------------------------
void GameOverScreen::OnEnter()
{
	Playing* playingState = (Playing*) g_game->m_states[TICTACTOE_PLAYING];
	playingState->GatherAllScores(&m_allScores);
	
	// Sort the top scores?
	std::sort(m_allScores.begin(), m_allScores.end(), [](Score a, Score b)
	{
		float aScore = a.GetTotalScore();
		float bScore = b.GetTotalScore();

		return aScore > bScore;
	});

	FindPlayerScore();
	GenerateTopTenNames();
	
	m_displayTimer.SetTimer(4.f);
	m_displayTimer.Reset();

	g_particleSystem->Reset();

	AudioSystem::GetInstance()->PlaySound(AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/drumroll.wav"), false, 1.f);
}

//-----------------------------------------------------------------------------------------------
void GameOverScreen::OnExit()
{

}

//-----------------------------------------------------------------------------------------------
void GameOverScreen::FindPlayerScore()
{
	for(int i = 0; i < m_allScores.size(); i++)
	{
		Score& currentScore = m_allScores.at(i);

		if(currentScore.m_type == PLAYER_SCORE)
		{
			m_playerPlace = i + 1;
			m_playerScore = currentScore;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void GameOverScreen::GenerateTopTenNames()
{
	std::vector<std::string> namesToChoose(std::begin(aiNames), std::end(aiNames));

	for(int i = 0; i < 10; i++)
	{
		int size = namesToChoose.size();
		int index = GetRandomIntRange(0, size - 1);
		std::string name = namesToChoose.at(index);

		m_topTenNames.push_back(name);

		RemoveFast((uint&) index, namesToChoose);
	}
}

//-----------------------------------------------------------------------------------------------
std::string GameOverScreen::GetNameFromScore(const Score& score, int place) const
{
	if(score.m_type == PLAYER_SCORE)
	{
		return "You   ";
	}

	std::string result = m_topTenNames.at(place);
	return result;
}

