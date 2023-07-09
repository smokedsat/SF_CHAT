#include "message.h"
#include "user.h"

#include "server_interface.h"

#include <vector>

struct server_with_mysql
{
    server_interface server;
    

    std::vector<std::vector<Message>> messages;

    server_with_mysql()
    {
        server.run();




    }
};