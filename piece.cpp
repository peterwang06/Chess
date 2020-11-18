#include "piece.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define new new
#endif

piece::piece(piece* old, chess_board* board_) {
	captured = old->captured;
	colour = old->colour;
	enPass = old->enPass;
	moved = old->moved;
	row = old->row;
	col = old->col;
	type = old->type;
	leftCastle = old->leftCastle;
	rightCastle = old->rightCastle;

	board = board_;
}

piece::piece(std::string type_, int row_, int col_, char colour_, chess_board* board_) {
	type = type_;
	row = row_;
	col = col_;
	colour = colour_;
	captured = false;
	moved = false;
	enPass = nullptr;
	leftCastle = false;
	rightCastle = false;

	board = board_;
}

piece::~piece() {
}

std::string piece::getType() {
	return type;
}
void piece::setType(std::string setType) {
	type = setType;
}

void piece::setColour(char setColour) {
	colour = setColour;
}
char piece::getColour() {
	return colour;
}

std::pair <int, int> piece::getPosition() {
	return  std::make_pair(row, col);
}
void piece::setPosition(int newRow, int newCol) {
	if ((newRow >= 0 && newRow < 8) && (newCol >= 0 && newCol < 8)) {
		row = newRow;
		col = newCol;
		return;
	}
}

bool piece::getCaptured() {
	return captured;
}
void piece::setCaptured(bool state) {
	captured = state;
}

bool piece::getMoved() {
	return moved;
}
void piece::setMoved(bool state) {
	moved = state;
}

piece* piece::getEP() {
	return enPass;
}
void piece::setEP(piece* piece_) {
	enPass = piece_;
}

bool piece::validMove(int newRow, int newCol, bool modify) {
	if (!(newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)) {
		return false;
	}
	if (type == "pawn") {
		return pawnValidMove(newRow, newCol, modify);
	}
	else if (type == "empty") {
		return emptyValidMove(newRow, newCol);
	}
	else if (type == "king") {
		return kingValidMove(newRow, newCol, modify);
	}
	else if (type == "rook") {
		return rookValidMove(newRow, newCol);
	}
	else if (type == "queen") {
		return queenValidMove(newRow, newCol);
	}
	else if (type == "bishop") {
		return bishopValidMove(newRow, newCol);
	}
	else if (type == "knight") {
		return knightValidMove(newRow, newCol);
	}
	return false;
}

bool piece::bishopValidMove(int newRow, int newCol) {
	if (col == newCol && row == newRow) {
		return false;
	}
	else if (board->pieceFromPosition(newRow, newCol)->getColour() == colour) {
		return false;
	}
	else if (abs(newRow - row) == abs(newCol - col)) {
		if ((newRow - row) < 0) {
			if ((newCol - col) > 0) {
				return testMove(row, col, newRow, newCol, "se");
			}
			else {
				return testMove(row, col, newRow, newCol, "sw");
			}
		}
		else {
			if ((newCol - col) > 0) {
				return testMove(row, col, newRow, newCol, "ne");
			}
			else {
				return testMove(row, col, newRow, newCol, "nw");
			}
		}
	}
	else return false;
}

