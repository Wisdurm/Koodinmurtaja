export interface WebSocketMessage {
    event: string;
    data?: any;
    success?: boolean;
    error?: string;
}

export interface JoinRequestData {
    name: string;
    code: string;
}

export interface JoinResponseData {
    roomCode: string;
    playerName: string;
}

export interface PlayerJoinedData {
    playerName: string;
    roomCode: string;
}

export type ConnectionState = 'connecting' | 'connected' | 'disconnected' | 'error';
