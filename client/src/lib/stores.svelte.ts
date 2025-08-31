import type { ConnectionState, WebSocketMessage } from './types.js';

// Pelin tila   
class AppState {
    connectionState = $state<ConnectionState>('connecting');
    messages = $state<WebSocketMessage[]>([]);
    lastError = $state<string | null>(null);
    joinState = $state<{
        isJoining: boolean;
        isJoined: boolean;
        roomCode: string | null;
        playerName: string | null;
    }>({
        isJoining: false,
        isJoined: false,
        roomCode: null,
        playerName: null
    });

    get isConnected() {
        return this.connectionState === 'connected';
    }

    addMessage(message: WebSocketMessage) {
        this.messages.unshift(message);
    }
}

export const appState = new AppState();
