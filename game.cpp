#include "game.h"

//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#ifdef _DEBUG
//#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#else
//#define new new
//#endif
bool isNumber(const std::string& str);

game::game() {
	if (!font.loadFromFile("ARLRDBD.ttf")) {
		std::cout << "font failed!" << std::endl;
		loadSuccess = false;
	}
	else {
		std::cout << "font loaded!" << std::endl;
	}

	if (!texture.loadFromFile("spritemap.png"))
	{
		std::cout << "sprites failed!" << std::endl;
		loadSuccess = false;
	}
	else {
		for (int i = 0; i < 6; i++) {
			whiteSprites[i].setTexture(texture);
			blackSprites[i].setTexture(texture);
			whiteSprites[i].setTextureRect(sf::IntRect(i * 60, 60, 60, 60));
			blackSprites[i].setTextureRect(sf::IntRect(i * 60, 0, 60, 60));
		}
		std::cout << "sprites loaded!" << std::endl;
	}
	board = new chess_board(blackSprites, whiteSprites, sizeUnit, offSet);
	handShake();
	std::string msg = "Chess";
	if (online && host) {
		msg = "Chess (Host)";
	}
	else if (online && !host) {
		msg = "Chess (Client)";
	}
	gameWindow = new sf::RenderWindow(sf::VideoMode((int)(sizeUnit * 10), (int)(sizeUnit * 10)), msg, sf::Style::Titlebar | sf::Style::Close);
	loadSuccess = true;
	gameLoop();
}

game::~game() {
	delete board;
	delete gameWindow;
}

bool game::gameLoop() {
	int row1 = 0;
	int col1 = 0;
	int row2 = 0;
	int col2 = 0;
	int count = 0;
	bool moveLoaded;
	bool winLoaded;
	sf::Sound moveFX;
	sf::Sound winFX;
	bool once = false;

	sf::SoundBuffer move;
	if (!move.loadFromFile("move.wav")) {
		std::cout << "move.wav not found" << std::endl;
		moveLoaded = false;
	}
	else {
		moveLoaded = true;
		moveFX.setBuffer(move);
		std::cout << "move sound loaded!" << std::endl;
	}
	sf::SoundBuffer win;
	if (!win.loadFromFile("win.wav")) {
		std::cout << "win.wav not found" << std::endl;
		winLoaded = false;
	}
	else {
		winLoaded = true;
		winFX.setBuffer(win);
		std::cout << "win sound loaded!" << std::endl;
	}

	drawBoard();
	drawAllSprites();
	displayWindow();
	board->calculateAllMoves();

	while (gameWindow->isOpen()) {
		sf::Event event;
		if (winLoaded && board->winFX == true && once == false) {
			winFX.play();
			once = true;
		}
		if (moveLoaded == true && board->moveFX == true) {
			moveFX.play();
			board->moveFX = false;
		}
		if (online == true && board->getTurn() == board->playerTwo) {
			hostSocket.setBlocking(false);
			sf::Packet input;
			sf::Packet ack;
			if (hostSocket.receive(input) == sf::Socket::Done) {
				std::pair<int, int> data = unmakePacket(input);
				if (data.first == PROMOTION) {
					if (data.second == -1) {
						makePacket(ack, PROMOTION, BAD);
						hostSocket.send(ack);
					}
					else {
						board->promotionType = data.second;
						makePacket(ack, PROMOTION, ACK);
						hostSocket.send(ack);
					}
				}
				else if (data.first == MOVE) {
					std::pair<std::pair<int, int>, std::pair<int, int>> moveConverted = board->intConversion(data.second);
					int row1 = moveConverted.first.first;
					int col1 = moveConverted.first.second;
					int row2 = moveConverted.second.first;
					int col2 = moveConverted.second.second;
					if (board->moveCheck(row1, col1, row2, col2) != true) {
						makePacket(ack, MOVE, BAD);
						hostSocket.send(ack);
						exit(-1);
					}
					else {
						makePacket(ack, MOVE, ACK);
						hostSocket.send(ack);
						board->checkmate();
						updateBoard();
					}
				}
				else {
					makePacket(ack, CONNECT, BAD);
					hostSocket.send(ack);
					exit(-1);
				}
			}
		}
		if (board->ai != 'x' && board->getTurn() == board->ai) {
			board->aiTurn();
			updateBoard();
		}
		while (gameWindow->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				gameWindow->close();
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					row1 = 8 - (event.mouseButton.y / (int)sizeUnit);
					col1 = event.mouseButton.x / (int)sizeUnit - 1;
					if (row1 >= 0 && row1 < 8 && col1 >= 0 && col1 < 8
						&& board->grid[row1][col1]->getColour() == board->getTurn()) {
						drawValidMoves(board->grid[row1][col1]);
					}
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					row2 = 8 - (event.mouseButton.y / (int)sizeUnit);
					col2 = event.mouseButton.x / (int)sizeUnit - 1;
					if (row2 >= 0 && row2 < 8 && col2 >= 0 && col2 < 8 &&
						row1 >= 0 && row1 < 8 && col1 >= 0 && col1 < 8) {
						if (online == false) {
							if (board->moveCheck(row1, col1, row2, col2) == true) {
								board->checkmate();
							}
							updateBoard();

						}
						else if (online == true) {
							if (board->getTurn() != board->playerTwo) {
								if (board->moveCheck(row1, col1, row2, col2) == true) {
									hostSocket.setBlocking(true);
									if (board->promotionType != -1) {
										sf::Packet promotion;
										sf::Packet ack;
										makePacket(promotion, PROMOTION, board->promotionType);
										hostSocket.send(promotion);
										hostSocket.receive(ack);
										std::pair<int, int> response = unmakePacket(ack);
										if (response.first != PROMOTION || response.second != ACK) {
											exit(-1);
										}
									}
									sf::Packet move;
									sf::Packet ack;
									makePacket(move, MOVE, board->pairConversion(row1, col1, row2, col2));
									hostSocket.send(move);
									hostSocket.receive(ack);
									std::pair<int, int> response = unmakePacket(ack);
									if (response.first != MOVE || response.second != ACK) {
										exit(-1);
									}
									board->checkmate();
								}
							}
							updateBoard();
						}
					}
				}
			}
		}
	}
	return false;
}

