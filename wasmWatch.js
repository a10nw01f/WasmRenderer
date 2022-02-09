let fs = require("fs")
const exec = require('child_process').exec;

function waitAync(ms){
    return new Promise(resolve=>{
        setTimeout(resolve, ms);
    });
}

let changeCount = 0;
let cp = null;

fs.watch("./wasm", {recursive:true}, async (event, filename)=>{
    changeCount++;
    await waitAync(100);
    changeCount--;
    if(!changeCount)
    {
        if(cp)
            cp.kill();
        console.log("make");
        try{
            cp = exec("make", (error, stdout, stderr)=>{
                console.log(stdout);
                console.log(stderr);
                console.log("done");
                cp = null;
            });
        }
        catch(e)
        {}
    }
})
