

const wnd = window as any;

export interface WasmArgWrapper<T>
{
    before(arg: T) : number;
    after(arg: number) : void;
}

type Unwrap<T> = {
    [K in keyof T]: T[K] extends { before: (arg: infer U)=> number } ? U : never;
}

export function wrapWasmFunc<U, T extends Array<WasmArgWrapper<any>>>(name: string, onResult: (x?:number)=>U, ...wrappers: T) : (...args : Unwrap<T>)=> U;

export function wrapWasmFunc(name: string, onResult: (x?:number)=>any, ...wrappers:WasmArgWrapper<any>[]) : any{
    let func = wnd.Module["_" + name];
    return (...args: any[])=>{
        for(let i = 0; i < wrappers.length; i++){
            args[i] = wrappers[i].before(args[i]);
        }

        let result = onResult(func(...args));
        
        for(let i = 0; i < wrappers.length; i++){
            args[i] = wrappers[i].after(args[i]);
        }

        return result;
    }
}