## Server API

### Message Format
```json
{
  "event": "string",
  "data": {},
  "success": true,
  "error": {"code": "string", "msg": "string"}
}
```

### Events

#### `join_request` → `join_response`

Connects the current websocket connection to the specified room, with a specified name. 

```json
// Request
{"event": "join_request", "data": {"name": "<player_name>", "code": "<room_code>"}}

// Success Response
{
  "event": "join_response", 
  "success": true, 
  "data": {
    "id": 1,
    "name": "<player_name>", 
    "players": [{"id": 2, "name": "<other_player>"}]
  }
}

// Error Response
{
  "event": "join_response", 
  "success": false, 
  "error": {"code": "<error_code>", "msg": "<description_of_error>"}
}

// Broadcast
{"event": "player_joined", "data": {"player": {"id": 1, "name": "<player_who_joined>"}}}
```

#### `start_request` → `start_response`

Indicates the player is ready for the game to start

```json
// Request
{"event": "start_request"}

// Success Response
{"event": "start_response", "success": true}

// Error Response
{
  "event": "start_response", 
  "success": false, 
  "error": {"code": "<error_code>", "msg": "<description_of_error>"}
}

// Broadcast
{"event": "player_readied", "data": {"player": {"id": 1, "name": "<player_who_readied>"}}}
```

#### Server Events

Informs clients that a specified player has left the room

```json
// Player disconnection
{"event": "player_left", "data": {"player": {"id": "<id_of_the_player_who_left>"}}}
```

### Error Codes
- `OCCUPIED` - You are already connected to the room you are trying to connect to
- `INTRUDER` - You are trying to send a request that would require you to be connected to a room, but you are not connected to any
- `JSON_ERROR` - JSON request is improperly formatted  
- `PARAM_ERROR` - JSON request is missing parameters
- `ROOM_MISSING` - The room you are trying to connect to does not exist

### Build

**Setup**:  
```bash
sudo apt install git ninja-build
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg && ./bootstrap-vcpkg.sh  # Linux
cd vcpkg && .\bootstrap-vcpkg.bat  # Windows
```
For the next steps, clone this repository, and cd in to the ```server``` directory.  
Create a file called ```CMakeUserPresets.json```, and paste the following.  
**CMakeUserPresets.json**:
```json
{
  "version": 2,
  "configurePresets": [{
    "name": "default", 
    "inherits": "vcpkg",
    "environment": {"VCPKG_ROOT": "<path_to_vcpkg>"}
  }]
}
```
**Build**:
```bash
cmake --preset=default
cmake --build build
```
