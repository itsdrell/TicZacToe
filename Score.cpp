#include "Score.hpp"

//===============================================================================================
Score::Score(bool isPlayer /*= false*/)
{
	m_type = (isPlayer) ? PLAYER_SCORE : AI_SCORE;
}

//-----------------------------------------------------------------------------------------------
float Score::GetTotalScore()
{
	float winTotal = m_wins * 5;
	float losesTotal = m_loses * -2.5;
	float tieTotal = m_ties * .25;
	
	return winTotal + losesTotal + tieTotal;
}

//-----------------------------------------------------------------------------------------------
std::string Score::GetAsString() const
{
	return std::to_string(m_wins) + "/" + std::to_string(m_ties) + "/" + std::to_string(m_loses);
}
