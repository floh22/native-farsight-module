#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>

#include "unit/GameObject.h"
#include "Benchmarks.h"

struct Snapshot
{
    std::vector<std::shared_ptr<GameObject>> champions;
    std::vector<std::shared_ptr<GameObject>> jungle;
    std::vector<std::shared_ptr<GameObject>> turrets;
    std::vector<std::shared_ptr<GameObject>> inhibitors;
    std::vector<std::shared_ptr<GameObject>> other;

    std::string nextDragonType;

    std::map<int, std::shared_ptr<GameObject>> objectMap;
    std::map<short, int> indexToNetId;

    std::set<int> updatedObjects;

    float gameTime = 0.f;

    std::unique_ptr<ReadBenchmark> benchmark = std::unique_ptr<ReadBenchmark>(new ReadBenchmark());


    Napi::Object ToNapiObject(Napi::Env env)
    {
        Napi::Object obj = Napi::Object::New(env);

        Napi::Array champions = Napi::Array::New(env, this->champions.size());
        for (int i = 0; i < this->champions.size(); i++)
        {
            champions[i] = this->champions[i]->ToNapiObject();
        }
        obj.Set("champions", champions);

        Napi::Array jungle = Napi::Array::New(env, this->jungle.size());
        for (int i = 0; i < this->jungle.size(); i++)
        {
            jungle[i] = this->jungle[i]->ToNapiObject();
        }
        obj.Set("jungle", jungle);

        Napi::Array turrets = Napi::Array::New(env, this->turrets.size());
        for (int i = 0; i < this->turrets.size(); i++)
        {
            turrets[i] = this->turrets[i]->ToNapiObject();
        }
        obj.Set("turrets", turrets);

        Napi::Array inhibitors = Napi::Array::New(env, this->inhibitors.size());
        for (int i = 0; i < this->inhibitors.size(); i++)
        {
            inhibitors[i] = this->inhibitors[i]->ToNapiObject();
        }
        obj.Set("inhibitors", inhibitors);

        Napi::Array other = Napi::Array::New(env, this->other.size());
        for (int i = 0; i < this->other.size(); i++)
        {
            other[i] = this->other[i]->ToNapiObject();
        }
        obj.Set("other", other);

        obj.Set("gameTime", this->gameTime);

        obj.Set("nextDragonType", Napi::String::New(env, this->nextDragonType));
        return obj;
    }
};

struct ChampionSnapshot
{
    std::vector<std::shared_ptr<GameObject>> champions;

    float gameTime = 0.f;

    std::unique_ptr<ReadBenchmark> benchmark = std::unique_ptr<ReadBenchmark>(new ReadBenchmark());
};