void game::displayWindow() {
	gameWindow->display();
}

void game::drawSprite(piece* piece_) {
	sf::Sprite sprite;
	sf::Vector2f location;
	if (piece_->getColour() == 'b') {
		if (piece_->getType() == "queen") {
			sprite = blackSprites[QUEEN];
		}
		else if (piece_->getType() == "king") {
			sprite = blackSprites[KING];
		}
		else if (piece_->getType() == "knight") {
			sprite = blackSprites[KNIGHT];
		}
		else if (piece_->getType() == "bishop") {
			sprite = blackSprites[BISHOP];
		}
		else if (piece_->getType() == "pawn") {
			sprite = blackSprites[PAWN];
		}
		else if (piece_->getType() == "rook") {
			sprite = blackSprites[ROOK];
		}
	}
	else if (piece_->getColour() == 'w') {
		if (piece_->getType() == "queen") {
			sprite = whiteSprites[QUEEN];
		}
		else if (piece_->getType() == "king") {
			sprite = whiteSprites[KING];
		}
		else if (piece_->getType() == "knight") {
			sprite = whiteSprites[KNIGHT];
		}
		else if (piece_->getType() == "bishop") {
			sprite = whiteSprites[BISHOP];
		}
		else if (piece_->getType() == "pawn") {
			sprite = whiteSprites[PAWN];
		}
		else if (piece_->getType() == "rook") {
			sprite = whiteSprites[ROOK];
		}
	}
	else return;
	int row = piece_->getPosition().first;
	int col = piece_->getPosition().second;
	location.x = sizeUnit + (sizeUnit - 60.f) / 2 + col * sizeUnit; //Need to add half the difference of tile size
	location.y = sizeUnit * 8 + (sizeUnit - 60.f) / 2 - row * sizeUnit; //and sprite size for centering
	sprite.setPosition(location);
	gameWindow->draw(sprite);
}

void game::drawAllSprites() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			drawSprite(board->grid[i][j]);
		}
	}
}

void game::drawValidMoves(piece* piece_) {
	drawBoard();
	sf::CircleShape circle;
	circle.setFillColor(sf::Color(50, 205, 50, 150));
	circle.setRadius(sizeUnit / 2);
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (board->moveCheckDraw(piece_->getPosition().first, piece_->getPosition().second, row, col) == true) {
				circle.setPosition(sizeUnit + sizeUnit * col, 8 * sizeUnit - sizeUnit * row);
				gameWindow->draw(circle);
			}
		}
	}
	drawAllSprites();
	displayWindow();
}

void game::drawBoard() {
	sf::Vector2f size(sizeUnit, sizeUnit);
	sf::Vector2f location(0.f, 0.f);
	sf::RectangleShape shape(size);

	sf::Vector2f size_background((sizeUnit * 10), (sizeUnit * 10));
	sf::RectangleShape background(size_background);
	background.setFillColor(sf::Color::Color(31, 31, 31, 255));
	sf::Text text;

	text.setFont(font);
	text.setCharacterSize((int)(offSet * 2));
	text.setFillColor(sf::Color::Color(230, 230, 230, 255));

	gameWindow->clear();
	gameWindow->draw(background);
	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			if ((i + j) % 2 == 0) {
				shape.setFillColor(sf::Color::Color(230, 230, 230, 255));
			}
			else {
				shape.setFillColor(sf::Color::Color(79, 121, 66, 255));
			}
			location.x = sizeUnit + i * sizeUnit;
			location.y = sizeUnit + j * sizeUnit;
			shape.setPosition(location);
			gameWindow->draw(shape);
		}
		text.setString(std::to_string(8 - j));
		location.x = sizeUnit - offSet * 2;
		location.y = sizeUnit * 3 / 2 - offSet + j * sizeUnit;
		text.setPosition(location);
		gameWindow->draw(text);
		location.x = sizeUnit * 9 + offSet;
		location.y = sizeUnit * 3 / 2 - offSet + j * sizeUnit;
		text.setPosition(location);
		gameWindow->draw(text);
	}
	for (int i = 0; i < 8; i++) {
		std::string first(1, 65 + i);
		text.setString(first);
		location.y = sizeUnit - offSet * 3;
		location.x = sizeUnit * 3 / 2 - offSet + i * sizeUnit;
		text.setPosition(location);
		gameWindow->draw(text);
		location.y = sizeUnit * 9 + offSet;
		location.x = sizeUnit * 3 / 2 - offSet + i * sizeUnit;
		text.setPosition(location);
		gameWindow->draw(text);
	}
}

