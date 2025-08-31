#include "room.h"

uint8_t Card::getByte()
{
    return CARD_PER_SUIT * static_cast<uint8_t>(suit) + value;
};
	
Player::Player(std::string name, std::stack<Card>& cards, crow::websocket::connection* conn, int id) : name(name), connection(conn), id(id)
{
    // Draw 5 cards
    for (int i = 0; i < 5; i++)
    {
        this->cards.push_back(cards.top());
        cards.pop();
        unguessedCards++;
    }
};

void Player::sendMessage(std::string msg)
{
    connection->send_text(msg);
}

Room::Room(int suits) : suits(suits)
{
    gameStarted = false;
    turn = 0;
    // Create deck
    for (int i = 0; i < 52; i++) { cards.push(i);}
}

void Room::AddPlayer(crow::websocket::connection* conn, std::string name)
{
    users.push_back(Player(name, cards, conn, users.size()));
}

int Room::getId()
{
    return users.size()-1;
}