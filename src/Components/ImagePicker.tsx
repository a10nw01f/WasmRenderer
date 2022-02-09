import React from 'react'
import { Proxy } from '../Types/Proxy'
import * as fileService from '../Services/FileService';

export default function imagePicker(src: Proxy<string>, onChange?: (x:HTMLImageElement)=>void){
    let imageSrc = src.get() || "assets/images/empty-image.png";

    return (
        <img 
            src={imageSrc} 
            style={{maxWidth:"inherit", maxHeight: "inherit", width:"inherit", height: "inherit"}} 
            onClick={async (event)=> {
                let target = event.target as HTMLImageElement;
                let file = await fileService.getFile("image/*");
                if(!file)
                    return;
                let result = await fileService.readAsDataURL(file);
                src.set(result);
                target.onload = ()=>{
                    if(onChange){
                        onChange(target as HTMLImageElement);
                    }
                    target.onload = null;
                }
        }}></img>
    )
}