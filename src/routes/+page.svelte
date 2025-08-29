<script lang="ts">
  import { invoke } from "@tauri-apps/api/core";
  import Jukebox from "$lib/components/Jukebox.svelte";

  let name = $state("");
  let greetMsg = $state("");

  async function greet(event: Event) {
    event.preventDefault();
    // Learn more about Tauri commands at https://tauri.app/develop/calling-rust/
    greetMsg = await invoke("greet", { name });
  }
</script>

<main class="container">
  <Jukebox/>
</main>

<style>
  main {
    height: 100%;
    padding: 0;
  }
</style>
