import React, {Component, ChangeEvent} from "react"
import * as gameService from "../Services/GameService"
import * as fileService from '../Services/FileService'
import Dropdown from "./Dropdown"
import EntityEditor from "./EntityEditor";
import KeyValue from "../Types/KeyValue";
import EnvironmentEditor from './EnvironmentEditor';
import {Proxy, StateProxy} from '../Types/Proxy'
import LightEditor from "./LightEditor";

interface IMenuProps{}

interface IMenuState
{
    currentEntity?: KeyValue<string, number>,
    entities: KeyValue<string, any>[],
    currentEntityName: string,
    selectedTabLabel: string,
}

function createTabs(tabs: any[], selectedTab: Proxy<string>){
    let activeTabLabel = selectedTab.get();

    let tabList = tabs.map((p)=>{
        return (
        <li key={p.label} className="tab" onClick={()=>{ selectedTab.set(p.label); }}>
            <a href="#" className={activeTabLabel == p.label ? "active" : ""}>
                {p.label}
            </a>
        </li>)
    });

    let tabsHeader = (
        <div className="row">
            <div className="col s12">
                <ul className="tabs">
                    {tabList}
                </ul>
            </div>
        </div>
    )

    let tabContent = tabs.map(x=> (<div key={x.label} style = {{display : (x.label == activeTabLabel ? "initial" : "none")}}>
        {x.content}
    </div>));

    return (
        <div>
            {tabsHeader}
            {tabContent}
        </div>
        )
}

class Menu extends Component<IMenuProps, IMenuState>
{
    private test: any = {};

    constructor(props:IMenuProps){
        super(props);
        this.state = {
            entities:[],
            currentEntityName: "",
            selectedTabLabel: "Entity",
        };

        this.test.entityEditor = (<div>
            <EntityEditor ></EntityEditor>
        </div>)
    }

    render(){
        const entity = this.state.currentEntity;

        let tabs = [{label: "Entity", content: this.test.entityEditor}, {
            label:"Light",
            content: (<div>
                <EnvironmentEditor></EnvironmentEditor>
                <LightEditor></LightEditor>
                </div>)
        }]

        let stateProxy = new StateProxy(this);

        return (
            <div className="menu-background">
                {createTabs(tabs, stateProxy.prop("selectedTabLabel"))}
            </div>
        )
    }
}

export default Menu;