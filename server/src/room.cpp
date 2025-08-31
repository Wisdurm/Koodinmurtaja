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
    idCounter = 0;
    gameStarted = false;
    turn = 0;
    // Create deck
    for (int i = 0; i < 52; i++) { cards.push(i);}
}

void Room::AddPlayer(crow::websocket::connection* conn, std::string name)
{
    users.insert({idCounter, Player(name, cards, conn, idCounter)});
    idCounter++;
    readyStates.push_back(false);
    mapPlayer(conn, &(users.at(idCounter-1)));
}
	
void Room::removePlayer(crow::websocket::connection* conn)
{
    unmapPlayer(conn);
    std::unordered_map<int, Player>::iterator it = users.begin();
    while (it != users.end())
    {
        if (it->second.getConnection() == conn)
        {
            it = users.erase(it); // Kind of useless since we break out anyway...
            break;
        }
        else
            ++it;
    }
}

std::vector<Player> Room::getPlayers()
{
    std::vector<Player> players;
    for (auto pair : users)
    {
        players.push_back(pair.second);
    }
    return players;
}