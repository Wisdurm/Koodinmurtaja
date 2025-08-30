#include "crow.h"
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <iostream>
// Game
#include "room.h"

int main()
{
    crow::SimpleApp app;

    std::mutex mtx;
    std::unordered_map<std::string, Room> rooms;
    std::unordered_map<crow::websocket::connection*, Room*> userConnections;

    CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) {
          CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
          std::lock_guard<std::mutex> _(mtx);
      })
      .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
          CROW_LOG_INFO << "websocket connection closed: " << reason;
          std::lock_guard<std::mutex> _(mtx);
      })
      .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
          std::lock_guard<std::mutex> _(mtx);

          switch (data[0])
          {
          case 'J': // Player joins room 
            {
                int splice = data.find(':');
                std::string code = data.substr(0, splice);
                std::string name = data.substr(splice+1);
                CROW_LOG_INFO << "Player: " << name << " joining room: " << code;
                if (rooms.find(code) != rooms.end()) // Room exists
                {
                    CROW_LOG_INFO << "Succesfully joined room!";
                    rooms.at(code).AddPlayer(&conn, name);
                }
                else
                {
                    CROW_LOG_INFO << "Cannot find room";
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