bool piece::kingValidMove(int newRow, int newCol, bool modify) {
	if (col == newCol && row == newRow) {
		return false;
	}
	else if (board->pieceFromPosition(newRow, newCol)->getColour() == colour) {
		return false;
	}
	else if ((abs(newRow - row) == 1 && abs(newCol - col) == 1) ||
		(abs(newCol - col) == 1 && abs(newRow - row) == 0) ||
		(abs(newCol - col) == 0 && abs(newRow - row) == 1)
		) {
		return true;
	}
	if (newRow == 0 && newCol == 2 && colour == 'w') {//white queenside castle
		if (board->grid[0][1]->getType() == "empty" && board->grid[0][2]->getType() == "empty"
			&& board->grid[0][3]->getType() == "empty") {
			if (board->checkPosition('w', 0, 2) == false && board->checkPosition('w', 0, 3) == false && board->check('w') == false && moved == false) {
				if (board->pieceFromPosition(0, 0)->getType() == "rook" && board->pieceFromPosition(0, 0)->getMoved() == false) {
					if (modify == true) {
						leftCastle = true;
					}
					return true;
				}
			}
		}
	}
	else if (newRow == 0 && newCol == 6 && colour == 'w') {//white kingside castle
		if (board->grid[0][5]->getType() == "empty" && board->grid[0][6]->getType() == "empty") {
			if (board->checkPosition('w', 0, 5) == false && board->checkPosition('w', 0, 6) == false && board->check('w') == false && moved == false) {
				if (board->pieceFromPosition(0, 7)->getType() == "rook" && board->pieceFromPosition(0, 7)->getMoved() == false) {
					if (modify == true) {
						rightCastle = true;
					}
					return true;
				}
			}
		}
	}
	else if (newRow == 7 && newCol == 2 && colour == 'b') {//black queenside castle
		if (board->grid[7][1]->getType() == "empty" && board->grid[7][2]->getType() == "empty" && board->grid[7][3]->getType() == "empty")
			if (board->checkPosition('b', 7, 2) == false && board->checkPosition('b', 7, 3) == false && board->check('b') == false && moved == false) {
				if (board->pieceFromPosition(7, 0)->getType() == "rook" && board->pieceFromPosition(7, 0)->getMoved() == false) {
					if (modify == true) {
						leftCastle = true;
					}
					return true;
				}
			}
	}
	else if (newRow == 7 && newCol == 6 && colour == 'b') {//black kingside castle
		if (board->grid[7][5]->getType() == "empty" && board->grid[7][6]->getType() == "empty")
			if (board->checkPosition('b', 7, 5) == false && board->checkPosition('b', 7, 6) == false && board->check('b') == false && moved == false) {
				if (board->pieceFromPosition(7, 7)->getType() == "rook" && board->pieceFromPosition(7, 7)->getMoved() == false) {
					if (modify == true) {
						rightCastle = true;
					}
					return true;
				}
			}
	}
	return false;
}

bool piece::emptyValidMove(int newRow, int newCol) {
	return false;
}

bool piece::pawnValidMove(int newRow, int newCol, bool modify) {
	if (col == newCol && row == newRow) {
		return false;
	}
	else if (board->pieceFromPosition(newRow, newCol)->getColour() == colour) {
		return false;
	}
	else if (colour == 'w') {
		if (moved == false && newCol == col && newRow == row + 2 && board->pieceFromPosition(row + 2, col)->getColour() == 'e'
			&& board->pieceFromPosition(row + 1, col)->getColour() == 'e') {
			if (modify == true) {
				if (col != 0 && board->grid[row + 2][col - 1]->getType() == "pawn") {
					board->grid[row + 2][col - 1]->setEP(board->grid[row][col]);
				}
				if (col != 7 && board->grid[row + 2][col + 1]->getType() == "pawn") {
					board->grid[row + 2][col + 1]->setEP(board->grid[row][col]);
				}
			}
			return true;
		}
		else if (newCol == col && newRow == row + 1 && board->pieceFromPosition(row + 1, col)->getColour() == 'e') {
			return true;
		}
		else if (col != 7 && newRow == row + 1 && newCol == col + 1 && board->pieceFromPosition(row + 1, col + 1)->getColour() == 'b') {
			return true;
		}
		else if (col != 0 && newRow == row + 1 && newCol == col - 1 && board->pieceFromPosition(row + 1, col - 1)->getColour() == 'b') {
			return true;
		}
		else if (enPass != nullptr) {
			if (newRow == enPass->getPosition().first + 1 && newCol == enPass->getPosition().second) {
				return true;
			}
		}
		else return false;
	}
	else if (colour == 'b') {
		if (moved == false && newCol == col && newRow == row - 2 && board->pieceFromPosition(row - 2, col)->getColour() == 'e'
			&& board->pieceFromPosition(row - 1, col)->getColour() == 'e') {
			if (modify == true) {
				if (col != 0 && board->grid[row - 2][col - 1]->getType() == "pawn") {
					board->grid[row - 2][col - 1]->setEP(board->grid[row][col]);
				}
				if (col != 7 && board->grid[row - 2][col + 1]->getType() == "pawn") {
					board->grid[row - 2][col + 1]->setEP(board->grid[row][col]);
				}
			}
			return true;
		}
		else if (newCol == col && newRow == row - 1 && board->pieceFromPosition(row - 1, col)->getColour() == 'e') {
			return true;
		}
		else if (col != 7 && newRow == row - 1 && newCol == col + 1 && board->pieceFromPosition(row - 1, col + 1)->getColour() == 'w') {
			return true;
		}
		else if (col != 0 && newRow == row - 1 && newCol == col - 1 && board->pieceFromPosition(row - 1, col - 1)->getColour() == 'w') {
			return true;
		}
		else if (enPass != nullptr) {
			if (newRow == enPass->getPosition().first - 1 && newCol == enPass->getPosition().second) {
				return true;
			}
		}
		else return false;
	}
	return false;
}

