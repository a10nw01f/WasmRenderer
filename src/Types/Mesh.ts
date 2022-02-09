export default class Mesh{
    constructor(public id: number)
    {
    }

    public albedoMap:string = "";
    public normalMap:string = "";
    public roughnessMap:string = "";
    public metallicMap:string = "";
    public roughness: number = 0;
    public metallic: number = 0;
}