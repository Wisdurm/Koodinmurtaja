#pragma once

#include "crow.h"
#include "room.h"

class Player;

// Maps a websocket connection against a player
void mapPlayer(crow::websocket::connection* conn, Player* player);
// Unmaps a websocket connection from a player
void unmapPlayer(crow::websocket::connection* conn);