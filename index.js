const addon = require('bindings')('farsight');

var offsetsSet = false;
var championsSet = false;
var connected = false;

var autoImportChampions = true;
var autoImportOffsets = true;
var gameVersion = [0,0];
var offsetRepository = "https://api.github.com/repos/floh22/native-farsight-module/contents/offsets?ref=main";



async function importChampionsFromCDragon() {
    if(championsSet) {
        console.error("Champions already set! Cannot import from community dragon.");
        return;
    }
    console.log("Getting champions from community dragon...");
    var champions = [];
    var response = await fetch('https://raw.communitydragon.org/latest/plugins/rcp-be-lol-game-data/global/default/v1/champion-summary.json');
    var data = await response.json();
    for (var i = 0; i < data.length; i++) {
        champions.push(data[i].alias.toLowerCase());
    }
    console.log("Champions received. Importing...");

    var added = setChampions(champions);

    if(!added) {
        console.error("Champions could not be imported into native module! Farsight may not work properly.");
    }

}


async function getLatestPatchFileUrl() {
    var response = await fetch(offsetRepository);
    var data = await response.json();
    
    var latestPatchFile = null;
    var latestPatchVersion = -1;
    for(var i = 0; i < data.length; i++) {
        var file = data[i];
        var offsetVersion = file.name.replace(/[^\d.-]/g, '');
        if(offsetVersion.startsWith(gameVersion[0] + "." + gameVersion[1])) {
            var patchVersion = offsetVersion.split(".").slice(-1)[0];
            if(latestPatchFile === null || patchVersion > latestPatchVersion) {
                latestPatchFile = file;
                latestPatchVersion = patchVersion;
            }
        }
    }

    if(latestPatchFile === null) {
        return null;
    }

    return latestPatchFile.download_url;
}


async function importOffsets() {
    console.log("Getting offsets from repository...");

    if(gameVersion[0] === 0 && gameVersion[1] === 0) {
        var response = await fetch('https://raw.communitydragon.org/latest/content-metadata.json');

        if(!response.ok) {
            console.error("Could not fetch game version. Status: " + response.status);
            return;
        }

        var data = await response.json();
        gameVersion = data.version.split(".").slice(0, 2);
    }

    console.log("Game version: " + gameVersion[0] + "." + gameVersion[1]);

    var url = await getLatestPatchFileUrl();

    if(url === null) {
        console.error("Could not find offsets for game version " + gameVersion[0] + "." + gameVersion[1]);
        return;
    }

    var response = await fetch(url);

    if(!response.ok) {
        console.error("Could not fetch offsets from repository. Status: " + response.status);
        return;
    }

    var data = await response.json();


    data.keys().forEach(key => {
        data[key] = parseInt(data[key]);
    });

    
    console.log(data);

    


    console.log("Offsets received. Importing...");

    var res = setOffsets({
        gameTime: 0x316FDE4,
        objIndex: 0x8,
        objTeam: 0x34,
        objNetworkID: 0xB4,
        objPosition: 0x1DC,
        objHealth: 0xE7C,
        objMaxHealth: 0xE8C,
        objMana: 0x029C,
        objMaxMana: 0x2AC,
        objName: 0x2DB4,
        objLvl: 0x35A4,
        objExperience: 0x3594,
        objCurrentGold: 0x1BB4,
        objTotalGold: 0x1BC4,
        objDisplayName: 0x54,
        objDisplayNameLength: 0x64,
    
    
        objectManager: 0x18D9ACC,
        objectMapCount: 0x2C,
        objectMapRoot: 0x28,
        objectMapNodeNetId: 0x10,
        objectMapNodeObject: 0x14,
    
        heroList: 0x18D9B6C,
        minionList: 0x252729C,
        turretList: 0X316EAC4,
        inhibitorList: 0x0
    });

    if(!res) {
        console.error("Offsets could not be imported into native module! Farsight will not work properly.");
    }
}


function convertObjectDisplayNames(snapshot) {
    var textDecoder = new TextDecoder();
    for (var i = 0; i < snapshot.champions.length; i++) {
        var champion = snapshot.champions[i];
        champion.displayName = textDecoder.decode(champion.displayName);
    }

    for(var i = 0; i < snapshot.other.length; i++) {
        var other = snapshot.other[i];
        other.displayName = textDecoder.decode(other.displayName);
    }
    
}


function makeSnapshot() {
    if (!offsetsSet) {
        console.error("Offsets not set!. Cannot create snapshot.");
        return {
            gameTime: 0
        }
    }

    if(!championsSet) {
        console.error("Champions not set!. Cannot create snapshot.");
        return {
            gameTime: 0
        }
    }

    if(!connected) {
        console.error("Not connected to league!. Cannot create snapshot.");
        return {
            gameTime: 0
        }
    }

    var snapshot = addon.makeSnapshot();
    convertObjectDisplayNames(snapshot)

    return snapshot;
}

async function connectToLeague () {
    if(connected) {
        console.error("Already connected to league!");
        return false;
    }

    console.log("Connecting to league...");

    var res = addon.connectToLeague();

    if(res.success) {
        connected = true;
        console.log("Connected!");

        if(autoImportChampions && !championsSet) {
            await importChampionsFromCDragon();
        }

        if(autoImportOffsets && !offsetsSet) {
            await importOffsets();
        }

        return true;
    }
    console.log("Failed to connect to league...");
    console.error(res.error);
    return false;
}

function disconnectFromLeague() {
    if(!connected) {
        console.error("Not connected to league!");
        return false;
    }
    var res = addon.disconnectFromLeague();
    if(res.success) {
        connected = false;
        return true;
    }
    
    console.error(res.error);
    return false;
}


function setOffsets(offsetObject) {
    if (offsetsSet) {
        console.error("Offsets already set!");
    }

    var res = addon.setOffsets(offsetObject);

    if(res.success) {
        offsetsSet = true;
        return true;
    }

    console.error("Offsets could not be set! Farsight will not work properly.");
    return false;
}


function setChampions(championList) {
    if (championsSet) {
        console.error("Champions already set!");
    }

    var res = addon.setChampionNames(championList);

    if(res === championList.length) {
        championsSet = true;
        return true;
    }

    console.error(res.error);
    return false;
}



function isReady() {
    return connected && offsetsSet && championsSet;
}

exports.connectToLeague = connectToLeague;
exports.disconnectFromLeague = disconnectFromLeague;
exports.makeSnapshot = makeSnapshot;
exports.setOffsets = setOffsets;
exports.isReady = isReady;
exports.setChampionNames = addon.setChampionNames;
exports.autoImportChampions = autoImportChampions;
exports.autoImportOffsets = autoImportOffsets;


// ---- TEMPOARY CODE ----


async function init() {


    var res = await connectToLeague();

    if(isReady) {
        console.log("Farsight is ready!");
    }


    var timeout = setInterval(() => {
        if(!res.success) {
            res = addon.connectToLeague();
        }
        if (res.success && isReady()) {
            console.log(makeSnapshot());
            
        }
    }
        , 1000);
}


const boundInit = init.bind(this);

boundInit();