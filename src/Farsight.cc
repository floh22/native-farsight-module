#include <set>
#include <queue>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <napi.h>
#include "Offsets.h"
#include "unit/GameObject.h"
#include "Farsight.h"
#include "windows.h"
#include "psapi.h"
#include "Utils.h"
std::set<std::string> Farsight::championNames{};

Farsight::Farsight()
{
    // Some trash object not worth reading, most likely outdated
    blacklistedObjectNames.insert("testcube");
    blacklistedObjectNames.insert("testcuberender");
    blacklistedObjectNames.insert("testcuberender10vision");
    blacklistedObjectNames.insert("s5test_wardcorpse");
    blacklistedObjectNames.insert("sru_camprespawnmarker");
    blacklistedObjectNames.insert("sru_plantrespawnmarker");
    blacklistedObjectNames.insert("preseason_turret_shield");
}

bool Farsight::IsLeagueRunning()
{
    // Check if the process is running
    hWindow = FindWindowA(NULL, "League of Legends (TM) Client");

    DWORD h;
    GetWindowThreadProcessId(hWindow, &h);
    return pid == h;
}

bool Farsight::IsHooked()
{
    return Process::IsProcessRunning(pid);
}


void Farsight::UnhookFromProcess()
{
    CloseHandle(hProcess);
    hProcess = NULL;
    pid = 0;
    hWindow = NULL;
    baseAddress = 0;
    size = 0;
    isSixtyFourBit = FALSE;
}

void Farsight::HookToProcess()
{
    // Get the process id
    hWindow = FindWindowA("RiotWindowClass", NULL);

    if (hWindow == NULL)
        throw WinApiException("Could not find League of Legends window");

    GetWindowThreadProcessId(hWindow, &pid);

    if (pid == NULL || !Process::IsProcessRunning(pid))
        throw WinApiException("Could not find League of Legends process");

    // Open the process
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (hProcess == NULL)
        throw WinApiException("Could not open League of Legends process");

    if (0 == IsWow64Process(hProcess, &isSixtyFourBit))
        throw WinApiException("Could not determine bitness of League of Legends process");

    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        baseAddress = (DWORD_PTR)hMods[0];
    }
    else
    {
        throw WinApiException("Couldn't retrieve league base address");
    }

    blacklistedObjects.clear();
}

void Farsight::ClearMissingObjects(Snapshot &snapshot)
{
    auto current = snapshot.objectMap.begin();

    while (current != snapshot.objectMap.end())
    {
        if (snapshot.updatedObjects.find(current->first) == snapshot.updatedObjects.end())
        {
            current = snapshot.objectMap.erase(current);
        }
        else
            ++current;
    }
}

