#pragma once
#include <napi.h>

struct ReadBenchmark
{
    float readObjectsMs;
    float fullUpdateMs;


    inline Napi::Object ToNapiObject(Napi::Env env)
    {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("readObjectsMs", readObjectsMs);
        obj.Set("fullUpdateMs", fullUpdateMs);
        return obj;
    }
};