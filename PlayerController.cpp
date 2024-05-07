#include "PlayerController.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//===============================================================================================
char PlayerController::s_letterInputs[9] = { 'z', 'x', 'c', 'a', 's', 'd', 'q', 'w', 'e' };

//===============================================================================================
PlayerController::PlayerController()
{

}

//-----------------------------------------------------------------------------------------------
void PlayerController::Update(Board& theBoard, float ds)
{
	InputSystem* input = InputSystem::GetInstance();

	for(int i = 0; i < 9; i++)
	{
		char letterKey = s_letterInputs[i];
		char numPadKey = std::to_string(i + 1)[0];

		if (input->WasKeyJustReleased(letterKey) || input->WasKeyJustReleased(numPadKey))
		{

			bool result = theBoard.SetTileByIndex(i, m_symbol);

			if(result)
				AudioSystem::GetInstance()->PlaySound(AudioSystem::GetInstance()->CreateOrGetSound("Data/Audio/place1.wav"), false, 1.f);
			
			return;
		}
	}
}

void PlayerController::UpdateInput()
{

}
