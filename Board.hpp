#pragma once
#include "Engine/Math/Vectors/IntVector2.hpp"
#include "Engine/Math/Geometry/AABB2.hpp"
#include "Engine/Core/General/Rgba.hpp"

//===============================================================================================
// Forward Declare
//===============================================================================================


//===============================================================================================
// Type Defs + Defines
//===============================================================================================
typedef IntVector2 TilePosition;
constexpr int MAX_TILES = 9;

//===============================================================================================
// ENUMS
//===============================================================================================
enum BOARD_SYMBOL
{
	EMPTY_BOARD_SYMBOL,
	X_BOARD_SYMBOL,
	O_BOARD_SYMBOL,
};
BOARD_SYMBOL GetOtherSymbol(BOARD_SYMBOL symbol);

enum BOARD_RESULT
{
	PLAYER_X_WIN,
	PLAYER_O_WIN,
	TIE,
	IN_PROGRESS,
};
std::string GetVictoryTextFromBoardResult(BOARD_RESULT result);

//===============================================================================================
// Classes
//===============================================================================================
class Board
{
public:
	Board() {}
	Board(const AABB2& bounds);

private:
	void CreateUIBounds();

public:
	bool SetTileByIndex(int index, BOARD_SYMBOL symbol);

	bool IsOver() const { return m_state != IN_PROGRESS; }

	void CheckForWinOrTie();
	void CheckForWin();
	void CheckForTie();

	int WhereCanSymbolWinNextTurn(BOARD_SYMBOL symbolToLookFor);
	int GetEmptyTileFromTrio(int a, int b, int c);
	bool CanThreeTilesMakeAWinNextTurn(int a, int b, int c, BOARD_SYMBOL symbolToLookFor);

	bool DoThreeTilesCreateAWin(int a, int b, int c);
	BOARD_RESULT GetWhoWonFromSymbol(BOARD_SYMBOL symbol);
	Rgba GetVictoryColor() const;

public:
	void Render(bool renderInputHints = false) const;

public:
	BOARD_RESULT		m_state = IN_PROGRESS;

	bool				m_wasUpdated = false;
	BOARD_SYMBOL		m_tiles[MAX_TILES] = { EMPTY_BOARD_SYMBOL };

private:
	AABB2				m_bounds;
	AABB2				m_tileBounds[MAX_TILES];

	Rgba				m_xColor = Rgba::BLUE;
	Rgba				m_oColor = Rgba::RED;
	Rgba				m_tieColor = Rgba::BLACK;
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