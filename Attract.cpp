#include "Attract.hpp"
#include "TicTacToeMain.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Game/TicTacToe/ParticleSystem.hpp"
#include "States/Playing.hpp"
#include "GameOver.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//===============================================================================================
Attract::Attract()
{

}

//-----------------------------------------------------------------------------------------------
Attract::~Attract()
{

}

//-----------------------------------------------------------------------------------------------
void Attract::Update(float ds /*= 0.f*/)
{
	g_particleSystem->Update(ds);

	if(InputSystem::GetInstance()->WasKeyJustPressed(KEYBOARD_ENTER))
	{
		AudioSystem::GetInstance()->PlaySound(AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/place1.wav"), false, 1.f);
		g_game->GoToState(TICTACTOE_PLAYING);
	}
}

//-----------------------------------------------------------------------------------------------
void Attract::Render() const
{
	g_particleSystem->Render();

	Renderer* r = Renderer::GetInstance();
	
	AABB2 screen = AABB2(0.f, 0.f, 256.f, 256.f);
	AABB2 titleBox = GetAABB2FromAABB2(Vector2(0.f, .4f), Vector2(1.f, .6), screen);

	r->DrawAABB2Filled(titleBox, Rgba::BLACK);
	r->DrawAABB2Outline(titleBox, Rgba::WHITE);
	r->DrawTextInBox("Tic-Zac-Toe", titleBox, 8.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));

	AABB2 enterTextBox = GetAABB2FromAABB2(Vector2(.1f, .25f), Vector2(.9f, .35f), screen);

	r->DrawAABB2Filled(enterTextBox, Rgba::BLACK);
	r->DrawAABB2Outline(enterTextBox, Rgba::WHITE);
	r->DrawTextInBox("Hit Enter to Start!", enterTextBox, 5.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
}

//-----------------------------------------------------------------------------------------------
void Attract::OnEnter()
{
	g_game->Reset();
}

//-----------------------------------------------------------------------------------------------
void Attract::OnExit()
{
	// clear all particles
	g_particleSystem->Reset();
}

