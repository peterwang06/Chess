#include "chess_board.h"
#include "positions.h"

#include <vector>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define new new
#endif

chess_board::chess_board(sf::Sprite blackSprites_[], sf::Sprite whiteSprites_[], float sizeUnit_, float offSet_): white(16), black(16){
	blackSprites = blackSprites_;
	whiteSprites = whiteSprites_;
	sizeUnit = sizeUnit_;
	offSet = offSet_;
	turn = 'w';
	ai = 'x';
	playerTwo = 'x';
	window = true;
	promotionType = -1;
	winFX = false;
	moveFX = false;

	grid[0][0] = new piece("rook", 0, 0, 'w', this);
	grid[0][1] = new piece("knight", 0, 1, 'w', this);
	grid[0][2] = new piece("bishop", 0, 2, 'w', this);
	grid[0][3] = new piece("queen", 0, 3, 'w', this);
	grid[0][4] = new piece("king", 0, 4, 'w', this);
	grid[0][5] = new piece("bishop", 0, 5, 'w', this);
	grid[0][6] = new piece("knight", 0, 6, 'w', this);
	grid[0][7] = new piece("rook", 0, 7, 'w', this);

	grid[7][0] = new piece("rook", 7, 0, 'b', this);
	grid[7][1] = new piece("knight", 7, 1, 'b', this);
	grid[7][2] = new piece("bishop", 7, 2, 'b', this);
	grid[7][3] = new piece("queen", 7, 3, 'b', this);
	grid[7][4] = new piece("king", 7, 4, 'b', this);
	grid[7][5] = new piece("bishop", 7, 5, 'b', this);
	grid[7][6] = new piece("knight", 7, 6, 'b', this);
	grid[7][7] = new piece("rook", 7, 7, 'b', this);

	for (int col = 0; col < 8; col++) {
		grid[1][col] = new piece("pawn", 1, col, 'w', this);
		white[col] = grid[0][col];
		white[8 + col] = grid[1][col];

		grid[6][col] = new piece("pawn", 6, col, 'b', this);
		black[col] = grid[7][col];
		black[8 + col] = grid[6][col];

		for (int row = 2; row < 6; row++) {
			grid[row][col] = new piece("empty", row, col, 'e', this);
		}
	}
}

chess_board::chess_board(chess_board* old, bool _window) { //allows the cloning of board to evaluate moves
	sizeUnit = old->sizeUnit;
	offSet = old->offSet;
	blackSprites = old->blackSprites;
	whiteSprites = old->whiteSprites;
	whiteCaptured = old->whiteCaptured;
	blackCaptured = old->blackCaptured;
	turn = old->turn;
	ai = old->ai;
	playerTwo = old->playerTwo;
	window = old->window;
	promotionType = -1;
	blackMoves = std::vector<int>(old->blackMoves);
	whiteMoves = std::vector<int>(old->whiteMoves);
	winFX = false;
	moveFX = false;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			grid[row][col] = new piece(old->grid[row][col], this);
			if (grid[row][col]->getColour() == 'w') {
				white.push_back(grid[row][col]);
			}
			if (grid[row][col]->getColour() == 'b') {
				black.push_back(grid[row][col]);
			}
		}
	}
	window = _window;
}

chess_board::~chess_board() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			delete grid[i][j];
		}
	}
}

piece* chess_board::pieceFromPosition(int row, int col) { //what piece is selected
	return (grid[row][col]);
}
std::pair <int, int> chess_board::positionFromName(std::string name, std::vector<piece*> colour) { //position of piece on this
	for (auto ptr = colour.begin(); ptr < colour.end(); ptr++) {
		if ((*ptr)->getType() == name) {
			return (*ptr)->getPosition();
		}
	}
	return std::make_pair((int)-1, (int)-1);
}

bool chess_board::check(char colour_) {
	std::pair<int, int> king_pos;
	if (colour_ == 'w') {
		king_pos = positionFromName("king", white);
		for (auto ptr = black.begin(); ptr < black.end(); ptr++) {
			if ((*ptr)->validMove(king_pos.first, king_pos.second, false) == true) {
				return true;
			}
		}
	}
	else if (colour_ == 'b') {
		king_pos = positionFromName("king", black);
		for (auto ptr = white.begin(); ptr < white.end(); ptr++) {
			if ((*ptr)->validMove(king_pos.first, king_pos.second, false) == true) {
				return true;
			}
		}
	}

	return false;
}

char chess_board::getTurn() {
	return turn;
}

