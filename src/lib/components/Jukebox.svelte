<script lang="ts">
    import {onDestroy, onMount} from "svelte";

    let jukebox = $state<HTMLDivElement | undefined>();
    let jukeboxHeight = $state<number>(512);
    let coverSize = $derived<number>(jukeboxHeight >> 1);

    function handleJukeboxResize(resizeEntries: ResizeObserverEntry[]): void {
        console.log("Jukebox resized.");
        for (const entry of resizeEntries) {
            console.dir(entry);
        }
    }

    const resizeObserver = new ResizeObserver(handleJukeboxResize);

    onMount<() => void>(() => {
        console.log("Jukebox is mounted.");
        if (jukebox) {
            console.log("Observing Jukebox resizes.");
            resizeObserver.observe(jukebox, {
                box: "device-pixel-content-box"
            });
        }

        return (): void => {
            console.log("Disconnecting Jukebox resize observer.");
            resizeObserver.disconnect();
        };
    });
</script>

<div bind:this={jukebox} class="jukebox" style="--cover-size: {coverSize}px;">
    <div class="album-cover active"></div>
</div>

<style>
.jukebox {
    width: 100%;
    height: 100%;
    background: black;
}

.album-cover {
    perspective: 20cm;
    height: var(--cover-size);
    width: var(--cover-size);
    background-color: red;
}

.active {
    transform: translateX(0);
}
</style>