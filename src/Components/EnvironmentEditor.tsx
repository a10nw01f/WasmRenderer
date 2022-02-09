import React, { Component } from 'react'
import * as gameService from "../Services/GameService"
import * as fileService from '../Services/FileService'

const changeEnvironment = async ()=>{
    let file = await fileService.getFile(".hdr");
    if(!file)
        return;

    const buffer = await fileService.readAsArrayBuffer(file) as any;
    const arr = new Uint8Array(buffer);
    gameService.setEnvironment(arr);
};

interface IEnvironmentEditorState
{
    exposure: number;
}

export default class EnvironmentEditor extends Component<{}, IEnvironmentEditorState>
{
    constructor(props:any){
        super(props);
        this.state = {exposure: 1};
    }

    render(){
        return (
            <div>
                <button className="btn waves-effect waves-light" onClick={changeEnvironment}>Set Environment</button>
                <div className="range-field row" style={{marginTop: 10}}>
                    <div className="col s4">exposure:{this.state.exposure}</div>
                    <input className="col s4 offset-s4" type="range" min="0" max="5" step="0.01" style={{padding:0, marginTop: 10}} 
                        value={this.state.exposure} 
                        onChange={(e)=>{
                            gameService.setEnvironmentSettings({exposure: e.target.valueAsNumber});
                            this.setState({exposure: e.target.valueAsNumber})
                    }} />
                </div>
            </div>
        )
    }
}