void chess_board::switchTurn() {
	if (turn == 'w') {
		turn = 'b';
		return;
	}
	if (turn == 'b') {
		turn = 'w';
		return;
	}
}

void chess_board::move(int row1, int col1, int row2, int col2) {
	moveFX = true;
	grid[row1][col1]->setMoved(true);
	if (grid[row1][col1]->getLC() == true) {
		grid[row1][col1]->setLC(false);
		if (grid[row1][col1]->getColour() == 'w') { //white queenside castle
			swap(row1, col1, row2, col2);
			pieceFromPosition(0, 0)->setMoved(true);
			swap(0, 0, 0, 3);
		}
		else if (grid[row1][col1]->getColour() == 'b') { //black queenside castle
			swap(row1, col1, row2, col2);
			pieceFromPosition(7, 0)->setMoved(true);
			swap(7, 0, 7, 3);
		}
	}
	else if (grid[row1][col1]->getRC() == true) { //white kingside castle
		grid[row1][col1]->setRC(false);
		if (grid[row1][col1]->getColour() == 'w') {
			swap(row1, col1, row2, col2);
			pieceFromPosition(0, 7)->setMoved(true);
			swap(0, 7, 0, 5);
		}
		else if (grid[row1][col1]->getColour() == 'b') { //black kingside castle
			swap(row1, col1, row2, col2);
			pieceFromPosition(7, 7)->setMoved(true);
			swap(7, 7, 7, 5);
		}
	}
	else if (grid[row1][col1]->getEP() != nullptr && grid[row1][col1]->getEP()->getPosition().second == col2) {
		piece* enPass = grid[row1][col1]->getEP();
		moveToCaptured(enPass);
		grid[enPass->getPosition().first][enPass->getPosition().second]->setPieceEmpty();
		swap(row1, col1, row2, col2);
		return;
	}
	else if (row1 == row2 && col1 == col2) {
		std::cout << "Same tile" << std::endl;
		return;
	}
	else if (grid[row1][col1]->getType() == "empty") {
		std::cout << "Empty selection" << std::endl;
		return;
	}
	else if (grid[row2][col2]->getType() == "empty") {
		swap(row1, col1, row2, col2);
		return;
	}
	else if (grid[row2][col2]->getType() != "empty") {
		moveToCaptured(grid[row2][col2]);
		grid[row2][col2]->setPieceEmpty();
		swap(row1, col1, row2, col2);
		return;
	}
}

void chess_board::swap(int row1, int col1, int row2, int col2) {
	int temp_row = grid[row1][col1]->getPosition().first;
	int temp_col = grid[row1][col1]->getPosition().second;

	grid[row1][col1]->setPosition(grid[row2][col2]->getPosition().first,
		grid[row2][col2]->getPosition().second);
	grid[row2][col2]->setPosition(temp_row, temp_col);

	piece* temp = grid[row1][col1];
	grid[row1][col1] = grid[row2][col2];
	grid[row2][col2] = temp;
}

void chess_board::endTurn() {
	//if (check('b')) {
	//	std::cout << "black is in check!" << std::endl;
	//}
	//else {
	//	std::cout << "black is not in check!" << std::endl;
	//}
	//if (check('w')) {
	//	std::cout << "white is in check!" << std::endl;
	//}
	//else {
	//	std::cout << "white is not in check!" << std::endl;
	//}

	promotionCheck();

	if (getTurn() == 'w') {
		for (auto ptr = white.begin(); ptr < white.end();) {
			if ((*ptr)->getType() == "empty") {
				ptr = white.erase(ptr);
			}
			else {
				(*ptr)->setEP(nullptr);
				ptr++;
			}
		}
	}
	else if (getTurn() == 'b') {
		for (auto ptr = black.begin(); ptr < black.end();) {
			if ((*ptr)->getType() == "empty") {
				ptr = black.erase(ptr);
			}
			else {
				(*ptr)->setEP(nullptr);
				ptr++;
			}
		}
	}
	switchTurn();
}