bool piece::queenValidMove(int newRow, int newCol) {
	if (row == newRow && col == newCol) {
		return false;
	}
	else if (board->pieceFromPosition(newRow, newCol)->getColour() == colour) {
		return false;
	}
	else if (row == newRow && col != newCol) {
		if ((newCol - col) > 0) {
			return testMove(row, col, newRow, newCol, "e");
		}
		else if ((newCol - col) < 0) {
			return testMove(row, col, newRow, newCol, "w");
		}
	}
	else if (row != newRow && col == newCol) {
		if ((newRow - row) < 0) {
			return testMove(row, col, newRow, newCol, "s");
		}
		else if ((newRow - row) > 0) {
			return testMove(row, col, newRow, newCol, "n");
		}
	}
	else if (abs(newRow - row) == abs(newCol - col)) {
		if ((newRow - row) < 0) {
			if ((newCol - col) > 0) {
				return testMove(row, col, newRow, newCol, "se");
			}
			else {
				return testMove(row, col, newRow, newCol, "sw");
			}
		}
		else {
			if ((newCol - col) > 0) {
				return testMove(row, col, newRow, newCol, "ne");
			}
			else {
				return testMove(row, col, newRow, newCol, "nw");
			}
		}
	}
	return false;
}

bool piece::testMove(int gridRow, int gridCol, int newRow, int newCol, std::string direction) {
	if (direction == "n") {
		gridRow++;
	}
	else if (direction == "ne") {
		gridRow++;
		gridCol++;
	}
	else if (direction == "e") {
		gridCol++;
	}
	else if (direction == "se") {
		gridRow--;
		gridCol++;
	}
	else if (direction == "s") {
		gridRow--;
	}
	else if (direction == "sw") {
		gridRow--;
		gridCol--;
	}
	else if (direction == "w") {
		gridCol--;
	}
	else if (direction == "nw") {
		gridRow++;
		gridCol--;
	}
	else {
		return false;
	}

	//base case 2: the piece has moved out of bounds
	if (!(gridRow >= 0 && gridRow < 8 && gridCol >= 0 && gridCol < 8)) {
		return false;
	}

	//base case 1: a friendly piece is occupying the position, so you cannot move there
	else if (board->pieceFromPosition(newRow, newCol)->getColour() == colour) {
		return false;
	}
	//base case 3: the given position is found
	else if (gridRow == newRow && gridCol == newCol) {
		return true;
	}
	//base case 4: a piece is blocking the position, before the position is found
	else if (board->pieceFromPosition(gridRow, gridCol)->getType() != "empty") {
		return false;
	}
	//recursion
	else return testMove(gridRow, gridCol, newRow, newCol, direction);
}

