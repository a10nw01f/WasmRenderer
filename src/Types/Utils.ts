export function getKeys<T>(type: { new(): T ;}){
    return Object.keys(new type()) as (keyof T)[];
}