import { Component } from "react";

type RemoveOptionl<T> = {
    [K in keyof T]-? : T[K]
}

export interface Proxy<T>
{
    get(): T;
    set(value: T): void;
    prop<K extends keyof T>(k : K): Proxy<RemoveOptionl<T>[K]>;   
    setPath(value:any, ...keys: any[]): void;
}

export class StateProxy<T>
{
    constructor(c: Component<any, T>){
        this.component = c;
    }

    public prop<K extends keyof T>(key: K)
    {
        return new StateProp<RemoveOptionl<T>[K]>(this.component, key);
    }

    private component: any;
}

export class StateProp<T>
{
    constructor(private component: any, private key : any)
    {
    }

    public setPath(value: any, ...keys : any[])
    {
        let prop = this.get() as any;
        if(prop.constructor === Array){
            prop = [...prop];
        }
        else {
            prop = {...prop};
        }

        let last = keys.pop();
        let next = prop as any;
        for(let k of keys){
            if(next[k].constructor === Array){
                next[k] = [...next[k]];
            }
            else {
                next[k] = {...next[k]};
            }
            
            next = next[k];
        }

        next[last] = value;

        this.set(prop);
    }

    public set(value: T)
    {
        this.component.setState({
            [this.key] : value
        });
    }

    public get():T
    {
        return this.component.state[this.key];
    }

    public prop<K extends keyof T>(key: K)
    {
        return new ProxyProp<RemoveOptionl<T>[K]>(this, key);
    }
}

export class ProxyProp<T>
{
    constructor(private parent: any, private key: any)
    {
    }

    public get() : T
    {
        return this.parent.get()[this.key];
    }

    public set(value: T)
    {
        this.parent.setPath(value, this.key);
    }

    public prop<K extends keyof T>(key : K)
    {
        return new ProxyProp<RemoveOptionl<T>[K]>(this, key);
    }

    public setPath(value: any, ...keys: any[])
    {
        this.parent.setPath(value, this.key, ...keys);
    }
}

export class ListenerProxy<T> 
{
    constructor(private proxy:Proxy<T>, private onChange:(value: T)=>void)
    {
    }

    public get() : T
    {
        return this.proxy.get();
    }

    public set(value: T)
    {
        this.proxy.setPath(value);
        this.onChange(this.get());
    }

    public prop<K extends keyof T>(key : K)
    {
        return new ProxyProp<T[K]>(this, key);
    }

    public setPath(value: any, ...keys: any[])
    {
        this.proxy.setPath(value, ...keys);
        setTimeout(()=>{
            this.onChange(this.get());
        },1);
    }
}