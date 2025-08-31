export { socket } from './websocket.js';

export { joinRoom } from './api.js';

export { appState } from './stores.svelte.js';

export type * from './types.js';

import { setupMessageHandlers } from './messages.js';

setupMessageHandlers();