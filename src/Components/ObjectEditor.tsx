import React, {Component} from 'react'
import Dropdown from './Dropdown';
import KeyValue from '../Types/KeyValue';
import {Proxy} from '../Types/Proxy'

interface IObjectEditorState<T>{
    currentName: string,
}

interface IObjectEditorProps<T>{
    createObject:(name: string)=>T,
    name: string,
    deleteObject:(obj: KeyValue<string, T>)=>void,
    objects: Proxy<KeyValue<string, T>[]>,
    currentIndex: Proxy<number | undefined>
}

export default class ObjectEditor<T> extends Component<IObjectEditorProps<T>, IObjectEditorState<T>>{
    constructor(props:any){
        super(props);

        this.state = {
            currentName: ""
        }
    }

    createObject(){
        let objects = [...this.props.objects.get()];
        objects.push({
            key: this.state.currentName,
            value: this.props.createObject(this.state.currentName)
        });

        this.props.objects.set(objects);
    }

    deleteObject(){
        let objects = [...this.props.objects.get()];
        const index = this.props.currentIndex.get() as number;

        this.props.deleteObject(objects[index]);

        objects.splice(index, 1);

        this.props.objects.set(objects);
        this.props.currentIndex.set(undefined);
    }

    onSelected(x: any, index: number){
        this.props.currentIndex.set(index);
    }

    render(){
        const index = this.props.currentIndex.get();
        const isSelected = index !== undefined;
        const objects = this.props.objects.get();
        return (
            <div>
                <div className="input-field">
                    <input type="text" className="createObject" 
                        onChange={(e)=>this.setState({currentName: e.target.value})} 
                        value={this.state.currentName}/>
                    <label htmlFor="createObject">{this.props.name} Name</label>
                </div>
                <button className="btn waves-effect waves-light" onClick={()=>this.createObject()}>Create {this.props.name}</button>
                <br />
                <Dropdown 
                    label={this.props.name}
                    options={objects} 
                    onSelected={(e, i)=>this.onSelected(e, i)}
                    selectedOption={isSelected ? objects[index as number] : undefined}></Dropdown>
                <div style={{display: (isSelected ? "initial" : "none")}}>
                    {this.props.children}
                    <button className="btn waves-effect waves-light" onClick={()=>this.deleteObject()}>Delete {this.props.name}</button>
                </div>
            </div>
        )
    }
}