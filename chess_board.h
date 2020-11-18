#pragma once
#include "piece.h"
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
class piece;

enum PIECES { QUEEN = 0, KING = 1, ROOK = 2, KNIGHT = 3, BISHOP = 4, PAWN = 5 };

class chess_board {
public:
	chess_board(sf::Sprite blackSprites_[], sf::Sprite whiteSprites_[], float sizeUnit_, float offSet_);
	chess_board(chess_board* old, bool window);
	~chess_board();
	float sizeUnit;
	float offSet;
	sf::Sprite* blackSprites;
	sf::Sprite* whiteSprites;

	piece* grid[8][8];	//grid state of board
	std::vector<piece*> white;	//array containing a pointer to all of white's pieces
	std::vector<piece*> black;	//array containing a pointer to all of black's pieces
	std::vector<std::string> whiteCaptured;
	std::vector<std::string> blackCaptured;

	std::vector <int> blackMoves;
	std::vector <int> whiteMoves;
	char turn;
	char ai;
	char playerTwo;
	bool window;
	int promotionType;
	bool moveFX;
	bool winFX;

	piece* pieceFromPosition(int row, int col);			//find the name of a piece from a position
	std::pair <int, int> positionFromName(std::string name, std::vector<piece*> colour);	//find the position of a piece

	void moveToCaptured(piece* piece_);

	bool check(char colour);												//determine if check for a colour
	bool checkPosition(char colour, int row, int col);						//determine if position is in check

	char getTurn();
	void switchTurn();
	void endTurn();
	void move(int row1, int col1, int row2, int col2);
	void swap(int row1, int col1, int row2, int col2);
	void promotionCheck();
	std::string promotion_window();

	bool moveCheckDraw(int row1, int col1, int row2, int col2);
	bool moveCheckAll(int row1, int col1, int row2, int col2);
	void calculateAllMoves();
	bool moveCheck(int row1, int col1, int row2, int col2);
	void clearAllMoves();
	void checkmate();

	int pairConversion(int row1, int col1, int row2, int col2);
	std::pair< std::pair <int, int>, std::pair <int, int>> intConversion(int key);

	void calculateColourMoves(char colour);
	int evaluatePosition(char max);
	int evaluatePiece(piece* pce);
	bool aiTurn();

	int alphaBetaMax(int alpha, int beta, int depthleft, int * counter);
	int alphaBetaMin(int alpha, int beta, int depthleft, int * counter);

	int stringToEnum(std::string input);
	std::string enumToString(int input);
};