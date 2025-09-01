type Asset = string | Uint8Array;

export default class AssetManager {
    private static _instance: AssetManager | null = null;

    public static get instance() {
        AssetManager._instance ??= new AssetManager();
        return this._instance!;
    }

    /**
     * Hash Map containing any text or binary assets which have been loaded by the front end
     * @private
     */
    private readonly assetMap: Map<string, Asset> = new Map();

    // public async loadBinaryAsset(url: URL): Promise<Uint8Array> {
    //
    // }
}