#pragma once
#include <string>
#include <cstddef>

class SpellData
{
    public:
        std::byte level;
        std::string name;
        float cooldown;
};