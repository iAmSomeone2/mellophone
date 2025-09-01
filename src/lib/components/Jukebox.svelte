<script lang="ts">
    import {onDestroy, onMount, tick} from "svelte";
    import JukeboxRenderer from "$lib/jukeboxRenderer";

    let { fov = 75 }: { fov?: number } = $props();

    let jukeboxContainer: HTMLDivElement | undefined = undefined;
    let jukeboxCanvas: HTMLCanvasElement | undefined = undefined;

    let renderer = $state<JukeboxRenderer | undefined>(undefined);
    let isAnimating = $state<boolean>(false);

    function toggleAnimState() {
        if (isAnimating) {
            renderer?.stopAnimation();
        } else {
            renderer?.animate();
        }
        isAnimating = !isAnimating;
    }

    const resizeDebounceMs = 100;
    let resizeTimeout = -1;

    const resizeObserver = new ResizeObserver(([entry]) => {
        renderer?.stopAnimation();
        jukeboxCanvas?.classList.add("blur-resize");
        if (resizeTimeout > 0) clearTimeout(resizeTimeout);
        const { width, height } = entry.contentRect;
        resizeTimeout = setTimeout(() => {
            jukeboxCanvas?.classList.remove("blur-resize");
            renderer?.resize(width, height);
            renderer?.animate();
            resizeTimeout = -1;
        }, resizeDebounceMs);
    });


    onMount(() => {
        jukeboxCanvas!.width = jukeboxContainer!.clientWidth;
        jukeboxCanvas!.height = jukeboxContainer!.clientHeight;

        renderer = new JukeboxRenderer(jukeboxCanvas!, fov);
        renderer.animate();
        isAnimating = true;
        resizeObserver.observe(jukeboxContainer!);

        // Add resize transition to canvas after the initial DOM load
        tick()
            .then(() => {
                jukeboxCanvas!.classList.add("animate-resize");
            });
    });

    onDestroy(() => {
       resizeObserver.disconnect();
    });
</script>

<div bind:this={jukeboxContainer} id="jukebox-container">
    <canvas bind:this={jukeboxCanvas} id="jukebox-canvas"></canvas>
</div>

<style>
#jukebox-container {
    align-self: center;
    justify-self: center;
    width: 90%;
    height: 90%;
}

#jukebox-canvas {
    width: 100%;
    height: 100%;
}
</style>