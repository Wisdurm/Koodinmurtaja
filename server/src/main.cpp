#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]
        (const crow::request& req){       
	    return "Hello world!";
    });

    app.port(18080).multithreaded().run();
}
