#include <napi.h>
#include <string>
#include "Farsight.h"
#include "Snapshot.h"
#include "windows.h"
#include "Offsets.h"
#include <chrono>

Farsight f = Farsight();

Napi::Object makeSnapshot(const Napi::CallbackInfo &info)
{
    if(!f.IsHooked()) {
        Napi::Env env = info.Env();
        Napi::Object res = Napi::Object::New(env);
        res.Set("success", Napi::Boolean::New(env, false));
        res.Set("error", "Not hooked to League of Legends");
        return res;
    }


    // Init variables and timer
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> readDuration;


    Napi::Env env = info.Env();

    Snapshot s;

    f.CreateSnapshot(s, env);

    Napi::Object snapshot = s.ToNapiObject(env);


    readDuration = std::chrono::high_resolution_clock::now() - start;
    s.benchmark->fullUpdateMs = readDuration.count();

    snapshot.Set("benchmark", s.benchmark->ToNapiObject(env));

    return snapshot;
}

Napi::Object disconnectFromLeague(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Object res = Napi::Object::New(env);
    f.UnhookFromProcess();
    res.Set("success", Napi::Boolean::New(env, !f.IsHooked()));
    return res;
}

Napi::Object connectToLeague(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Object res = Napi::Object::New(env);
    try
    {
        f.HookToProcess();
    }
    catch (const std::exception &e)
    {
        res.Set("success", Napi::Boolean::New(env, false));
        res.Set("error", e.what());
        return res;
    }
    res.Set("success", Napi::Boolean::New(env, f.IsHooked()));
    return res;
}

Napi::Object setOffsets(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Object offsets = info[0].As<Napi::Object>();
    Napi::Object res = Napi::Object::New(env);

    Offsets::GameTime = offsets.Get("gameTime").As<Napi::Number>().Int32Value();
    Offsets::ObjectManager = offsets.Get("objectManager").As<Napi::Number>().Int32Value();
    Offsets::ObjectMapCount = offsets.Get("objectMapCount").As<Napi::Number>().Int32Value();
    Offsets::ObjectMapRoot = offsets.Get("objectMapRoot").As<Napi::Number>().Int32Value();
    Offsets::ObjectMapNodeNetId = offsets.Get("objectMapNodeNetId").As<Napi::Number>().Int32Value();
    Offsets::ObjectMapNodeObject = offsets.Get("objectMapNodeObject").As<Napi::Number>().Int32Value();

    Offsets::HeroList = offsets.Get("heroList").As<Napi::Number>().Int32Value();
    Offsets::MinionList = offsets.Get("minionList").As<Napi::Number>().Int32Value();
    Offsets::TurretList = offsets.Get("turretList").As<Napi::Number>().Int32Value();
    Offsets::InhibitorList = offsets.Get("inhibitorList").As<Napi::Number>().Int32Value();

    Offsets::ObjIndex = offsets.Get("objIndex").As<Napi::Number>().Int32Value();
    Offsets::ObjTeam = offsets.Get("objTeam").As<Napi::Number>().Int32Value();
    Offsets::ObjNetworkID = offsets.Get("objNetworkID").As<Napi::Number>().Int32Value();
    Offsets::ObjPosition = offsets.Get("objPosition").As<Napi::Number>().Int32Value();
    Offsets::ObjHealth = offsets.Get("objHealth").As<Napi::Number>().Int32Value();
    Offsets::ObjMaxHealth = offsets.Get("objMaxHealth").As<Napi::Number>().Int32Value();
    Offsets::ObjMana = offsets.Get("objMana").As<Napi::Number>().Int32Value();
    Offsets::ObjMaxMana = offsets.Get("objMaxMana").As<Napi::Number>().Int32Value();
    Offsets::ObjName = offsets.Get("objName").As<Napi::Number>().Int32Value();
    Offsets::ObjNameLength = offsets.Get("objNameLength").As<Napi::Number>().Int32Value();
    Offsets::ObjLvl = offsets.Get("objLvl").As<Napi::Number>().Int32Value();
    Offsets::ObjExperience = offsets.Get("objExperience").As<Napi::Number>().Int32Value();
    Offsets::ObjCurrentGold = offsets.Get("objCurrentGold").As<Napi::Number>().Int32Value();
    Offsets::ObjTotalGold = offsets.Get("objTotalGold").As<Napi::Number>().Int32Value();
    Offsets::ObjDisplayName = offsets.Get("objDisplayName").As<Napi::Number>().Int32Value();
    Offsets::ObjDisplayNameLength = offsets.Get("objDisplayNameLength").As<Napi::Number>().Int32Value();


   res.Set("success", Napi::Boolean::New(env, true));

   return res;
}

Napi::Number setChampionNames(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::Array championNames = info[0].As<Napi::Array>();

    for (unsigned int i = 0; i < championNames.Length(); i++) {
        std::string champion = championNames.Get(i).As<Napi::String>().Utf8Value();
        Farsight::championNames.insert(champion);
    }

     return Napi::Number::New(env, Farsight::championNames.size());
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{

    exports.Set(
        Napi::String::New(env, "makeSnapshot"),
        Napi::Function::New(env, makeSnapshot));

    exports.Set(
        Napi::String::New(env, "connectToLeague"),
        Napi::Function::New(env, connectToLeague));

    exports.Set(
        Napi::String::New(env, "disconnectFromLeague"),
        Napi::Function::New(env, disconnectFromLeague));

    exports.Set(
        Napi::String::New(env, "setOffsets"),
        Napi::Function::New(env, setOffsets));

    exports.Set(
        Napi::String::New(env, "setChampionNames"),
        Napi::Function::New(env, setChampionNames));


    return exports;
}

NODE_API_MODULE(farsight, Init);