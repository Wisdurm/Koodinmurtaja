#include "room.h"

uint8_t Card::getByte()
{
    return CARD_PER_SUIT * static_cast<uint8_t>(suit) + value;
};
	
Player::Player(std::string name, std::stack<Card>& cards) : name(name)
{
    // Draw 5 cards
    for (int i = 0; i < 5; i++)
    {
        this->cards.push_back(cards.top());
        cards.pop();
        unguessedCards++;
    }
};

Room::Room(int suits) : suits(suits)
{
    // Create shuffled deck
    int addedTotal = 0;
    bool added[52];
    for (int i = 0; i < 52; i++) {added[i]=false;} // Necessary to avoid UB
    int rnd = 0;
    while (addedTotal < 52)
    {
        rnd += rand() % 52;
        if (rnd > 52)
            rnd = rnd%52;
        
        if (added[rnd] == false) // Add card if not already added
        {
            cards.push(Card(rnd));
            added[rnd] = true;
            addedTotal++;
        }
    }
}

void Room::AddPlayer(crow::websocket::connection* conn, std::string name)
{
    users.insert({conn, Player(name, cards)});
}