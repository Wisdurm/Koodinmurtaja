# Api

## CLIENT -> SERVER
This is a list of messages the client (player) can send the server:  

<table>
<tr>
  <th>Message</th>
  <th>Purpose</th>
  <th>Response</th>
</tr>
<tr>
  <th>
  <code>
  {  
    "event": "join_request",  
    "data": {  
      "name": "player name",  
      "code": "room code"  
    }  
  }  
  </code>
  </th>
  <th>
  Connects the current websocket connection to the specified room, with a specified name. 
  </th>
  <th>
  On fail
  <code>
  {
    "event": "join_response",
    "success": false,
    "error": {
      "code": "Error code",
      "msg": "Description of error"
    }
  }
  </code><br>
  On success<br>
  <code>
  {
    "event": "join_response",
    "success": true,
    "data": {
      "id": 1,
      "name": "player name",
      "players": [
          {
            "id": 2,
            "name": "other player",
          }
        ],
    }
  }
  </code><br>
  To other players (on success)<br>
  <code>
  {
    "event": "player_joined",
    "data": {
      "player": {
        "id": 1,
        "name": "player who joined",
      }
    }
  }
  </code>
  </th>
</tr>
<tr>
  <th>
    <code>
    {  
    "event": "start_request"
    }  
    </code>
  </th>
  <th>
    Indicates the player is ready for the game to start
  </th>
  <th>
  On success<br>
  <code>
  {
    "event": "start_response",
    "success": true
  }
  </code><br>
  On failure<br>
  <code>
  {
    "event": "start_response",
    "success": false,
    "error": {
      "code": "Error code",
      "msg": "Description of error"
    }
  }
  </code><br>
  To other players (on success)<br>
  <code>
  {
    "event": "player_readied",
    "data": {
      "player": {
        "id": 1,
        "name": "player who readied",
      }
    }
  }
  </code>
  </th>
</tr>
</table>

## SERVER -> CLIENT
This is a list of messages the server can send the client, WITHOUT needing a message first. That is, the server will respond to all client messages, but that
isn't listed here.

<table>
<tr>
  <th>
    Message
  </th>
  <th>
    Purpose
  </th>
  <th>
    Response
  </th>
  </tr>
<tr>
  <th>
    <code>
    {
      "event": "player_left",
      "data": {
        "player": {
          "id": id of the player who left
        }
      }
    }
    </code>
  </th>
  <th>
    Informs clients that a specified player has left the room
  </th>
  <th>
    N/A
  </th>
</tr>
</table>

## Error codes
Error code | Meaning
-|-
OCCUPIED | You are already connected to the room you are trying to connect to
JSON_ERROR | JSON request is improperly formatted
PARAM_ERROR | JSON request is missing parameters
ROOM_MISSING | The room you are trying to connect to does not exist
# Compiling
First make sure Git and Ninja are installed.  
sudo apt install git ninja-build```  
Then install and setup vcpkg.  
Start by cloning the repository:  
git clone https://github.com/microsoft/vcpkg.git```  
Then run the bootstrap script:  
cd vcpkg; .\bootstrap-vcpkg.bat``` On Windows  
cd vcpkg && ./bootstrap-vcpkg.sh``` On Linux  
Now all dependencies are setup.  
For the next steps, cd into this repository.   
Create CMakeUserPresets.json``` in the project folder,  
and paste the following:  

{
  "version": 2,
  "configurePresets": [
    {
      "name": "default",
      "inherits": "vcpkg",
      "environment": {
        "VCPKG_ROOT": "<path to vcpkg>"
      }
    }
  ]
}

Then finally, compile the program with:  
cmake --preset=default```  
cmake --build build```  

todo: copied straight from WisdomSite, check if still applies :grin: