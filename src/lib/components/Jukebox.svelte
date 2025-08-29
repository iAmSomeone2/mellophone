<script lang="ts">
    import {onDestroy, onMount} from "svelte";

    let jukebox = $state<HTMLDivElement | undefined>();
    let jukeboxHeight = $derived<number>(jukebox?.clientHeight ?? 0);
    let coverSize = $derived<number>(jukeboxHeight >> 1);

    function handleJukeboxResize(resizeEntries: ResizeObserverEntry[]): void {
        for (const entry of resizeEntries) {
            console.dir(entry);
        }
    }

    const resizeObserver = new ResizeObserver(handleJukeboxResize);

    // onMount(() => {
    //     if (jukebox) {
    //         resizeObserver.observe(jukebox, {
    //             box: "device-pixel-content-box"
    //         });
    //     }
    // });
    //
    // onDestroy(() => {
    //     resizeObserver.disconnect();
    // });
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