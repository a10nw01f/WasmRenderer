import Transform from "../Types/Transform";
import Entity from "../Types/Entity";
import Vector3 from "../Types/Vector3";
import { catchClause } from "@babel/types";
import { wrapWasmFunc, WasmArgWrapper } from "../Types/WrapFunc";
import { number } from "prop-types";
import { PointLight, DirectionalLight } from "../Types/Light";

const wnd = window as any;
const Module = wnd.Module;

const waitForLoad = ()=>{
    return new Promise(resolve=>{
        if(Module.calledRun){
            resolve();
        }
        else {
            Module.addOnPostRun(resolve);
        }
    })
};

let lastWidth = 0;
let lastHeight = 0;

function resizeCanvasToDisplaySize(canvas:HTMLCanvasElement) {
    const width = canvas.clientWidth;
    const height = canvas.clientHeight;
 
    if (canvas.width !== width || canvas.height !== height) {
      canvas.width = width;
      canvas.height = height;
    }

    if(canvas.width !== lastWidth || canvas.height !== lastHeight)
    {
        lastWidth = canvas.width
        lastHeight = canvas.height
        wnd.Module.ctx.viewport(0, 0, lastWidth, lastHeight)
        wnd._OnResize(lastWidth, lastHeight);
    }
 
 }

const setFrameLoopFunction = (loopFunc:()=>void)=>{
    const func = function(this: FrameRequestCallback){
      loopFunc();
      window.requestAnimationFrame(func);
    };
    window.requestAnimationFrame(func);
  }

export async function init(canvas : HTMLCanvasElement){
        await waitForLoad();

        Module.createContext(canvas, true, true, {
            antialias: true,
            alpha: true,
            majorVersion: 2,
            minorVersion: 0
        });

        Module.keyboardListeningElement = canvas;

        wnd._Init();
        wnd._OnResize(canvas.width, canvas.height);

        setFrameLoopFunction(()=>{
            wnd._Update();
            resizeCanvasToDisplaySize(canvas);
        });
}

wnd.gameEnv = {
    texImage2D:(target:number, level: number, internalFormat: number, format: number, type: number, handle: number)=>{
        let gl = wnd.Module.ctx as WebGLRenderingContext;
        gl.texImage2D(target, level, internalFormat, format, type, wnd.Emval.toValue(handle));
    },
    texStorage2D:(target:number, levels: number, internalFormat: number, width: number, height: number)=>{
        let gl = wnd.Module.ctx as any;
        gl.texStorage2D(target, levels, internalFormat, width, height);
    }
}

export function setEnvironment(arr:Uint8Array){
    const ptr = wnd._malloc(arr.length);
    wnd.writeArrayToMemory(arr, ptr);
    wnd._SetEnvironment(ptr, arr.length);
    wnd._free(ptr);
}

const numberWrapper = {
    before(x:number){return x},
    after(x:number){}
}

const handleWrapper = {
    before(x:any){ return wnd.Emval.toHandle(x) },
    after(x: number){ wnd.__emval_decref(x) }
}

const largeStringWrapper = {
    before(x:string) { 
        const len = (x.length << 2) + 1;
        const ptr  = wnd._malloc(len);
        wnd.stringToUTF8(x, ptr, len);
        return ptr;
     },
     after(ptr: number){
        wnd._free(ptr);
     }
}

const u8ArrayWrapper = {
    before(arr:Uint8Array){
        const ptr = wnd._malloc(arr.length);
        wnd.writeArrayToMemory(arr, ptr);
        return ptr;
    },
    after(ptr: number){
        wnd._free(ptr);
     }
}

const onVoid = (x?: number): void => undefined;
const onNumber = (x?:number) : number =>  x ? x : 0;

const upperCaseFirstLetter = (str:string)=>{
    return str[0].toUpperCase() + str.slice(1);
}

export const setEnvironmentSettings = wrapWasmFunc("SetEnvironmentSettings", onVoid, handleWrapper);
export const setMeshTexture = (mesh:number, name:string, image:HTMLImageElement)=>{
    let funcName = "SetMesh" + upperCaseFirstLetter(name);
    let func = wrapWasmFunc(funcName, onVoid, numberWrapper, handleWrapper as WasmArgWrapper<HTMLImageElement>);
    func(mesh,image);
}
export const setMeshMaterial = wrapWasmFunc("SetMeshMaterial", onVoid, numberWrapper, handleWrapper);
export const createEntity = wnd._CreateEntity as ()=>number;
export const deleteEntity = wnd._DeleteEntity as (e:number)=>void;
export const setEntityMesh = wnd._SetEntityMesh as (entity:number, mesh:number) => void;
export const loadMesh = wrapWasmFunc("AddMesh", onNumber, largeStringWrapper);
export const setEntityTransform = wrapWasmFunc("SetEntityTransform", onVoid, numberWrapper, handleWrapper as WasmArgWrapper<Transform>)
export const createPointLight = wnd._CreatePointLight as ()=>number;
export const setPointLight = wrapWasmFunc("SetPointLight", onVoid, numberWrapper, handleWrapper as WasmArgWrapper<PointLight>)
export const deletePointLight = wnd._DeletePointLight as (e:number)=>void;
export const createDirectionalLight = wnd._CreateDirectionalLight as ()=>number;
export const setDirectionalLight = wrapWasmFunc("SetDirectionalLight", onVoid, numberWrapper, handleWrapper as WasmArgWrapper<DirectionalLight>)
export const deleteDirectionalLight = wnd._DeleteDirectionalLight as (e:number)=>void;
