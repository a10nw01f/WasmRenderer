import React,{ Component, ChangeEvent } from "react"
import Dropdown from "./Dropdown"
import Entity from "../Types/Entity"
import * as gameService from "../Services/GameService"
import * as fileService from "../Services/FileService"
import KeyValue from "../Types/KeyValue";
import Vector3 from "../Types/Vector3";
import Transform from "../Types/Transform";
import { Proxy, ListenerProxy, StateProxy } from "../Types/Proxy"
import createTransformEditor from './TransformEditor'
import imagePicker from "./ImagePicker";
import Mesh from '../Types/Mesh'
import ObjectEditor from './ObjectEditor'

interface IEntityEditorProps
{
}

interface IEntityEditorState
{
    meshes: KeyValue<string,Mesh>[],
    currentEntityIndex?: number,
    entities: KeyValue<string, Entity>[],
    currentEntityName: string,
}

function createImagePickers<T, K extends keyof T>(proxy: Proxy<T>, onChange:any, ...maps: KeyValue<string, K>[]){
    return maps.map((x:KeyValue<string, K>)=>{
        return     (
            <div className="row" key={x.key}>
                <div style={{float: "left", marginTop: "38px"}}>{x.key}</div>
                <div style={{float:"right", maxWidth:"135px", height:"100px"}}>
                {imagePicker(proxy.prop(x.value) as any, (image)=>{
                    onChange(x, image);
                })}
                </div>
            </div>);
    });
}

function createRange(mesh: any, proxy: any, name:any, props: any){
    return (
    <div className="range-field">
        <div>{name}:{mesh[name]}</div>
        <input type="range" min="0" max="1" step="0.001" value={mesh[name]} onChange={(e)=>{
            proxy.prop(name).set(e.target.valueAsNumber);
            let material = {} as any;
            for(let i of props){
                material[i] = mesh[i];
            }
            material[name] = e.target.valueAsNumber;
            gameService.setMeshMaterial(mesh.id, material);
        }} />
    </div>)
}

function createMeshEditor(meshValueProxy:Proxy<Mesh>){
        let mesh = meshValueProxy.get();

        let imagePickers = createImagePickers(meshValueProxy,(pair:any, image:any)=>{
            gameService.setMeshTexture(mesh.id, pair.value, image);
        }, {key:"albedo", value:"albedoMap"}, 
           {key:"normal", value: "normalMap"}, 
           {key:"roughness", value: "roughnessMap"},
           {key:"metallic", value: "metallicMap"});

        let props = ["roughness", "metallic"];
        return (
            <div>
                {createRange(mesh, meshValueProxy, "roughness", props)}
                {createRange(mesh, meshValueProxy, "metallic", props)}
                {imagePickers}
            </div>)
}

function createMeshSelector(self: any){
    return (
    <div>
        <button style={{marginTop: "10px"}} className="btn waves-effect waves-light" onClick={()=>self.loadMesh()}>Load Mesh</button>
        <Dropdown label="Model"
            options={self.state.meshes} 
            onSelected={(e)=>self.onMeshSelected(e)}
            selectedOption={self.state.entities[self.state.currentEntityIndex].value.currentMesh}></Dropdown>
    </div>
        )
}

function createEntityEditor(entityProxy: Proxy<Entity>){
    let entity = entityProxy.get();
    let transformProxy = new ListenerProxy(entityProxy.prop("transform"), (value: Transform)=>{
        gameService.setEntityTransform(entity.id, value);
    });
    let transformEditor = createTransformEditor(transformProxy);

    let meshEditor = null;
    if(entity.currentMesh){
        meshEditor = createMeshEditor(entityProxy.prop("currentMesh").prop("value"));
    }

    return (
        <div>
            {meshEditor}
            {transformEditor}
        </div>
        )
}

export default class EntityEditor extends Component<IEntityEditorProps, IEntityEditorState>
{
    constructor(props:IEntityEditorProps)
    {
        super(props)

        this.state = {
            meshes: [],
            entities:[],
            currentEntityName: ""};
    }

    onMeshSelected(v: KeyValue<string, Mesh>){
        const index = this.state.currentEntityIndex as number;
        const entity = this.state.entities[index];
        gameService.setEntityMesh(entity.value.id, v.value.id);
        let stateProxy = new StateProxy(this);
        stateProxy.prop("entities").prop(index).prop("value").prop("currentMesh").set(v);
    }

    async loadMesh(){
        let file: File = await fileService.getFile(".obj");
        if(!file){
            return;
        }
        
        let option:KeyValue<string, Mesh> = {
            key: file.name,
            value: new Mesh(gameService.loadMesh(await fileService.readFileText(file)))
        }
        
        this.setState({meshes:[...this.state.meshes, option]});
    }

    render(){
        let stateProxy = new StateProxy(this);
        let editor = null;
        if(this.state.currentEntityIndex !== undefined){
            let entityProxy = stateProxy.prop("entities").prop(this.state.currentEntityIndex).prop("value");
            editor = (<div>
                {createMeshSelector(this)}
                {createEntityEditor(entityProxy)}
            </div>);
        }
        return(
            <ObjectEditor 
                name="Entity" 
                objects={stateProxy.prop("entities")} 
                currentIndex={stateProxy.prop("currentEntityIndex")} 
                createObject={(x)=>{ return new Entity(gameService.createEntity())}} 
                deleteObject={(e: KeyValue<string,Entity>)=>{gameService.deleteEntity(e.value.id);}}>
                    {editor}
                </ObjectEditor>
        )
    }
}