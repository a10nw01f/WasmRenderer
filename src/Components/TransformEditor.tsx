import React from 'react'
import { Proxy } from '../Types/Proxy'
import { getKeys } from '../Types/Utils'
import Transform from '../Types/Transform';
import Vector3 from '../Types/Vector3';

export function createVectorEditor(vec: Proxy<Vector3>){
    return getKeys(Vector3).map((k:keyof Vector3)=>{
        return (
            <div className="input-field col s4" key={k}>
                <input 
                    type="number" 
                    step={0.1}
                    className={k} 
                    value={vec.prop(k).get()} 
                    onChange={(e)=>{ vec.prop(k).set(e.target.valueAsNumber || 0) }}/>
                <label htmlFor={k}>{k}</label>
            </div>
            )
    })
}

export default function createTransformEditor(value: Proxy<Transform>){
    return getKeys(Transform).map((k: keyof Transform)=>{
        return (
            <div className="row" key={k}>
                <div>{k}</div>
                {createVectorEditor(value.prop(k))}
            </div>
            )
    });
}