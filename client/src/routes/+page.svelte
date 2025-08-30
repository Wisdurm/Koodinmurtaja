<script lang="ts">
    import autoAnimate from '@formkit/auto-animate';

    type Card = { value: number };
    type Enemy = { id: number; name: string; cards: Card[] };

    const suits = ["risti", "hertta", "pata", "ruutu"];

    let enemys: Enemy[] = [
        { id: 1, name: "Luukas", cards: [
            { value: 13 },
            { value: 14 },
            { value: 29 },
            { value: 15 },
            { value: 34 }
        ]},
        { id: 2, name: "Matias", cards: [
            { value: 4 },
            { value: 17 },
            { value: 36 },
            { value: 46 },
            { value: 42 }
        ]},
        { id: 3, name: "Eetu", cards: [
            { value: 12 },
            { value: 25 },
            { value: 18 },
            { value: 30 },
            { value: 51 }
        ]},
    ];

    let own = { id: 0, name: "Luka", cards: [
        { value: 4 },
        { value: 17 },
        { value: 29 },
        { value: 18 },
        { value: 46 }
    ]};

    const positions = [
        "top-4 left-4", // vase
        "top-4 left-1/2 transform -translate-x-1/2", // keskipiste
        "top-4 right-4", // oikea
        "top-1/3 left-1/2 transform -translate-y-1/2 -translate-x-1/2" // keskellä
    ];

    // Muuttaa kortin arvon (0-51) kortin numeroksi (2-14, missä 14=Ässä)
    // Esim kortti 13 -> numero 2, koska edellinen maa päättyi 12 ja pienin mahdollinen kortti on kaksi, kortti 25 -> numero 14 (Ässä)
    function getCardValue(card: Card): number {
        return (card.value % 13) + 2; // 2-14, lisäätään 2, koska näkyy käyttäjälle
    }
 
    /*
    // KORTIT 0-12: "PATA", 13-25: "HERTTA", 26-38: "RISTI", 39-51: "RUUTU"!!
    */

   // Muuttaa kortin arvon (0-51) maan nimeksi suits-arraysta
    function getCardSuit(card: Card): string {
        return suits[Math.floor(card.value / 13) % 4]; // TODO: make changeable 
    }

    function removeCard(enemyId: number, cardValue: number) {
        enemys = enemys.map(enemy => {
            if (enemy.id === enemyId) {
                return { 
                    ...enemy, 
                    cards: enemy.cards.filter(card => card.value !== cardValue) 
                };
            }
            return enemy;
        });
    }

    function getCardKey(card: Card) {
        return `${card.value}`;
    }

</script>

<main class="h-screen relative overflow-hidden">
    <!-- viholliset -->
    {#each enemys as enemy, i}
        <div class={`absolute ${positions[i]} text-center`}>
            <div>{enemy.name}</div>
            <ul class="grid grid-cols-5 gap-1" use:autoAnimate>
                {#each enemy.cards as card (getCardKey(card))}
                    <li class="w-14 h-24 bg-gray-500 flex flex-col items-center justify-center text-xs">
                        <div>{getCardValue(card)}</div>
                        <div class="text-xs">{getCardSuit(card)}</div>
                        <button class="text-red-500" on:click={() => removeCard(enemy.id, card.value)}>
                            r
                        </button>
                    </li>
                {/each}
            </ul>
        </div>
    {/each}

    <!-- omat -->
    <div class="absolute bottom-4 left-1/2 transform -translate-x-1/2 text-center">
        <div>{own.name}</div>
        <div class="grid grid-cols-5 gap-1">
            {#each own.cards as card (getCardKey(card))}
                <div class="w-14 h-24 bg-gray-500 flex flex-col items-center justify-center text-xs">
                    <div>{getCardValue(card)}</div>
                    <div class="text-xs">{getCardSuit(card)}</div>
                </div>
            {/each}
        </div>
    </div>
</main>