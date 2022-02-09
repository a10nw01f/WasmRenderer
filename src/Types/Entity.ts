import Vector3 from "./Vector3"
import Transform from "./Transform"
import KeyValue from "./KeyValue"
import Mesh from './Mesh'

export default class Entity
{
    constructor(id: number){
        this.id = id;
    }
    
    id: number;
    currentMesh?: KeyValue<string, Mesh>;
    transform: Transform = new Transform();
    image: string = "";
}