void chess_board::promotionCheck() {
	for (auto ptr = white.begin(); ptr < white.end(); ptr++) {
		if ((*ptr)->getPosition().first == 7 && (*ptr)->getType() == "pawn") {
			int row = (*ptr)->getPosition().first;
			int col = (*ptr)->getPosition().second;
			std::string type;
			if (playerTwo != 'x' && promotionType != -1) {
				type = enumToString(promotionType);
				promotionType = -1;
			}
			else if (ai == 'w' || window == false) {
				type = "queen";
			}
			else {
				type = promotion_window();
				if (playerTwo != 'x') {
					promotionType = stringToEnum(type);
				}
			}
			grid[row][col]->setType(type);
			(*ptr) = grid[row][col];
		}
	}
	for (auto ptr = black.begin(); ptr < black.end(); ptr++) {
		if ((*ptr)->getPosition().first == 0 && (*ptr)->getType() == "pawn") {
			int row = (*ptr)->getPosition().first;
			int col = (*ptr)->getPosition().second;
			std::string type;
			if (playerTwo != 'x' && promotionType != -1) {
				type = enumToString(promotionType);
				promotionType = -1;
			}
			else if (ai == 'b' || window == false) {
				type = "queen";
			}
			else {
				type = promotion_window();
				if (playerTwo != 'x') {
					promotionType = stringToEnum(type);
				}
			}

			grid[row][col]->setType(type);
			(*ptr) = grid[row][col];
		}
	}
}

std::string chess_board::promotion_window() {
	sf::RenderWindow promotion_window(sf::VideoMode((int)(sizeUnit * 4), (int)(sizeUnit)), "Promotion", sf::Style::Titlebar);
	sf::Vector2f size_background((sizeUnit * 4), (sizeUnit));
	sf::RectangleShape background(size_background);

	if (getTurn() == 'w') {
		background.setFillColor(sf::Color::Black);
		promotion_window.draw(background);
		sf::Sprite sprite = whiteSprites[QUEEN];
		sprite.setPosition(0, 0);
		promotion_window.draw(sprite);

		sprite = whiteSprites[ROOK];
		sprite.setPosition(sizeUnit, 0);
		promotion_window.draw(sprite);

		sprite = whiteSprites[BISHOP];
		sprite.setPosition(sizeUnit * 2, 0);
		promotion_window.draw(sprite);

		sprite = whiteSprites[KNIGHT];
		sprite.setPosition(sizeUnit * 3, 0);
		promotion_window.draw(sprite);
	}
	if (getTurn() == 'b') {
		background.setFillColor(sf::Color::White);
		promotion_window.draw(background);
		sf::Sprite sprite = blackSprites[QUEEN];
		sprite.setPosition(0, 0);
		promotion_window.draw(sprite);

		sprite = blackSprites[ROOK];
		sprite.setPosition(sizeUnit, 0);
		promotion_window.draw(sprite);
		sprite = blackSprites[BISHOP];
		sprite.setPosition(sizeUnit * 2, 0);
		promotion_window.draw(sprite);

		sprite = blackSprites[KNIGHT];
		sprite.setPosition(sizeUnit * 3, 0);
		promotion_window.draw(sprite);
	}
	promotion_window.display();

	int row1 = 0;
	int col1 = 0;
	int row2 = 0;
	int col2 = 0;
	while (promotion_window.isOpen()) {
		sf::Event event;
		while (promotion_window.pollEvent(event))
		{
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					row1 = event.mouseButton.y / (int)sizeUnit;
					col1 = event.mouseButton.x / (int)sizeUnit;
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					row2 = event.mouseButton.y / (int)sizeUnit;
					col2 = event.mouseButton.x / (int)sizeUnit;
					if (col2 == 0) {
						promotion_window.close();
						std::cout << "promoted to queen!" << std::endl;
						return "queen";
					}
					if (col2 == 1) {
						promotion_window.close();
						std::cout << "promoted to rook!" << std::endl;
						return "rook";
					}
					if (col2 == 2) {
						promotion_window.close();
						std::cout << "promoted to bishop!" << std::endl;
						return "bishop";
					}
					if (col2 == 3) {
						promotion_window.close();
						std::cout << "promoted to knight!" << std::endl;
						return "knight";
					}
				}
			}
		}
	}
	return "promotion failed";
}

bool chess_board::checkPosition(char colour_, int row, int col) {
	if (colour_ == 'w') {
		for (auto ptr = black.begin(); ptr < black.end(); ptr++) {
			if ((*ptr)->validMove(row, col, false) == true) {
				return true;
			}
		}
	}
	else if (colour_ == 'b') {
		for (auto ptr = white.begin(); ptr < white.end(); ptr++) {
			if ((*ptr)->validMove(row, col, false) == true) {
				return true;
			}
		}
	}

	return false;
}

void chess_board::moveToCaptured(piece* piece_) {
	if (piece_->getColour() == 'b') {
		blackCaptured.push_back(piece_->getType());
	}
	else if (piece_->getColour() == 'w') {
		whiteCaptured.push_back(piece_->getType());
	}
}

