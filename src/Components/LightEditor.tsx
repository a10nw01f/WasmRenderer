import React, {Component} from 'react'
import {PointLight, DirectionalLight} from '../Types/Light'
import * as gameService from '../Services/GameService'
import KeyValue from '../Types/KeyValue';
import ObjectEditor from './ObjectEditor';
import { Proxy, StateProxy, ListenerProxy } from '../Types/Proxy';
import { createVectorEditor } from './TransformEditor';
import Vector3 from '../Types/Vector3';
import { SketchPicker } from 'react-color';

interface ILightEditorState
{
    pointLights: KeyValue<string, PointLight>[],
    currentPointLight?: number,
    directionalLights: KeyValue<string, DirectionalLight>[],
    currentDirectionalLight?: number
}

function createEditorIfSelected<T>(current: number | undefined, arr: Proxy<KeyValue<string, T>[]>, create: (value:Proxy<T>)=>any)
{
    let result = null as any;
    if(current !== undefined){
        return create(arr.prop(current as number).prop("value"));
    }

    return result;
}

function vector3ToColor(v: Vector3){
    return {
        r: v.x * 255, 
        g: v.y * 255, 
        b: v.z *  255
    }
}

function colorToVector3(v: any){
    return Vector3.div(new Vector3(v.rgb.r, v.rgb.g, v.rgb.b), 255);
}

export default class LightEditor extends Component<{}, ILightEditorState>
{
    constructor(props: any)
    {
        super(props);

        this.state = {
            pointLights:[],
            directionalLights:[]
        }
    }

    createPointLightEditor(light: Proxy<PointLight>){
        return (
            <div>
                <div className="row">
                    <div>Position</div>
                    {createVectorEditor(light.prop("position"))}
                </div>
                <div className="row">
                    <SketchPicker
                        color={ vector3ToColor(light.prop("color").get()) }
                        onChangeComplete={ (color: any)=>{
                            light.prop("color").set(colorToVector3(color));
                        }}
                    />
                </div>
                <div className="input-field col s">
                    <input type="number" className="intensity" min="0" max="100000" step="0.1" value={light.prop("intensity").get()} onChange={(e)=>{
                        light.prop("intensity").set(e.target.valueAsNumber);
                    }}></input>
                    <label htmlFor="intensity">Intensity</label>
                </div>
            </div>
            )
    }

    createDirectionalLightEditor(light: Proxy<DirectionalLight>){
        return (
            <div>
                <div className="row">
                    <div>Direction</div>
                    {createVectorEditor(light.prop("direction"))}
                </div>
                <div className="row">
                    <SketchPicker
                        color={ vector3ToColor(light.prop("color").get()) }
                        onChangeComplete={ (color: any)=>{
                            light.prop("color").set(colorToVector3(color));
                        }}
                    />
                </div>
            </div>
        )
    }

    render(){
        let stateProxy = new StateProxy(this);
        let pointLightEditor = createEditorIfSelected(this.state.currentPointLight, stateProxy.prop("pointLights"), (lightProxy)=>{
            return this.createPointLightEditor(new ListenerProxy(lightProxy, (value: PointLight)=>{
                gameService.setPointLight(value.id, value);
            }));
        });

        let directinalLightEditor = createEditorIfSelected(this.state.currentDirectionalLight, stateProxy.prop("directionalLights"), (proxy)=>{
            return this.createDirectionalLightEditor(new ListenerProxy(proxy, (value: DirectionalLight)=>{
                gameService.setDirectionalLight(value.id, value);
            }));
        });

        return (
            <div>
                <ObjectEditor
                    name="Point Light"
                    createObject={()=>{
                        const result =  new PointLight(gameService.createPointLight());
                        gameService.setPointLight(result.id, result);
                        return result;
                    }}
                    deleteObject={()=>{ 
                        gameService.deletePointLight(this.state.pointLights[this.state.currentPointLight as number].value.id)
                    }}
                    objects={stateProxy.prop("pointLights")}
                    currentIndex={stateProxy.prop("currentPointLight")}>
                        {pointLightEditor}
                    </ObjectEditor>
                    <ObjectEditor
                    name="Directional Light"
                    createObject={()=>{
                        const result =  new DirectionalLight(gameService.createDirectionalLight());
                        gameService.setDirectionalLight(result.id, result);
                        return result;
                    }}
                    deleteObject={()=>{ 
                        gameService.deleteDirectionalLight(this.state.directionalLights[this.state.currentDirectionalLight as number].value.id)
                    }}
                    objects={stateProxy.prop("directionalLights")}
                    currentIndex={stateProxy.prop("currentDirectionalLight")}>
                        {directinalLightEditor}
                    </ObjectEditor>
            </div>
        )
    }
}