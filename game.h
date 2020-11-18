#pragma once
#include "chess_board.h"

enum responseCodes { ACK = 10, BAD = 11, START = 12 }; //can be put in data field

enum packetType { CONNECT = 20, MOVE = 21, PROMOTION = 22};

enum finishWork { MORE = 30, FINISHED  = 31};

class chess_board;
class game {
	chess_board* board;
	sf::RenderWindow* gameWindow;
	sf::Font font;
	sf::Texture texture;
	bool loadSuccess;
public:
	sf::Sprite whiteSprites[6];
	sf::Sprite blackSprites[6];
	float sizeUnit = 70;
	float offSet = 12;
	bool online;
	bool host;
	sf::TcpSocket hostSocket;
	game();
	~game();

	bool gameLoop();
	void drawSprite(piece* piece_);
	void drawAllSprites();
	void drawValidMoves(piece* piece_);
	void drawBoard();
	void displayWindow();
	void makePacket(sf::Packet & packet, packetType type, int data);
	std::pair<int,int> unmakePacket(sf::Packet& packet);
	bool send(sf::UdpSocket & socket, sf::Packet & packet);
	int receive(sf::UdpSocket & socket, sf::Packet & packet);
	void handShake();
	void updateBoard();
};
