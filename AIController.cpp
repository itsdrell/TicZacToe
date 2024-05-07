#include "AIController.hpp"
#include "Engine/Math/MathUtils.hpp"

//===============================================================================================
std::vector<AIBehavior*> AIController::s_behaviors;

//===============================================================================================
void AIBehavior::MakeRandomPriority()
{
	std::vector<int> used;

	while (used.size() < MAX_TILES)
	{
		int randomIndex = GetRandomIntRange(0, MAX_TILES - 1);
		auto findIterator = std::find(used.begin(), used.end(), randomIndex);

		if (findIterator == used.end())
		{
			used.push_back(randomIndex);
		}
	}

	m_priority = new int[MAX_TILES] { used[0], used[1], used[2], used[3], used[4], used[5], used[6], used[7], used[8] };
}

//===============================================================================================
AIController::AIController()
{
	int behaviorIndex = GetRandomIntRange(0, s_behaviors.size() - 1);
	m_behavior = s_behaviors.at(behaviorIndex);

	if (m_behavior->m_name == "random")
	{
		m_behavior->MakeRandomPriority();
	}
}

//-----------------------------------------------------------------------------------------------
void AIController::Update(Board& theBoard, float ds)
{
	// Win if I can
	int myWinLocation = theBoard.WhereCanSymbolWinNextTurn(m_symbol);
	if (myWinLocation != -1 && (Chance(10.f) == false)) // add some chance to mess up
	{
		theBoard.SetTileByIndex(myWinLocation, m_symbol);
		return;
	}
	
	// Prevent my opponent from winning
	int opponentWinLocation = theBoard.WhereCanSymbolWinNextTurn(GetOtherSymbol(m_symbol));
	if(opponentWinLocation != -1 && (Chance(10.0f) == false)) // add some chance to mess up and not block
	{
		theBoard.SetTileByIndex(opponentWinLocation, m_symbol);
		return;
	}

	// follow my priority
	for(int i = 0; i < MAX_TILES; i++)
	{
		int indexToTry = m_behavior->m_priority[i];
		if(theBoard.SetTileByIndex(indexToTry, m_symbol))
		{
			return;
		}
	}
}

//===============================================================================================
void CreateAllAIBehaviors()
{
	AIBehavior* behavior = new AIBehavior();
	behavior->m_name = "random";
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "crosses";
	behavior->m_priority = new int[MAX_TILES]{ 4,0,2,6,8,1,5,3,7 };
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "crosses";
	behavior->m_priority = new int[MAX_TILES] { 4, 0, 2, 6, 8, 1, 5, 3, 7 };
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "mickeyLeft";
	behavior->m_priority = new int[MAX_TILES] { 4, 0, 6, 2, 8, 1, 5, 3, 7 };
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "L";
	behavior->m_priority = new int[MAX_TILES] { 6,2,0,8,1,3,7,5,4 };
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "InvertedL";
	behavior->m_priority = new int[MAX_TILES] { 6,2,8,7,5,0,3,4,1 };
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "bottomLeftSide";
	behavior->m_priority = new int[MAX_TILES] { 3,1,0,2,6,4,7,8,5 };
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "topLeftSide";
	behavior->m_priority = new int[MAX_TILES] { 3,7,6,4,8,0,1,2,5 };
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "bottomRightSide";
	behavior->m_priority = new int[MAX_TILES] { 5,1,2,4,0,8,6,7,3 };
	AIController::s_behaviors.push_back(behavior);

	behavior = new AIBehavior();
	behavior->m_name = "topRightSide";
	behavior->m_priority = new int[MAX_TILES] { 5,7,8,4,2,6,0,1,3 };
	AIController::s_behaviors.push_back(behavior);
}

//-----------------------------------------------------------------------------------------------
void DeleteAllAIBehaviors()
{
	std::vector<AIBehavior*>& behaviors = AIController::s_behaviors;

	for(int i = 0; i < behaviors.size(); i++)
	{
		AIBehavior* current = behaviors.at(i);

		delete current->m_priority;
		delete current;
	}

	behaviors.clear();
}
