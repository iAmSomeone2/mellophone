import WebGL from "three/addons/capabilities/WebGL.js";
import WebGPU from "three/addons/capabilities/WebGPU.js";

export enum GraphicsAPI {
  WebGL,
  /** WebGL 2: Standard web API for hardware-accelerated rendering based on OpenGL ES 3.0 */
  WebGL2,
  /** WebGPU: Next-gen modern hardware-accelerated rendering and compute API */
  WebGPU,
}


/**
 * Features detected in the client environment
 */
export interface ClientFeatures {
  /**
   * Best available graphics API provided by the client
   */
  supportedGraphicsAPIs: GraphicsAPI[];
}

export function getSupportedGraphicsAPIs(): GraphicsAPI[] {
  const apis = [];
  if (WebGPU.isAvailable()) apis.push(GraphicsAPI.WebGPU);
  if (WebGL.isWebGL2Available()) apis.push(GraphicsAPI.WebGL2);

  // Manually determine WebGL 1 support
  const canvas = document?.createElement("canvas");
  canvas.width = 1;
  canvas.height = 1;
  const supportsWebGL = canvas.getContext("webgl") !== null;
  canvas.remove();
  if (supportsWebGL) apis.push(GraphicsAPI.WebGL);

  return apis;
}
