#pragma once

#include "crow.h"
#include <vector>
#include <stack>
#include <map>
#include <cmath>
#include <nlohmann/json.hpp>
// Game
#include "main.h"

using namespace nlohmann;

constexpr uint8_t CARD_PER_SUIT = 13;
enum class Suits {DIAMOND, CLUB, HEART, SPADE};

class Card
{
private:
	Suits suit;
	uint8_t value;
	bool guessed;
public:
	Card(Suits suit, uint8_t value) : suit(suit), value(value), guessed(false) {};
	Card(uint8_t rep) : suit(static_cast<Suits>(floor(static_cast<float>(rep)/CARD_PER_SUIT))), value(rep % CARD_PER_SUIT), guessed(false) {};
	// Returns a single byte representation of the card
	uint8_t getByte() const;
	// Guesses a specific value of the card
	bool guess(uint8_t value);
	// Whether or not the card has been guessed
	bool isGuessed() const { return guessed; };
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
	int getId() const { return id; };
	// Returns name
	std::string& getName() { return name; }
	// Send the player a message
	void sendMessage(std::string msg);
	// Returns the connection of the player
	crow::websocket::connection* getConnection() const {return connection;};
	// Returns the players cards
	std::vector<Card>& getCards() { return cards; };
	// Guesses a value from a players cards
	bool guess(uint8_t value);
	// Adds card to player
	void addCard(Card card);
};

// Room which hosts a game
class Room
{
private:
	// Vector which maps all users ids to a player object
	std::unordered_map<int, Player> users;
	// The amount of suits in use in the current game
	uint8_t suits;
	// The draw pile
	std::stack<Card> cards;
	// Which players turn it currently is
	int turn;
	// Whether or not the game has started yet
	bool gameStarted;
	// Whether or not players are ready for the game to start
	std::vector<bool> readyStates;
	// Counter which ticks up, assigning new ids
	int idCounter;
public:
	Room(int suits);
	// Adds player to room
	void AddPlayer(crow::websocket::connection* conn, std::string name);
	// Returns the most recently added player's  id
	int getId() const {return idCounter - 1;};
	// Retrieves the list of players
	std::vector<Player> getPlayers();
	// Retrieves the list of player ready states
	std::vector<bool>& getReadyStates() {return readyStates; };
	// Whether or not the game has started yet
	bool gameOn() const {return gameStarted; };
	// Removes a player by their websocket connection
	void removePlayer(crow::websocket::connection* conn);
	// Starts the game
	void startGame();
};

