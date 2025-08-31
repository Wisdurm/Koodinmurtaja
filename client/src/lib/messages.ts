import { socket } from './websocket.js';
import { appState } from './stores.svelte.js';
import type { WebSocketMessage, JoinResponseData, PlayerJoinedData } from './types.js';

export function setupMessageHandlers() {
    socket.addEventListener('message', (event) => {
        try {
            const data: WebSocketMessage = JSON.parse(event.data);
            console.log('received message:', data);
            
            appState.addMessage(data);
            
            switch (data.event) {
                case 'join_response':
                    handleJoinResponse(data);
                    break;
                case 'player_joined':
                    handlePlayerJoined(data);
                    break;
                default:
                    console.log('Unknown event:', data.event);
            }
        } catch (e) {
            console.error('error parsing message:', event.data, e);
            appState.lastError = 'Failed to parse server message 🐟';
        }
    });
}

function handleJoinResponse(data: WebSocketMessage) {
    if (data.success) {
        const responseData = data.data as JoinResponseData;
        console.log('joined room:', responseData);
        
        appState.joinState.isJoining = false;
        appState.joinState.isJoined = true;
        appState.joinState.roomCode = responseData.roomCode;
        appState.joinState.playerName = responseData.playerName;
        
        appState.lastError = null;
    } else {
        console.error('failed to join room:', data.error);
        
        appState.joinState.isJoining = false;
        appState.joinState.isJoined = false;
        
        appState.lastError = 'Huoneeseen liittyminen epäonnistu';
    }
}

function handlePlayerJoined(data: WebSocketMessage) {
    const playerData = data.data as PlayerJoinedData;
    console.log('player joined:', playerData);
}
