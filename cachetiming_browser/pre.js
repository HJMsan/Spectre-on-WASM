var cache_size = 12 * 1024 * 1024;
var evictionBuffer = new ArrayBuffer(cache_size);


function clflush(){
    var evictionView = new DataView(evictionBuffer);
    for(let i = 0;i < cache_size/64;i++){
        evictionView.setUint32(i*64, (i*61)%255);
    }
}

function triggerGC(){
    var arr = []
    for (let i = 0;i < 50;i++){
        arr[i] = new ArrayBuffer(1024*1024);
    }
}
