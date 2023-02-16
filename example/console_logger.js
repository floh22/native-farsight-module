var farsight = require('./../index.js');


async function init() {


    var res = await farsight.connectToLeague();

    if(res && farsight.isReady()) {
        console.log("Farsight is ready!");
    }


    var timeout = setInterval(() => {
        if(!farsight.isReady() || !res) {
            res = farsight.connectToLeague();
        }
        if (res && farsight.isReady()) {
            console.log(farsight.makeSnapshot());
            
        }
    }
        , 1000);
}

const boundInit = init.bind(this);

boundInit();