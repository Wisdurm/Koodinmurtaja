import { appState } from './stores.svelte.js';

const wsUrl = "ws://127.0.0.1:18080/ws";
export const socket = new WebSocket(wsUrl);

socket.addEventListener('open', (event) => {
    console.log('connected');
    appState.connectionState = 'connected';
    appState.lastError = null;
});

socket.addEventListener('close', (event) => {
    console.log('disconnected');
    appState.connectionState = 'disconnected';
});

socket.addEventListener('error', (event) => {
    console.error('error:', event);
    appState.connectionState = 'error';
    appState.lastError = 'Yhteysvirhe, luukaksen ongelma';
});