void Farsight::ReadObjects(Snapshot &snapshot, Napi::Env env)
{

    static const int maxObjects = 1024;
    static DWORD64 pointerArray[maxObjects];

    std::chrono::high_resolution_clock::time_point readTimeBegin;
    std::chrono::duration<float, std::milli> readDuration;
    readTimeBegin = std::chrono::high_resolution_clock::now();

    snapshot.champions.clear();
    snapshot.jungle.clear();
    snapshot.inhibitors.clear();
    snapshot.turrets.clear();
    snapshot.other.clear();
    snapshot.nextDragonType.clear();

    DWORD64 objectManager = Memory::ReadDWORD64(hProcess, baseAddress + Offsets::ObjectManager);

    if(objectManager == 0)
        return;
        
    static char buff[0x500];
    Memory::Read(hProcess, objectManager, buff, 0x100);

    DWORD64 rootNode;
    memcpy(&rootNode, buff + Offsets::ObjectMapRoot, sizeof(DWORD64));

    std::queue<DWORD64> nodesToVisit;
    std::set<DWORD64> visitedNodes;
    nodesToVisit.push(rootNode);

    // Read object pointers from tree
    int nrObj = 0;
    int reads = 0;
    DWORD64 childNode1, childNode2, childNode3, node;
    while (reads < maxObjects && nodesToVisit.size() > 0)
    {
        node = nodesToVisit.front();
        nodesToVisit.pop();
        if (visitedNodes.find(node) != visitedNodes.end())
            continue;

        reads++;
        visitedNodes.insert(node);
        Memory::Read(hProcess, node, buff, 0x50);

        memcpy(&childNode1, buff, sizeof(DWORD64));
        memcpy(&childNode2, buff + 8, sizeof(DWORD64));
        memcpy(&childNode3, buff + 16, sizeof(DWORD64));

        nodesToVisit.push(childNode1);
        nodesToVisit.push(childNode2);
        nodesToVisit.push(childNode3);

        unsigned int netId = 0;
        memcpy(&netId, buff + Offsets::ObjectMapNodeNetId, sizeof(int));

        DWORD64 addr;
        memcpy(&addr, buff + Offsets::ObjectMapNodeObject, sizeof(DWORD64));
        
        //Im pretty sure netids did not increase to 64bit integers, so anything above 2^31 is invalid
        if (addr == 0 || netId > MAX_UINT || netId == 0)
            continue;

        pointerArray[nrObj] = addr;
        nrObj++;
    }

    // Read objects from the pointers we just read
    for (int i = 0; i < nrObj; ++i)
    {
        int netId;
        Memory::Read(hProcess, pointerArray[i] + Offsets::ObjNetworkID, &netId, sizeof(int));
        if (blacklistedObjects.find(netId) != blacklistedObjects.end())
            continue;

        std::shared_ptr<GameObject> obj;
        auto it = snapshot.objectMap.find(netId);
        if (it == snapshot.objectMap.end())
        {
            obj = std::shared_ptr<GameObject>(new GameObject(env));
            obj->LoadFromMemory(pointerArray[i], hProcess, true);
            snapshot.objectMap[obj->networkId] = obj;
        }
        else
        {
            obj = it->second;
            obj->LoadFromMemory(pointerArray[i], hProcess, false);

            if (netId != obj->networkId)
            {
                snapshot.objectMap[obj->networkId] = obj;
            }
        }

        if (obj->networkId == 0)
        {
            continue;
        }
        snapshot.indexToNetId[obj->objectIndex] = obj->networkId;
        snapshot.updatedObjects.insert(obj->networkId);

        /* Do not filter out names to include troy objects
        if(obj->name.size() < 2 || blacklistedObjectNames.find(obj->name) != blacklistedObjectNames.end()) {
            blacklistedObjects.insert(obj->networkId);
            continue;
        }

        */

        std::string name = std::string(obj->name.begin(), obj->name.end());
        std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });

        if (championNames.find(name) != championNames.end())
        {
            obj->LoadChampionData();
            snapshot.champions.push_back(obj);
            continue;
        }

        const char *inhibitorText = "Barracks_T";
        const char *turretText = "Turret_T";

        if (Character::VectorStartsWith(obj->displayName, inhibitorText))
        {
            snapshot.inhibitors.push_back(obj);
            continue;
        }

        if (Character::VectorStartsWith(obj->displayName, turretText))
        {
            snapshot.turrets.push_back(obj);
            continue;
        }

        if(Character::VectorStartsWith(obj->displayName, "Dragon_Indicator_"))
        {
            std::vector<char> dragonType(obj->displayName);
            // dragonType.erase(dragonType.end() - 5, dragonType.end());
            dragonType.erase(dragonType.begin(), dragonType.begin() + 17);
            snapshot.nextDragonType = std::string(dragonType.begin(), dragonType.end());
            snapshot.jungle.push_back(obj);
            continue;
        }

        if (Character::VectorStartsWith(obj->name, "SRU_Dragon"))
        {
            snapshot.jungle.push_back(obj);
            continue;
        }

        if (Character::VectorStartsWith(obj->name, "SRU_Baron"))
        {
            snapshot.jungle.push_back(obj);
            continue;
        }

        if (Character::VectorStartsWith(obj->name, "SRU_RiftHerald"))
        {
            snapshot.jungle.push_back(obj);
            continue;
        }

        snapshot.other.push_back(obj);
    }

    readDuration = std::chrono::high_resolution_clock::now() - readTimeBegin;
    snapshot.benchmark->readObjectsMs = readDuration.count();
}

void Farsight::ReadChampions(ChampionSnapshot &snapshot, Napi::Env env)
{
}

void Farsight::CreateSnapshot(Snapshot &snapshot, Napi::Env env)
{
    Memory::Read(hProcess, baseAddress + Offsets::GameTime, &snapshot.gameTime, sizeof(float));

    if (snapshot.gameTime <= 5.0f)
        return;

    ReadObjects(snapshot, env);
    ClearMissingObjects(snapshot);
};

void Farsight::CreateChampionSnapshot(ChampionSnapshot &championSnapshot, Napi::Env env)
{
    Memory::Read(hProcess, baseAddress + Offsets::GameTime, &championSnapshot.gameTime, sizeof(float));
}