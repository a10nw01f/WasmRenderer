import React, { Component } from "react"
import KeyValue from "../Types/KeyValue"

interface IDropdownProps<T>
{
    label: string;
    options: KeyValue<string, T>[];
    onSelected?: (v:KeyValue<string, T>, index: number)=>void;
    selectedOption?:  KeyValue<string, T>;
}

interface IDropdownState<T>
{
    isOpen: boolean;
}

export default class Dropdown<T> extends Component<IDropdownProps<T>, IDropdownState<T>>
{
    constructor(props: IDropdownProps<T>){
        super(props);
        this.state = { isOpen: false };
    }

    toggleDropdown(){
        this.setState({isOpen: !this.state.isOpen});
    }

    onOptionSelected(option: KeyValue<string, T>, index: number){
        this.setState({isOpen: false});
        const onSelect = this.props.onSelected;
        if(onSelect){
            onSelect(option, index);
        }
    }

    render(){
        const optionsDisplay = this.state.isOpen ? "" : "none";
        const style:any = {display:"block", opacity: 1};
        const selectedOption = this.props.selectedOption;
        const items = this.props.options.map((v,i)=>{
            return <li key={v.key} onClick={()=>this.onOptionSelected(v,i)}><span>{v.key}</span></li>
        })

        return (<div className="select-wrapper">
            <div style={{display: optionsDisplay, position: "absolute", marginTop: "70px", marginLeft: "5px" }}>
                <ul className="dropdown-content select-dropdown" style={style}>
                    {items}
                </ul>
            </div>
            <label>{this.props.label}</label>
            <input onClick={()=>this.toggleDropdown()} 
                    className="select-dropdown dropdown-trigger" 
                    type="text" 
                    readOnly={true}
                    value={selectedOption ? selectedOption.key : ""}></input>
        </div>)
    }
}

