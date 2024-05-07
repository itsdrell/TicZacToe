#include "Board.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "PlayerController.hpp"
#include "TicTacToeMain.hpp"
#include "Engine/Renderer/Images/SpriteSheet.hpp"

//===============================================================================================
BOARD_SYMBOL GetOtherSymbol(BOARD_SYMBOL symbol)
{
	if (symbol == O_BOARD_SYMBOL)
		return X_BOARD_SYMBOL;

	return O_BOARD_SYMBOL;
}

//-----------------------------------------------------------------------------------------------
std::string GetVictoryTextFromBoardResult(BOARD_RESULT result)
{
	if (result == PLAYER_O_WIN)
		return "O's win!";
	if (result == PLAYER_X_WIN)
		return "X's win!";

	return "TIE!!!";
}

//===============================================================================================
Board::Board(const AABB2& bounds)
{
	m_bounds = bounds;

	CreateUIBounds();
}

//-----------------------------------------------------------------------------------------------
void Board::CreateUIBounds()
{
	// 20-40, 40-60, 60-80
	float step = .2;
	for (uint y = 0; y < 3; y++)
	{
		for (uint x = 0; x < 3; x++)
		{
			float minX = (x + 1) * step;
			float minY = (y + 1) * step;
			float maxX = (x + 2) * step;
			float maxY = (y + 2) * step;

			int index = y * 3 + x;
			m_tileBounds[index] = GetAABB2FromAABB2(Vector2(minX, minY), Vector2(maxX, maxY), m_bounds);
		}
	}
}