bool piece::testMove2(int gridRow, int gridCol, int newRow, int newCol, std::string direction) {
	if (direction == "n") {
		gridRow++;
	}
	else if (direction == "ne") {
		gridRow++;
		gridCol++;
	}
	else if (direction == "e") {
		gridCol++;
	}
	else if (direction == "se") {
		gridRow--;
		gridCol++;
	}
	else if (direction == "s") {
		gridRow--;
	}
	else if (direction == "sw") {
		gridRow--;
		gridCol--;
	}
	else if (direction == "w") {
		gridCol--;
	}
	else if (direction == "nw") {
		gridRow++;
		gridCol--;
	}
	else {
		return false;
	}

	//base case 2: the piece has moved out of bounds
	if (!(gridRow >= 0 && gridRow < 8 && gridCol >= 0 && gridCol < 8)) {
		return false;
	}

	//base case 1: a friendly piece is occupying the position, so you cannot move there
	else if (board->pieceFromPosition(gridRow, gridCol)->getColour() == colour) {
		return false;
	}
	//base case 3: the given position is found
	else if (gridRow == newRow && gridCol == newCol) {
		getMoveCheck(gridRow, gridCol);
		return true;
	}
	//base case 4: a enemy piece is blocking the position, before the position is found
	else if (board->pieceFromPosition(gridRow, gridCol)->getType() != "empty") {
		getMoveCheck(gridRow, gridCol);
		return false;
	}
	//recursion
	else {
		getMoveCheck(gridRow, gridCol);
		return testMove2(gridRow, gridCol, newRow, newCol, direction);
	}
}

bool piece::rookValidMove(int newRow, int newCol) {
	if (row == newRow && col == newCol) {
		return false;
	}
	else if (board->pieceFromPosition(newRow, newCol)->getColour() == colour) {
		return false;
	}
	else if (row == newRow && col != newCol) {
		if ((newCol - col) > 0) {
			return testMove(row, col, newRow, newCol, "e");
		}
		else if ((newCol - col) < 0) {
			return testMove(row, col, newRow, newCol, "w");
		}
	}
	else if (row != newRow && col == newCol) {
		if ((newRow - row) < 0) {
			return testMove(row, col, newRow, newCol, "s");
		}
		else if ((newRow - row) > 0) {
			return testMove(row, col, newRow, newCol, "n");
		}
	}
	return false;
}

bool piece::knightValidMove(int newRow, int newCol) {
	if (col == newCol && row == newRow) {
		return false;
	}
	else if (board->pieceFromPosition(newRow, newCol)->getColour() == colour) {
		return false;
	}
	else if (abs(newRow - row) == 2 && abs(newCol - col) == 1) {
		return true;
	}
	else if (abs(newRow - row) == 1 && abs(newCol - col) == 2) {
		return true;
	}
	else return false;
}

void piece::setPieceEmpty() {
	setType("empty");
	setColour('e');
	setMoved(false);
	setCaptured(false);
}

bool piece::getLC() {
	return leftCastle;
}

bool piece::getRC() {
	return rightCastle;
}

void piece::setLC(bool state) {
	leftCastle = state;
}
void piece::setRC(bool state) {
	rightCastle = state;
}