bool chess_board::moveCheck(int row1, int col1, int row2, int col2) {
	char col = grid[row1][col1]->getColour();
	if (col != getTurn()) {
		return false;
	}
	int key = pairConversion(row1, col1, row2, col2);
	bool found = false;
	if (col == 'b') {
		for (size_t i = 0; i < blackMoves.size(); i++) {
			if (blackMoves[i] == key) {
				found = true;
			}
		}
	}
	else if (col == 'w') {
		for (size_t i = 0; i < whiteMoves.size(); i++) {
			if (whiteMoves[i] == key) {
				found = true;
			}
		}
	}

	if (found == true) {
		clearAllMoves();
		grid[row1][col1]->validMove(row2, col2, true); //needed for enPasse and castling to work
		move(row1, col1, row2, col2);
		endTurn();
		calculateAllMoves();
		return true;
	}

	return false;
}
bool chess_board::moveCheckDraw(int row1, int col1, int row2, int col2) {
	chess_board testBoard(this, false);
	if (testBoard.grid[row1][col1]->validMove(row2, col2, true) == true) {
		testBoard.move(row1, col1, row2, col2);
		return !testBoard.check(testBoard.getTurn());
	}
	return false;
}

bool chess_board::moveCheckAll(int row1, int col1, int row2, int col2) {
	chess_board testBoard(this, false);
	if (testBoard.grid[row1][col1]->validMove(row2, col2, true) == true) {
		testBoard.move(row1, col1, row2, col2);
		return !testBoard.check(testBoard.getTurn());
		/*if (!testBoard.check(testBoard.getTurn())) {
			return true;
		}*/
	}
	return false;
}

void chess_board::calculateAllMoves() {
	if (turn == 'b') {
		for (auto ptr = black.begin(); ptr < black.end(); ptr++) {
			auto piece = (*ptr);
			piece->getMove();
		}
	}
	else if (turn == 'w') {
		for (auto ptr = white.begin(); ptr < white.end(); ptr++) {
			auto piece = (*ptr);
			(*ptr)->getMove();
		}
	}
}

void chess_board::calculateColourMoves(char colour) {
	if (colour == 'b') {
		for (auto ptr = black.begin(); ptr < black.end(); ptr++) {
			auto piece = (*ptr);
			piece->getMove();
		}
	}
	else if (colour == 'w') {
		for (auto ptr = white.begin(); ptr < white.end(); ptr++) {
			auto piece = (*ptr);
			(*ptr)->getMove();
		}
	}
}

void chess_board::clearAllMoves() {
	whiteMoves.clear();
	blackMoves.clear();
	for (auto ptr = white.begin(); ptr < white.end(); ptr++) {
		(*ptr)->moves.clear();
	}
	for (auto ptr = black.begin(); ptr < black.end(); ptr++) {
		(*ptr)->moves.clear();
	}
}

int chess_board::pairConversion(int row1, int col1, int row2, int col2) {
	return row1 * 1000 + col1 * 100 + row2 * 10 + col2;
}

std::pair< std::pair <int, int>, std::pair <int, int>> chess_board::intConversion(int key) {
	int row1 = key / 1000;
	int col1 = (key - row1 * 1000) / 100;
	int row2 = (key - row1 * 1000 - col1 * 100) / 10;
	int col2 = (key - row1 * 1000 - col1 * 100 - row2 * 10);
	return std::make_pair(std::make_pair(row1, col1), std::make_pair(row2, col2));
}

void chess_board::checkmate() {
	if (turn == 'b' && blackMoves.size() == 0 && check('b') == true) {
		std::cout << "Checkmate: White wins" << std::endl;
		winFX = true;
	}
	else if (turn == 'w' && whiteMoves.size() == 0 && check('w') == true) {
		std::cout << "Checkmate: Black wins" << std::endl;
		winFX = true;
	}
	else if (turn == 'w' && whiteMoves.size() == 0 && check('w') == false) {
		std::cout << "Stalemate" << std::endl;
		winFX = true;
	}
	else if (turn == 'b' && blackMoves.size() == 0 && check('b') == false) {
		std::cout << "Stalemate" << std::endl;
		winFX = true;
	}
}

