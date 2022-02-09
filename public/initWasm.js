"use strict";

(function(){
    function setEnv(env){
        env.Debugger = function(){
            debugger;
        }

        env.TexImage2DHandle = function(){
            window.gameEnv.texImage2D.apply(null, arguments);
        }

        env.TexStorage2D = function(){
            window.gameEnv.texStorage2D.apply(null, arguments);
        }
    }
    
    const originalInstantiate = WebAssembly.instantiateStreaming;
    WebAssembly.instantiateStreaming = function(buffer, info){
        setEnv(info.env);
        return originalInstantiate(buffer,info);
    }
}());