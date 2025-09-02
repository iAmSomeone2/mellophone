import * as THREE from "three";
import defaultCoverUrl from "../../assets/missing_album_cover.dds";

export class AlbumObject extends THREE.Object3D {
    private static readonly defaultTexture: THREE.Texture = new THREE.Texture();
}

export default class JukeboxRenderer {
    private readonly canvas: HTMLCanvasElement;
    public readonly scene: THREE.Scene = new THREE.Scene();
    private readonly camera: THREE.PerspectiveCamera;
    private renderer: THREE.WebGLRenderer;

    private _isAnimating: boolean = false;

    public get isAnimating(): boolean {
        return this._isAnimating;
    }

    private readonly cube: THREE.Mesh;
    private readonly light: THREE.DirectionalLight;

    public set fov(fov: number) {
        this.camera.fov = fov;
    }

    public get fov(): number {
        return this.camera.fov;
    }

    constructor(canvas: HTMLCanvasElement, fov: number = 75) {
        this.canvas = canvas;
        const width = Math.trunc(this.canvas.width);
        const height = Math.trunc(this.canvas.height);

        this.camera = new THREE.PerspectiveCamera(fov, width / height, 0.1, 1000);
        this.renderer = new THREE.WebGLRenderer({
            canvas,
            alpha: false,
            antialias: true,
        });
        this.renderer.setSize(width, height);
        this.scene.background = new THREE.Color(0x000000);

        // Make box
        const geometry = new THREE.BoxGeometry(1, 1, 1);
        const material = new THREE.MeshPhongMaterial({color: 0x11FFAA, precision: "highp"});
        this.cube = new THREE.Mesh(geometry, material);
        this.scene.add(this.cube);

        // Make light
        this.light = new THREE.DirectionalLight(0xffffff, 3);
        this.light.position.set(-1, 2, 5);
        this.scene.add(this.light);

        this.camera.position.setZ(2);

        // Render once to at least have a starting image
        this.render();
    }

    public resize(width: number, height: number) {
        width = Math.trunc(width);
        height = Math.trunc(height);

        this.canvas.style.width = `${width}px`;
        this.canvas.style.height = `${height}px`;

        const pixelRatio = window.devicePixelRatio ?? 1.0;
        width *= pixelRatio;
        height *= pixelRatio;

        this.renderer.setSize(width, height, false);
        this.canvas.width = width;
        this.canvas.height = height;

        this.camera.aspect = width / height;
        this.camera.updateProjectionMatrix();

        if (!this._isAnimating) this.render(); // Render at least once to resolve the new aspect ratio
        console.debug(
            `[${JukeboxRenderer.name}] Resized renderer to ${this.canvas.width}x${this.canvas.height}`,
        );
    }

    public update(deltaTime: number) {
        this.cube.rotateX(0.001 * deltaTime);
        this.cube.rotateY(0.00125 * deltaTime);
    }

    public render() {
        this.renderer.render(this.scene, this.camera);
    }

    public animate() {
        if (this._isAnimating) return; // Animation already active

        this._isAnimating = true;
        let lastFrameTime = performance.now();
        this.renderer.setAnimationLoop((time) => {
            const deltaTime = time - lastFrameTime;
            lastFrameTime = time;
            this.update(deltaTime);
            this.render();
        });
    }

    public stopAnimation() {
        if (!this._isAnimating) return; // Animation not running so nothing to stop

        this._isAnimating = false;
        this.renderer.setAnimationLoop(null);
    }
}