int chess_board::alphaBetaMax(int alpha, int beta, int depthleft, int * counter) {
	int score;
	if (depthleft == 0) {
		*counter += 1;
		return evaluatePosition(getTurn());
	}

	if (getTurn() == 'w') {
		for (size_t i = 0; i < whiteMoves.size(); i++) {
			chess_board test(this, false);
			std::pair<std::pair<int, int>, std::pair<int, int>> moveConverted = intConversion(test.whiteMoves[i]);
			int row1 = moveConverted.first.first;
			int col1 = moveConverted.first.second;
			int row2 = moveConverted.second.first;
			int col2 = moveConverted.second.second;
			test.moveCheck(row1, col1, row2, col2);
			score = test.alphaBetaMin(alpha, beta, depthleft - 1, counter);
			if (score >= beta)
				return beta;   // fail hard beta-cutoff
			if (score > alpha)
				alpha = score; // alpha acts like max in MiniMax
		}
	}
	else if (getTurn() == 'b') {
		for (size_t i = 0; i < blackMoves.size(); i++) {
			chess_board test(this, false);
			std::pair<std::pair<int, int>, std::pair<int, int>> moveConverted = intConversion(test.blackMoves[i]);
			int row1 = moveConverted.first.first;
			int col1 = moveConverted.first.second;
			int row2 = moveConverted.second.first;
			int col2 = moveConverted.second.second;
			test.moveCheck(row1, col1, row2, col2);
			score = test.alphaBetaMin(alpha, beta, depthleft - 1, counter);
			if (score >= beta)
				return beta;   // fail hard beta-cutoff
			if (score > alpha)
				alpha = score; // alpha acts like max in MiniMax
		}
	}
	return alpha;
}

int chess_board::alphaBetaMin(int alpha, int beta, int depthleft, int * counter) {
	int score;
	if (depthleft == 0){
		*counter += 1;
		return  -evaluatePosition(getTurn()); 
	}
	if (getTurn() == 'w') {
		for (size_t i = 0; i < whiteMoves.size(); i++) {
			chess_board test(this, false);
			std::pair<std::pair<int, int>, std::pair<int, int>> moveConverted = intConversion(test.whiteMoves[i]);
			int row1 = moveConverted.first.first;
			int col1 = moveConverted.first.second;
			int row2 = moveConverted.second.first;
			int col2 = moveConverted.second.second;
			test.moveCheck(row1, col1, row2, col2);
			score = test.alphaBetaMax(alpha, beta, depthleft - 1, counter);
			if (score <= alpha)
				return alpha; // fail hard alpha-cutoff
			if (score < beta)
				beta = score; // beta acts like min in MiniMax
		}
	}
	else if (getTurn() == 'b') {
		for (size_t i = 0; i < blackMoves.size(); i++) {
			chess_board test(this, false);
			std::pair<std::pair<int, int>, std::pair<int, int>> moveConverted = intConversion(test.blackMoves[i]);
			int row1 = moveConverted.first.first;
			int col1 = moveConverted.first.second;
			int row2 = moveConverted.second.first;
			int col2 = moveConverted.second.second;
			test.moveCheck(row1, col1, row2, col2);
			score = test.alphaBetaMax(alpha, beta, depthleft - 1, counter);
			if (score <= alpha)
				return alpha; // fail hard alpha-cutoff
			if (score < beta)
				beta = score; // beta acts like min in MiniMax
		}
	}

	return beta;
}

int chess_board::evaluatePosition(char max) {
	int blackvalue = 0;
	int whitevalue = 0;
	for (auto itr = black.begin(); itr != black.end(); itr++) {
		blackvalue += evaluatePiece(*itr);
	}
	for (auto itr = white.begin(); itr != white.end(); itr++) {
		whitevalue += evaluatePiece(*itr);
	}
	if (max == 'b') {
		return blackvalue - whitevalue;
	}
	else if (max == 'w') {
		return whitevalue - blackvalue;
	}

	return blackvalue - whitevalue;
}

