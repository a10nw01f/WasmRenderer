export default class Vector3
{
    constructor(public x:number = 0,public y:number = 0,public z:number = 0)
    {
    }

    static mult(self: Vector3, v: number){
        return new Vector3(self.x * v, self.y * v, self.z * v)
    }

    static div(self: Vector3, v: number){
        return this.mult(self, 1/v);
    }
}