//-----------------------------------------------------------------------------------------------
bool Board::SetTileByIndex(int index, BOARD_SYMBOL symbol)
{
	if(m_tiles[index] == EMPTY_BOARD_SYMBOL)
	{
		m_tiles[index] = symbol;

		CheckForWinOrTie();

		m_wasUpdated = true;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
void Board::CheckForWinOrTie()
{
	CheckForTie();
	CheckForWin();
}

//-----------------------------------------------------------------------------------------------
void Board::CheckForWin()
{
	// horizontals
	if (DoThreeTilesCreateAWin(0, 1, 2)) { m_state = GetWhoWonFromSymbol(m_tiles[0]); }
	if (DoThreeTilesCreateAWin(3, 4, 5)) { m_state = GetWhoWonFromSymbol(m_tiles[3]); }
	if (DoThreeTilesCreateAWin(6, 7, 8)) { m_state = GetWhoWonFromSymbol(m_tiles[6]); }

	// verticals
	if (DoThreeTilesCreateAWin(0, 3, 6)) { m_state = GetWhoWonFromSymbol(m_tiles[0]); }
	if (DoThreeTilesCreateAWin(1, 4, 7)) { m_state = GetWhoWonFromSymbol(m_tiles[1]); }
	if (DoThreeTilesCreateAWin(2, 5, 8)) { m_state = GetWhoWonFromSymbol(m_tiles[2]); }

	// diagonals
	if (DoThreeTilesCreateAWin(0, 4, 8)) { m_state = GetWhoWonFromSymbol(m_tiles[0]); }
	if (DoThreeTilesCreateAWin(2, 4, 6)) { m_state = GetWhoWonFromSymbol(m_tiles[2]); }
}

//-----------------------------------------------------------------------------------------------
void Board::CheckForTie()
{
	for(int i = 0; i < 9; i++)
	{
		if (m_tiles[i] == EMPTY_BOARD_SYMBOL)
			return;
	}

	m_state = TIE;
}

//-----------------------------------------------------------------------------------------------
int Board::WhereCanSymbolWinNextTurn(BOARD_SYMBOL symbolToLookFor)
{
	int matches[8][3] =
	{
		{0,1,2},
		{3,4,5},
		{6,7,8},

		{0,3,6},
		{1,4,7},
		{2,5,8},

		{0,4,8},
		{2,4,6},
	};

	for(int i = 0; i < 8; i++)
	{
		int a = matches[i][0];
		int b = matches[i][1];
		int c = matches[i][2];

		bool result = CanThreeTilesMakeAWinNextTurn(a, b, c, symbolToLookFor);

		if(result)
		{
			int winningIndex = GetEmptyTileFromTrio(a,b,c);
			return winningIndex;
		}
	}

	// no where!
	return -1;
}

//-----------------------------------------------------------------------------------------------
int Board::GetEmptyTileFromTrio(int a, int b, int c)
{
	BOARD_SYMBOL aSymbol = m_tiles[a];
	BOARD_SYMBOL bSymbol = m_tiles[b];
	BOARD_SYMBOL cSymbol = m_tiles[c];

	if (aSymbol == EMPTY_BOARD_SYMBOL) { return a; }
	if (bSymbol == EMPTY_BOARD_SYMBOL) { return b; }
	if (cSymbol == EMPTY_BOARD_SYMBOL) { return c; }

	// ERROR todo throw???
	return -1;
}

//-----------------------------------------------------------------------------------------------
bool Board::CanThreeTilesMakeAWinNextTurn(int a, int b, int c, BOARD_SYMBOL symbolToLookFor)
{
	BOARD_SYMBOL aSymbol = m_tiles[a];
	BOARD_SYMBOL bSymbol = m_tiles[b];
	BOARD_SYMBOL cSymbol = m_tiles[c];
	
	int amountOfMatches = 0;
	if (aSymbol == symbolToLookFor) { amountOfMatches++; }
	if (bSymbol == symbolToLookFor) { amountOfMatches++; }
	if (cSymbol == symbolToLookFor) { amountOfMatches++; }
	
	int emptyMatches = 0;
	if (aSymbol == EMPTY_BOARD_SYMBOL) { emptyMatches++; }
	if (bSymbol == EMPTY_BOARD_SYMBOL) { emptyMatches++; }
	if (cSymbol == EMPTY_BOARD_SYMBOL) { emptyMatches++; }

	return ((amountOfMatches == 2) && (emptyMatches == 1));
}

//-----------------------------------------------------------------------------------------------
bool Board::DoThreeTilesCreateAWin(int a, int b, int c)
{
	if((m_tiles[a] == EMPTY_BOARD_SYMBOL) ||
		(m_tiles[b] == EMPTY_BOARD_SYMBOL) ||
		(m_tiles[c] == EMPTY_BOARD_SYMBOL))
	{
		return false;
	}

	return ((m_tiles[a] == m_tiles[b]) && (m_tiles[a] == m_tiles[c]));
}

//-----------------------------------------------------------------------------------------------
BOARD_RESULT Board::GetWhoWonFromSymbol(BOARD_SYMBOL symbol)
{
	if (symbol == X_BOARD_SYMBOL)
		return PLAYER_X_WIN;

	return PLAYER_O_WIN;
}

//-----------------------------------------------------------------------------------------------
Rgba Board::GetVictoryColor() const
{
	if (m_state == PLAYER_O_WIN)
		return m_oColor;

	if (m_state == PLAYER_X_WIN)
		return m_xColor;

	return m_tieColor;
}

//-----------------------------------------------------------------------------------------------
void Board::Render(bool renderInputHints) const
{
	Renderer* r = Renderer::GetInstance();

	// DrawLines
	// horizontal
	r->DrawLine2D(m_tileBounds[3].mins, Vector2(m_tileBounds[5].maxs.x, m_tileBounds[5].mins.y), Rgba::WHITE);
	r->DrawLine2D(m_tileBounds[6].mins, Vector2(m_tileBounds[8].maxs.x, m_tileBounds[8].mins.y), Rgba::WHITE);

	// verticals
	r->DrawLine2D(Vector2(m_tileBounds[0].maxs.x, m_tileBounds[0].mins.y), m_tileBounds[6].maxs, Rgba::WHITE);
	r->DrawLine2D(Vector2(m_tileBounds[1].maxs.x, m_tileBounds[1].mins.y), m_tileBounds[7].maxs, Rgba::WHITE);

	r->DrawAABB2Outline(m_bounds, Rgba::WHITE);

	// Draw what's in the tiles
	for(uint i = 0; i < 9; i++)
	{
		AABB2 currentBounds = m_tileBounds[i];
		BOARD_SYMBOL symbol = m_tiles[i];

		if(symbol != EMPTY_BOARD_SYMBOL)
		{
			Rgba color = (symbol == X_BOARD_SYMBOL) ? m_xColor : m_oColor;

			AABB2 uvs = g_gameSpriteSheet->GetTexCoordsForSpriteIndex(symbol - 1);

			r->SetCurrentTexture(0, g_gameSpriteSheet->m_texture);
			r->DrawTexturedAABB2(currentBounds, *g_gameSpriteSheet->m_texture, uvs.mins, uvs.maxs, color);
			r->SetCurrentTexture();
		}
		else
		{
			if(renderInputHints)
			{
				char letter = PlayerController::s_letterInputs[i];
				AABB2 hintBounds = GetAABB2FromAABB2(Vector2(.7f, 0.f), Vector2(1.f, .3f), currentBounds);
				r->DrawTextInBox(std::string(1, letter), hintBounds, 8, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
			}
		}
	}

	if(IsOver())
	{
		AABB2 victoryScreen = GetAABB2FromAABB2(Vector2(0.f, .4f), Vector2(1.f, .6f), m_bounds);
		Rgba victoryColor = GetVictoryColor();
		std::string victoryText = GetVictoryTextFromBoardResult(m_state);

		r->DrawAABB2Filled(victoryScreen, victoryColor);
		r->DrawAABB2Outline(victoryScreen, Rgba::WHITE);
		r->DrawTextInBox(victoryText, victoryScreen, 8.f, 1.f, DRAW_TEXT_MODE_SHRINKED, Vector2(.5f, .5f));
	}
}

