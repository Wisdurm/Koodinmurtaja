#include "crow.h"
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <iostream>
#include <nlohmann/json.hpp>
// Game
#include "room.h"
#include "main.h"

using namespace nlohmann;

// Dictionary which contains every room, with their roomcode being their key
static std::unordered_map<std::string, Room> rooms;
// Dictionary which maps every active websocket connection to the room they're in
static std::unordered_map<crow::websocket::connection*, Room*> userConnections;
// Dictionary which maps websocket connections to their player
static std::unordered_map<crow::websocket::connection*, Player*> userPlayers;
// Dictionary which maps event strings to their int representations
static std::unordered_map<std::string, int> reqMap = {
    {"join_request", 0},
    {"start_request", 1},
};

// Maps a websocket connection against a player
void mapPlayer(crow::websocket::connection* conn, Player* player)
{
    userPlayers.insert({conn, player});
}
// Unmaps a websocket connection from a player
void unmapPlayer(crow::websocket::connection* conn)
{
    userPlayers.erase(conn);
}

int main()
{
    crow::SimpleApp app;

    std::mutex mtx;
    // Default room for debugging
    rooms.insert({"TEST", Room(3)});

    CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) {
          CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
          std::lock_guard<std::mutex> _(mtx);
      })
      .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
          CROW_LOG_INFO << "websocket connection closed: " << reason;
          std::lock_guard<std::mutex> _(mtx);
          //REMOVE PLAYER
          if (userConnections.find(&conn) != userConnections.end()) // If player is in a room
          {
            // Get some info
            Room& room = *userConnections.at(&conn);
            int playerId = userPlayers.at(&conn)->getId();
            CROW_LOG_INFO << "Player: " << playerId << " left room";
            // Eject player
            userConnections[&conn]->removePlayer(&conn); // This handless userPlayers as well
            userConnections.erase(&conn);
            // Inform other players
            json report =
            {
                {"event", "player_left"},
                {"data",{
                    {"player",{
                        {"id", playerId},
                    }}
                }}
            };
            for (Player& player : room.getPlayers())
            {
                player.sendMessage(report.dump());
            }
          }
      })
      .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
          std::lock_guard<std::mutex> _(mtx);
          
          json req;
          try
          {
            req = json::parse(data);
          }
          catch (json::parse_error)
          {
            json error =
            {
                {"event", "join_response"},
                {"success", false},
                {"error",{
                    {"code", "JSON_ERROR"},
                    {"msg", "Unable to parse JSON request"}
                }}
            };
            conn.send_text(error.dump());
            return;
          }

          json::iterator event = req.find("event");
          if (event == req.end()) // Does message contain imporant "event" field
          {
              json error =
              {
                  {"event", "join_response"},
                  {"success", false},
                  {"error",{
                      {"code", "PARAM_ERROR"},
                      {"msg", "Unable to find necessary JSON params"}
                  }}
              };
              conn.send_text(error.dump());
              return;
          }
          // Switch based on event type
          switch (reqMap.at(*event))
          {
          case 0: // Player joins room 
            {
                if (userConnections.find(&conn) != userConnections.end())
                {
                    json error =
                    {
                        {"event", "join_response"},
                        {"success", false},
                        {"error",{
                            {"code", "OCCUPED"},
                            {"msg", "Player is already in another room"}
                        }}
                    };
                    conn.send_text(error.dump());
                    return;
                }

                std::string code;
                std::string name;
                try
                {
                    code = req["data"]["code"];
                    name = req["data"]["name"];
                }
                catch(json::type_error)
                {
                    json error =
                    {
                        {"event", "join_response"},
                        {"success", false},
                        {"error",{
                            {"code", "PARAM_ERROR"},
                            {"msg", "Unable to find necessary JSON params"}
                        }}
                    };
                    conn.send_text(error.dump());
                    return;
                }
                if (rooms.find(code) == rooms.end()) // Room does not exist
                {
                    json error =
                    {
                        {"event", "join_response"},
                        {"success", false},
                        {"error",{
                            {"code", "ROOM_MISSING"},
                            {"msg", "Unable to find room"}
                        }}
                    };
                    conn.send_text(error.dump());
                    return;
                }
                // Continue if everything is fine

                CROW_LOG_INFO << "Player: " << name << " joined room: " << code;
                // Add player to room
                Room& room = rooms.at(code);
                room.AddPlayer(&conn, name);
                userConnections.insert({&conn, &room});
                int playerId = userPlayers[&conn]->getId(); // Could use room.getId() here :/
                // Return success message
                json players;
                json success =
                {
                    {"event", "join_response"},
                    {"success", true},
                    {"data",{
                        {"id", playerId},
                        {"name", name},
                    }}
                };
                // Generate list of all currently connected players
                auto listPlayers = room.getPlayers();
                for (Player& player : listPlayers)
                {
                    players.push_back(
                        {
                            {"id", player.getId()},
                            {"name", player.getName()}
                        });   
                }
                success["data"]["players"] = players;
                conn.send_text(success.dump());
                // Inform other players of the connection
                json report =
                {
                    {"event", "player_joined"},
                    {"data", {
                        {"player", {
                            {"id", playerId},
                            {"name", name}
                        }},
                    }}
                };
                for (Player& player : listPlayers)
                {
                    if (player.getId() != playerId) // Tell everyone except the new user
                    {
                        player.sendMessage(report.dump());
                    }
                }
                return;
                
                break;
            }
          case 1: // Player indicates they are ready 
            {
                // Check if player is connected
                std::unordered_map<crow::websocket::connection*, Player*>::iterator userPlayer = userPlayers.find(&conn);
                if (userPlayer == userPlayers.end()) // Connection does not map to a player
                {
                    json error =
                    {
                        {"event", "start_response"},
                        {"success", false},
                        {"error",{
                            {"code", "INTRUDER"},
                            {"msg", "Player is not in any room"}
                        }}
                    };
                    conn.send_text(error.dump());
                    return;
                }

                // Continue if everything is fine
                Player& readier = *userPlayer->second;
                userConnections[&conn]->getReadyStates().at(readier.getId()) = true;
                json success =
                {
                    {"event", "start_response"},
                    {"success", true}
                };
                conn.send_text(success.dump());
                // Tell other players
                Room& room = *userConnections[&conn];
                json report =
                {
                    {"event", "player_readied"},
                    {"data", {
                        {"player", {
                            {"id", readier.getId()},
                            {"name", readier.getName()}
                        }},
                    }}
                };
                for (Player& player : room.getPlayers())
                {
                    if (player.getId() != readier.getId()) // Tell everyone except the player who readied
                    {
                        player.sendMessage(report.dump());
                    }
                }

                break;
            }
          default:
            {
                break;
            }
          }

          /*
          for (auto u : users)
              if (is_binary)
                  u->send_binary(data);
              else
                  u->send_text(data);
          */
        });

    CROW_ROUTE(app, "/")
    ([] {
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;

        auto page = crow::mustache::load("ws.html");
        return page.render(x);
    });

    app.port(18080)
      .multithreaded()
      .run();
}