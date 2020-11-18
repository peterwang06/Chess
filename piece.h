#pragma once
#include <string>
#include <utility>
#include <math.h>
#include <vector>
#include "chess_board.h"

class chess_board;
class piece {
protected:
	friend chess_board;
	chess_board* board;
	std::string type;
	int row;
	int col;
	char colour;
	bool captured;
	bool moved;
	piece* enPass;
	bool leftCastle;
	bool rightCastle;
	std::vector <int> moves;
public:
	piece(piece* old, chess_board* board);
	piece(std::string type_, int row_, int col_, char colour_, chess_board* board_);
	virtual ~piece();

	std::string getType();
	void setType(std::string setType);

	std::pair <int, int> getPosition();
	void setPosition(int setRow, int setCol);

	char getColour();
	void setColour(char setColour);

	bool getCaptured();
	void setCaptured(bool state);

	bool getMoved();
	void setMoved(bool state);

	piece* getEP();
	void setEP(piece* setEP);

	bool getLC();
	void setLC(bool state);

	bool getRC();
	void setRC(bool state);

	void setPieceEmpty();

	bool testMove(int gridRow, int gridCol, int newRow, int newCol, std::string direction);
	bool testMove2(int gridRow, int gridCol, int newRow, int newCol, std::string direction);
	void testMoveDiagonal(int row, int col);
	void testMoveCardinal(int row, int col);

	void getMove();
	void getMoveCheck(int newRow, int newCol);
	bool bound(int row, int col);
	bool validMove(int newRow, int newCol, bool modify);
	bool kingValidMove(int newRow, int newCol, bool draw);
	bool pawnValidMove(int newRow, int newCol, bool draw);
	bool queenValidMove(int newRow, int newCol);
	bool emptyValidMove(int newRow, int newCol);
	bool bishopValidMove(int newRow, int newCol);
	bool knightValidMove(int newRow, int newCol);
	bool rookValidMove(int newRow, int newCol);
};