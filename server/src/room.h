#include "crow.h"
#include <vector>
#include <stack>
#include <map>
#include <cmath>

constexpr uint8_t CARD_PER_SUIT = 13;
enum class Suits {DIAMOND, CLUB, HEART, SPADE};

class Card
{
private:
	Suits suit;
	uint8_t value;
public:
	Card(Suits suit, uint8_t value) : suit(suit), value(value) {};
	Card(uint8_t rep) : suit(static_cast<Suits>(floor(static_cast<float>(rep)/CARD_PER_SUIT))), value(value % CARD_PER_SUIT) {};
	// Returns a single byte representation of the card
	uint8_t getByte();
};

// Player object
class Player
{
private:
	// Cards the player has
	std::vector<Card> cards;
	// The amount of cards a player has that have yet to be guessed
	uint8_t unguessedCards;
	// Name of the player
	std::string name;
public:
	// Default constructor
	Player(std::string name, std::stack<Card>& cards);
};

// Room which hosts a game
class Room
{
private:
	// Dictionary which maps all users connected to the room to a player object
	std::map<crow::websocket::connection*, Player> users;
	// The amount of suits in use in the current game
	uint8_t suits;
	// The draw pile
	std::stack<Card> cards;
public:
	Room(int suits);
	// Adds player to room
	void AddPlayer(crow::websocket::connection* conn, std::string name);
};

