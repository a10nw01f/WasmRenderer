import Vector3 from "./Vector3";

export default class Transform{
    position : Vector3 = new Vector3();
    scale : Vector3 = new Vector3(1,1,1);
    rotation : Vector3 = new Vector3();
}