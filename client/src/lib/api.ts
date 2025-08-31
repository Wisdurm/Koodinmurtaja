import { socket } from './websocket.js';
import { appState } from './stores.svelte.js';
import type { JoinRequestData, WebSocketMessage } from './types.js';

export function joinRoom(name: string, roomCode: string): boolean {
    if (!name.trim() || !roomCode.trim()) {
        appState.lastError = 'Anna nimi ja koodi';
        return false;
    }
    
    if (appState.connectionState !== 'connected') {
        appState.lastError = 'Yhistettynä palvelimeen';
        return false;
    }
    
    if (appState.joinState.isJoining) {
        appState.lastError = 'Äläpä hoppuile';
        return false;
    }
    
    try {
        const message: WebSocketMessage = {
            event: "join_request",
            data: {
                name: name.trim(),
                code: roomCode.trim()
            } as JoinRequestData
        };
        
        console.log('sending join request:', message);
        socket.send(JSON.stringify(message));
        
        appState.joinState.isJoining = true;
        
        setTimeout(() => {
            if (appState.joinState.isJoining && !appState.joinState.isJoined) {
                appState.joinState.isJoining = false;
                appState.lastError = 'Liittyminen aikakatkesi';
            }
        }, 10000); // 10 s
        
        appState.lastError = null;
        return true;
    } catch (error) {
        console.error('failed to send join request:', error);
        appState.lastError = 'Pynnön lähettäminen ei onnistunu';
        return false;
    }
}
