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
	// Websocket connection of the player
	crow::websocket::connection* connection;
	// Per room unique id
	int id;
public:
	// Default constructor
	Player(std::string name, std::stack<Card>& cards, crow::websocket::connection* conn, int id);
	// Returns id
	int getId() { return id; };
	// Returns name
	std::string& getName() { return name; }
	// Send the player a message
	void sendMessage(std::string msg);
};

// Room which hosts a game
class Room
{
private:
	// Vector which maps all users ids to a player object
	std::vector<Player> users;
	// The amount of suits in use in the current game
	uint8_t suits;
	// The draw pile
	std::stack<Card> cards;
	// Which players turn it currently is
	int turn;
	// Whether or not the game has started yet
	bool gameStarted;
public:
	Room(int suits);
	// Adds player to room
	void AddPlayer(crow::websocket::connection* conn, std::string name);
	// Returns the most recently added player's  id
	int getId();
	// Retrieves the list of players
	const std::vector<Player>& getPlayers() {return users; };
	// Whether or not the game has started yet
	bool gameOn() {return gameStarted; };
};

