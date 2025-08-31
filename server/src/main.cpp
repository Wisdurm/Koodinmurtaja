#include "crow.h"
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <iostream>
#include <nlohmann/json.hpp>
// Game
#include "room.h"

using namespace nlohmann;

int main()
{
    crow::SimpleApp app;

    std::mutex mtx;
    // Dictionary which contains every room, with their roomcode being their key
    std::unordered_map<std::string, Room> rooms;
    // Dictionary which maps every active websocket connection to the room they're in
    std::unordered_map<crow::websocket::connection*, Room*> userConnections;
    // Dictionary which maps event strings to their int representations
    std::unordered_map<std::string, int> reqMap = {
        {"join_request", 0},
    };

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
            CROW_LOG_INFO << "Player: " << conn.get_remote_ip() << " left room ";
            userConnections[&conn]->removePlayer(&conn);
            userConnections.erase(&conn);
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

          switch (reqMap[req["event"].dump()])
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
                if (rooms.find(code) != rooms.end()) // Room exists
                {
                    CROW_LOG_INFO << "Player: " << name << " joined room: " << code;
                    // Add player to room
                    Room& room = rooms.at(code);
                    room.AddPlayer(&conn, name);
                    userConnections.insert({&conn, &room});
                    // Return success message
                    json players;
                    json success =
                    {
                        {"event", "join_response"},
                        {"success", true},
                        {"data",{
                            {"id", room.getId()},
                            {"name", name},
                            {"game_on", room.gameOn() }
                        }}
                    };
                    for (Player player : room.getPlayers())
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
                    for (Player player : room.getPlayers())
                    {
                        if (player.getId() != room.getId()) // Tell everyone except the new user
                        {
                            json report =
                            {
                                {"event", "player_joined"},
                                {"data", {
                                    {"player", {
                                        {"id", room.getId()},
                                        {"name", name}
                                    }},
                                    {"game_on", room.gameOn()},
                                    {"players", players}
                                }}
                            };
                            player.sendMessage(report.dump());
                        }
                    }
                    return;
                }
                else
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
                
                break;
            }
          default:
            break;
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