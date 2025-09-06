#include "room.h"

uint8_t Card::getByte() const
{
    return CARD_PER_SUIT * static_cast<uint8_t>(suit) + value;
};

bool Card::guess(uint8_t value)
{
    if (value == this->value)
        guessed = true;
    return guessed;
}
	
Player::Player(std::string name, std::stack<Card>& cards, crow::websocket::connection* conn, int id) : name(name), connection(conn), id(id), unguessedCards(0)
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

bool Player::guess(uint8_t value)
{
    bool r = false;
    for (Card& card : cards)
    {
        if (card.guess(value))
        {
            unguessedCards--;
            r = true;
        }
    }
    return r;
}

void Player::addCard(Card card)
{
    cards.push_back(card);
    unguessedCards++;
}

Room::Room(int suits) : suits(suits)
{
    idCounter = 0;
    gameStarted = false;
    turn = INT_MAX;
    // Create deck
    for (int i = 0; i < 52; i++) { cards.push(i);}
}

void Room::startGame()
{
    gameStarted = true;
    readyStates.clear();
    
    json playersJson;
    // Get a list of all players
    for (std::pair<int, Player> player : users)
    {
        playersJson.push_back(
            {
                {"id", player.second.getId()},
                {"name", player.second.getName()}
            });
        // If has the lowest id, this player starts the game
        if (turn == INT_MAX)
            turn = player.second.getId();
    }
    // Tell everyone game has started
    for (std::pair<int, Player> player : users)
    {
        // Basic info
        json report =
        {
            {"event", "game_start"},
            {"data",{
                {"players", playersJson},
                {"turn", turn},
                {"cards", {}}
            }}
        };      
        // Cards
        for (Card card : player.second.getCards())
        {
            json cardJson = {
                {"card", card.getByte()},
                {"guessed", card.isGuessed()}
            };
            report["data"]["cards"].push_back(cardJson);
            // TODO: Sort
        }
        player.second.sendMessage(report.dump());
        // If turn, inform player
        if (player.second.getId() == turn)
        {
            json turnReport = {
                {"event", "your_turn"},
                {"data",{
                    {"cards", report["data"]["cards"]} // Just reuse this while we can
                }}
            };
            player.second.sendMessage(turnReport.dump());
        }
    }
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
    for (std::pair<int, Player> pair : users)
    {
        players.push_back(pair.second);
    }
    return players;
}