import Vector3 from "./Vector3"

export class PointLight
{
    constructor(public id:number){}

    public position: Vector3 = new Vector3();
    public color: Vector3 = new Vector3(1,1,1);
    public intensity: number = 100;
}

export class DirectionalLight
{
    constructor(public id:number){}

    public direction: Vector3 = new Vector3(0,1,0);
    public color: Vector3 = new Vector3(1,1,1);
}