bool isNumber(const std::string& str) {
	return !str.empty() && str.find_first_not_of("0123456789.") == std::string::npos;
}

void game::makePacket(sf::Packet& packet, packetType type, int data) {
	packet << (sf::Uint64)type << (sf::Uint64)data;
}

std::pair<int, int> game::unmakePacket(sf::Packet& packet) {
	sf::Uint64 type;
	sf::Uint64 data;
	if (packet >> type >> data) {
		return std::make_pair((int)type, (int)data);
	}
	else return std::make_pair(-1, -1);;
}

void game::handShake() {
	std::string input;
	std::cout << "Enter one of the following: 'local', 'ai', 'host', 'client'" << std::endl;
	std::cin >> input;

	if (input == "local") {
		board->playerTwo = 'x';
		board->ai = 'x';
		online = false;
		host = false;
	}
	else if (input == "ai") {
		board->playerTwo = 'x';
		board->ai = 'b';
		online = false;
		host = false;
	}
	else if (input == "host") {
		board->playerTwo = 'b';
		board->ai = 'x';
		online = true;
		host = true;
	}
	else if (input == "client") {
		board->playerTwo = 'w';
		board->ai = 'x';
		online = true;
		host = false;
	}
	else {
		std::cout << "Invalid input. Exiting." << std::endl;
	}
	std::cin.clear();
	fflush(stdin);

	if (online == true && host == true) {
		sf::TcpListener listener;
		sf::IpAddress hostAddress = hostAddress.getPublicAddress();

		// bind the listener to a port
		if (listener.listen(sf::Socket::AnyPort) != sf::Socket::Done)
		{
			exit(-1);
		}
		std::cout << "You are listening on: " << hostAddress << " on port " << listener.getLocalPort() << std::endl;
		std::cout << "Waiting for a connection..." << std::endl;
		// accept a new connection
		hostSocket;
		if (listener.accept(hostSocket) != sf::Socket::Done)
		{
			exit(-1);
		}
		std::cout << "Connection accepted." << std::endl;
		sf::Packet packet;
		sf::Packet packet1;
		sf::Packet packet2;
		hostSocket.receive(packet);
		std::pair<int, int> packetInfo = unmakePacket(packet);
		if (packetInfo.first != CONNECT || packetInfo.second != START) {
			exit(-1);
		}
		makePacket(packet1, CONNECT, ACK);
		hostSocket.send(packet1);
		hostSocket.receive(packet2);
		packetInfo = unmakePacket(packet2);
		if (packetInfo.first != CONNECT || packetInfo.second != ACK) {
			exit(-1);
		}
		std::cout << "Connected to: " << hostSocket.getRemoteAddress() << std::endl;
	}
	else if (online == true && host == false) {
		unsigned short remotePort = 0;
		sf::IpAddress remoteAddress;
		std::cout << "Enter a remote IP address and port number" << std::endl;
		std::cout << "Remote Adress: [Ipaddress] [Port]" << std::endl;
		std::cin >> remoteAddress >> remotePort;
		if (!isNumber(remoteAddress.toString())) {
			std::cout << "Error in IP address" << std::endl;
			exit(-1);
		}
		if (remotePort == 0) {
			std::cout << "Error in Port" << std::endl;
			exit(-1);
		}
		std::cout << "You entered: " << remoteAddress << " " << remotePort << std::endl;
		sf::Socket::Status status = hostSocket.connect(remoteAddress, remotePort);
		if (status != sf::Socket::Done)
		{
			exit(-1);
		}
		std::cout << "Connection accepted." << std::endl;
		sf::Packet packet;
		sf::Packet packet1;
		sf::Packet packet2;
		makePacket(packet, CONNECT, START);
		hostSocket.send(packet);
		hostSocket.receive(packet1);
		std::pair<int, int> packetInfo = unmakePacket(packet1);
		if (packetInfo.first != CONNECT || packetInfo.second != ACK) {
			exit(-1);
		}
		std::cout << "Connected to: " << remoteAddress.toString() << std::endl;
		makePacket(packet2, CONNECT, ACK);
		hostSocket.send(packet2);
	}
}

void game::updateBoard() {
	drawBoard();
	drawAllSprites();
	displayWindow();
}