void piece::getMove() {
	if (type == "pawn") {
		if (colour == 'w') {
			getMoveCheck(row + 1, col);
			getMoveCheck(row + 1, col + 1);
			getMoveCheck(row + 1, col - 1);
			if (moved == false) {
				getMoveCheck(row + 2, col);
			}
		}
		else if (colour == 'b') {
			getMoveCheck(row - 1, col);
			getMoveCheck(row - 1, col + 1);
			getMoveCheck(row - 1, col - 1);
			if (moved == false) {
				getMoveCheck(row - 2, col);
			}
		}
	}
	else if (type == "knight") {
		getMoveCheck(row + 2, col - 1);
		getMoveCheck(row + 2, col + 1);
		getMoveCheck(row - 2, col - 1);
		getMoveCheck(row - 2, col + 1);
		getMoveCheck(row - 1, col + 2);
		getMoveCheck(row + 1, col + 2);
		getMoveCheck(row - 1, col - 2);
		getMoveCheck(row + 1, col - 2);
	}
	else if (type == "king") {
		getMoveCheck(row + 1, col);
		getMoveCheck(row - 1, col);
		getMoveCheck(row, col - 1);
		getMoveCheck(row, col + 1);
		getMoveCheck(row + 1, col + 1);
		getMoveCheck(row + 1, col - 1);
		getMoveCheck(row - 1, col + 1);
		getMoveCheck(row - 1, col - 1);
		if (moved == false) {
			getMoveCheck(row, col + 2);
			getMoveCheck(row, col - 2);
		}
	}
	else if (type == "queen") {
		testMoveDiagonal(row, col);
		testMoveCardinal(row, col);
	}
	else if (type == "rook") {
		testMoveCardinal(row, col);
	}
	else if (type == "bishop") {
		testMoveDiagonal(row, col);
	}

	if (colour == 'b') {
		board->blackMoves.reserve(board->blackMoves.size() + distance(moves.begin(), moves.end()));
		board->blackMoves.insert(board->blackMoves.end(), moves.begin(), moves.end());
	}
	else if (colour == 'w') {
		board->whiteMoves.reserve(board->whiteMoves.size() + distance(moves.begin(), moves.end()));
		board->whiteMoves.insert(board->whiteMoves.end(), moves.begin(), moves.end());
	}
}

void piece::getMoveCheck(int newRow, int newCol) {
	if (bound(newRow, newCol) == false) {
		return;
	}
	if (board->grid[newRow][newCol]->getColour() != colour) {
		if (board->moveCheckAll(row, col, newRow, newCol) == true) {
			moves.push_back(board->pairConversion(row, col, newRow, newCol));
			//std::cout << std::string(1,colour) << "new:" << row+1 << (char)(col + 65) << "-" << newRow + 1 << (char)(newCol + 65) <<  "(" << type << ")" << std::endl;
		}
	}
}

bool piece::bound(int gridRow, int gridCol) {
	if (gridRow >= 0 && gridRow < 8 && gridCol >= 0 && gridCol < 8) {
		return true;
	}
	else {
		return false;
	}
}

void piece::testMoveDiagonal(int row, int col) {
	//std::cout << "ne----------" << std::endl;
	int rowtest = 7 - row;
	int coltest = 7 - col;
	if (rowtest < coltest) {
		testMove2(row, col, row + rowtest, col + rowtest, "ne");
	}
	else {
		testMove2(row, col, row + coltest, col + coltest, "ne");
	}
	//nw
	//std::cout << "nw----------" << std::endl;
	rowtest = 7 - row;
	coltest = col;
	if (rowtest < coltest) {
		testMove2(row, col, row + rowtest, col - rowtest, "nw");
	}
	else {
		testMove2(row, col, row + coltest, col - coltest, "nw");
	}
	//se
	//std::cout << "se----------" << std::endl;
	rowtest = row;
	coltest = 7 - col;
	if (rowtest < coltest) {
		testMove2(row, col, row - rowtest, col + rowtest, "se");
	}
	else {
		testMove2(row, col, row - coltest, col + coltest, "se");
	}
	//sw
	//std::cout << "sw----------" << std::endl;
	rowtest = row;
	coltest = col;
	if (rowtest < coltest) {
		testMove2(row, col, row - rowtest, col - rowtest, "sw");
	}
	else {
		testMove2(row, col, row - coltest, col - coltest, "sw");
	}
}

void piece::testMoveCardinal(int row, int col) {
	//std::cout << "s-----------" << std::endl;
	testMove2(row, col, 0, col, "s");
	//std::cout << "n-----------" << std::endl;
	testMove2(row, col, 7, col, "n");
	//std::cout << "w-----------" << std::endl;
	testMove2(row, col, row, 0, "w");
	//std::cout << "e-----------" << std::endl;
	testMove2(row, col, row, 7, "e");
}