int chess_board::evaluatePiece(piece* pce) {
	std::string type = pce->getType();
	char colour = pce->getColour();
	int row = pce->getPosition().first;
	int col = pce->getPosition().second;
	int pieceValue = 0;
	int positionValue = 0;
	if (type == "pawn") {
		if (colour == 'w') {
			positionValue = pawnSquares[7 - row][col];
		}
		else if (colour == 'b') {
			positionValue = pawnSquares[row][7 - col];
		}
	}
	else if (type == "knight") {
		if (colour == 'w') {
			positionValue = knightSquares[7 - row][col];
		}
		else if (colour == 'b') {
			positionValue = knightSquares[row][7 - col];
		}
	}
	else if (type == "bishop") {
		if (colour == 'w') {
			positionValue = bishopSquares[7 - row][col];
		}
		else if (colour == 'b') {
			positionValue = bishopSquares[row][7 - col];
		}
	}
	else if (type == "rook") {
		if (colour == 'w') {
			positionValue = rookSquares[7 - row][col];
		}
		else if (colour == 'b') {
			positionValue = rookSquares[row][7 - col];
		}
	}
	else if (type == "queen") {
		if (colour == 'w') {
			positionValue = queenSquares[7 - row][col];
		}
		else if (colour == 'b') {
			positionValue = queenSquares[row][7 - col];
		}
	}
	else if (type == "king") {
		if (colour == 'w') {
			positionValue = kingSquares[7 - row][col];
		}
		else if (colour == 'b') {
			positionValue = kingSquares[row][7 - col];
		}
	}

	if (type == "pawn") {
		pieceValue = 100;
	}
	else if (type == "knight") {
		pieceValue = 320;
	}
	else if (type == "bishop") {
		pieceValue = 330;
	}
	else if (type == "rook") {
		pieceValue = 500;
	}
	else if (type == "queen") {
		pieceValue = 900;
	}
	else if (type == "king") {
		pieceValue = 20000;
	}

	return pieceValue + positionValue;
}

bool chess_board::aiTurn() {
	int move = 0;
	int score = 0;
	int counter = 0;
	int highscore = std::numeric_limits<int>::min();
	int alpha = std::numeric_limits<int>::min();
	int beta = std::numeric_limits<int>::max();
	int depth = 2;
	if (ai == 'b') {
		for (size_t i = 0; i < blackMoves.size(); i++) {
			chess_board test(this, false);
			std::pair<std::pair<int, int>, std::pair<int, int>> moveConverted = intConversion(test.blackMoves[i]);
			int row1 = moveConverted.first.first;
			int col1 = moveConverted.first.second;
			int row2 = moveConverted.second.first;
			int col2 = moveConverted.second.second;
			test.moveCheck(row1, col1, row2, col2);
			score = test.alphaBetaMin(alpha, beta, depth, &counter);
			std::cout << "(" << grid[row1][col1]->getType() << ") " << "move: " << (row1+1) << (char)(col1+97) << "->" << row2+1 << (char)(col2+97) << " score:" << score << std::endl;
			if (score >= highscore) {
				highscore = score;
				move = blackMoves[i];
			}
		}
		std::cout << "Positions evaluated: " << counter << std::endl;
	}
	else if (ai == 'w') {
		for (size_t i = 0; i < whiteMoves.size(); i++) {
			chess_board test(this, false);
			std::pair<std::pair<int, int>, std::pair<int, int>> moveConverted = intConversion(test.whiteMoves[i]);
			int row1 = moveConverted.first.first;
			int col1 = moveConverted.first.second;
			int row2 = moveConverted.second.first;
			int col2 = moveConverted.second.second;
			test.moveCheck(row1, col1, row2, col2);
			score = test.alphaBetaMin(alpha, beta, depth, &counter);
			if (score >= highscore) {
				highscore = score;
				move = whiteMoves[i];
			}
		}
	}

	std::pair<std::pair<int, int>, std::pair<int, int>> moveConverted = intConversion(move);
	int row1 = moveConverted.first.first;
	int col1 = moveConverted.first.second;
	int row2 = moveConverted.second.first;
	int col2 = moveConverted.second.second;
	if (moveCheck(row1, col1, row2, col2) == true) {
		checkmate();
		return true;
	}
	else {
		return false;
	}
}

int chess_board::stringToEnum(std::string input) {
	if (input == "pawn") {
		return PAWN;
	}
	else if (input == "knight") {
		return KNIGHT;
	}
	else if (input == "bishop") {
		return BISHOP;
	}
	else if (input == "rook") {
		return ROOK;
	}
	else if (input == "queen") {
		return QUEEN;
	}
	else if (input == "king") {
		return KING;
	}
	else {
		return -1;
	}
}
std::string chess_board::enumToString(int input) {
	if (input == PAWN) {
		return "pawn";
	}
	else if (input == KNIGHT) {
		return "knight";
	}
	else if (input == BISHOP) {
		return "bishop";
	}
	else if (input == ROOK) {
		return "rook";
	}
	else if (input == QUEEN) {
		return "queen";
	}
	else if (input == KING) {
		return "king";
	}
	else {
		return "error";
	}
}