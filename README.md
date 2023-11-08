<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![Donate][donate-paypal]](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=ZSY2FXPUHXVUJ)

<h1 align="center">🔭 Farsight 🔭</h1>

>A simple passive memory reader which aims to retrieve information about League of Legends spectator games which the API does not provide.
This a nodejs native module which utilizes native memory reading with a js interface.

Farsight uses Memory Reading to get information that the Riot API does not expose. Riot's policy in the past has been to allow passive memory reading, which is exactly what this program does, but this may change at any time.
Farsight currently provides little use during live games and is tailored to be used during spectate games. During live games, objects the player does not see will not be read properly, leading to incorrect or outdated data
---

<h2 align="center">Features </h2>


<p align="left">Each memory snapshot produces game state information the moment the snapshot was taken. Each snapshot includes the following:<p>

1. Game time
2. Next dragon type to spawn
3. List of Champions
4. List of Turrets
5. List of inhibitors
6. Misc game objects


<p align="left">Each game object currently has the following data.
If you need any extra unit information, create an [Issue][issues-url] in the form of a feature request.<p>
<p align="left">!Not possible: Aggregate player stats: (Kills, Deaths, Assists, CS, Vision Score)!<p>

1. Health/Max Health
2. Mana/Max Mana
3. Position
4. Team
5. Unit Id
6. Display Name
7. Current Gold / Total Gold
8. Level / Experience

(7 and 8 player only)

---

<h2 align="center">Offsets</h2>

Roughly every two weeks League of Legends updates and its memory layout changes slightly, so offsets must be updated.

Offsets will be updated within roughly 24-48 hours, though this may not always be the case. Keep in mind, this is an open source project maintained in free time, please do not harass me or others for offsets in the issues or anywhere else.
You are welcome to assist and contribute to this project by updating the offsets yourselves and creating a pull request so others can use them.

There is a [Text tutorial](https://github.com/floh22/LeagueBroadcast/tree/v2/LeagueBroadcast.Farsight) avaible which outlines the process. 


---

<h2 align="center">Methods</h2>

### connectToLeague

- `[out] success: bool` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Operation sucess

init library connection with the league process.


### disconnectFromLeague

- `[out] success: bool` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Operation sucess

close library connection with the league process.

### setOffsets

```js
var result = setOffsets({
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
```

- `[in] offsets: Object` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;An object containing the needed offsets. Object format can be seen in code example
- `[out] success: bool` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Operation sucess

Sets the offsets used for memory reading. Must be done before any snapshot is requested, otherwise snapshot creation fails. Currently cannot be reset once set.


### setChampionNames

- `[in] champions: string[]` &nbsp;A lowercase list of all champion names/ids
- `[out] success: bool` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Operation sucess

Sets the list of champions to scan for. Most common usage will be to use a full list of all champions currently in League of Legends. List must be lower case! Currently cannot be reset once set.
 
### isReady

- `[out] ready state: bool` &nbsp;&nbsp;&nbsp;Library readiness state.

Wether or not the library is ready to create Snapshots

### makeSnapshot

- `[out] snapshot: Object` &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Snapshot object containing current game state information


Creates a snapshot of all game objects currently in game. Farsight must be connected, as well as champion list and offsets provided.

## Members

### autoImportChampions

- `boolean`: Wether or not Farsight should automatically fetch the current list of all champions from [Community Dragon](https://communitydragon.org/). This takes place on `connectToLeague`.

True by default. If set to false, you must provide the list before requesting a snapshot.

### autoImportOffsets

- `boolean`: Wether or not Farsight should automatically fetch the current offsets from this repository. Repository currently not configurable! This takes place on `connectToLeague`.

True by default. If set to false, you must provide offsets before requesting a snapshot.

---

<h2 align="center">Example</h2>

Example usage is provided in the example folder of this repo. It currently contains a very basic console logger which conntects and prints the snapshot to the console. In case you write a simple use case using this library, please feel free to create a PR to add your example to this folder.


---

<!-- LICENSE -->
<h2 align="center">License</h2>

Distributed under the MIT License. See `LICENSE` for more information.

__This is a standalone project from Lars Eble. Riot Games does not endorse or sponsor this project.__

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/floh22/native-farsight-module.svg?style=for-the-badge
[contributors-url]: https://github.com/floh22/native-farsight-module/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/floh22/native-farsight-module.svg?style=for-the-badge
[forks-url]: https://github.com/floh22/native-farsight-module/network/members
[stars-shield]: https://img.shields.io/github/stars/floh22/native-farsight-module.svg?style=for-the-badge
[stars-url]: https://github.com/floh22/native-farsight-module/stargazers
[issues-shield]: https://img.shields.io/github/issues/floh22/native-farsight-module.svg?style=for-the-badge
[issues-url]: https://github.com/floh22/native-farsight-module/issues
[license-shield]: https://img.shields.io/github/license/floh22/native-farsight-module.svg?style=for-the-badge
[license-url]: https://github.com/floh22/native-farsight-module/blob/master/LICENSE
[donate-paypal]: https://img.shields.io/badge/Paypal-Donate-blueviolet?style=for-the-badge&logo=paypal
