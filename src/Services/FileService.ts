
let fileInput = document.createElement("input");
fileInput.type = "file";
fileInput.style.display = "none";

document.body.appendChild(fileInput);

let lastResolve : any;

document.body.onfocus  = ()=>{
    setTimeout(()=>{
        if(lastResolve){
            lastResolve();
            lastResolve = null;
        }
    },5000)
}

export function getFile(accept:string = ""){
    return new Promise<File>(resolve=>{
        lastResolve = resolve;
        fileInput.value = "";

        fileInput.accept = accept;
        fileInput.onchange = (e:any)=>{
            resolve(e.target.files[0]);
            lastResolve = null;
            fileInput.value = "";
        };
    
        fileInput.click();
    });
}

const readFile = (file: File, method: keyof FileReader)=>{
    return new Promise<string>((resolve)=>{
        const reader: any = new FileReader();
        reader.onload = (e:any)=>{
            resolve(e.target.result);
        };
    
        reader[method](file);
    })
}

export function readFileText(file: File){
    return readFile(file, "readAsText");
}

export function readAsDataURL(file: File){
    return readFile(file, "readAsDataURL");
}

export function readAsArrayBuffer(file: File){
    return readFile(file, "readAsArrayBuffer");
}

