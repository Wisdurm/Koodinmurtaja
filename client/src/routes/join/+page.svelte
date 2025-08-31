<script lang="ts">
  import { joinRoom, appState } from '$lib/index.js';
  import type { WebSocketMessage } from '$lib/index.js';
  
  let name = $state('');
  let roomCode = $state('');

  const canJoin = $derived(
    appState.isConnected && 
    !appState.joinState.isJoining && 
    name.trim() && 
    roomCode.trim()
  );
  
  const buttonText = $derived(
    appState.joinState.isJoining ? 'Liitytään...' : 'Liity huoneeseen'
  );

  function handleJoinRoom() {
    if (canJoin) {
      joinRoom(name, roomCode);
    }
  }

  function formatMessage(msg: WebSocketMessage): string {
    const timestamp = new Date().toLocaleTimeString();
    return `[${timestamp}] ${msg.event}: ${JSON.stringify(msg.data || msg, null, 2)}`;
  }

  function getConnectionStatusText(state: string): string {
    switch (state) {
      case 'connecting': return 'Yhdistetään...';
      case 'connected': return 'Yhdistetty';
      case 'disconnected': return 'Katkaistu';
      case 'error': return 'Yhteysvirhe';
      default: return 'Tuntematon';
    }
  }
</script>

<div>
  <div>
    <h1>Liity huoneeseen</h1>
    <div>
      Tila: {getConnectionStatusText(appState.connectionState)}
    </div>
  </div>

  <div>
    {#if appState.joinState.isJoined}
      <div>
        Liitytty huoneeseen {appState.joinState.roomCode}
      </div>
    {:else}
      <div>
        <label for="name">Nimi</label>
        <input 
          id="name" 
          type="text" 
          bind:value={name} 
          disabled={appState.joinState.isJoining || !appState.isConnected}
        />
      </div>

      <div>
        <label for="room">Huoneen koodi</label>
        <input 
          id="room" 
          type="text" 
          bind:value={roomCode} 
          disabled={appState.joinState.isJoining || !appState.isConnected}
        />
      </div>
      
      <button 
        onclick={handleJoinRoom} 
        disabled={!canJoin}
      >
        {buttonText}
      </button>

      {#if appState.lastError}
        <div>
          {appState.lastError}
        </div>
      {/if}
    {/if}
  </div>
    <!-- temp -->
    <div>
      {#if appState.messages.length === 0}
        <div>No messages yet...</div>
      {:else}
        {#each appState.messages as message (message)}
          <div>
            {formatMessage(message)}
          </div>
        {/each}
      {/if}
    </div>
</div>
