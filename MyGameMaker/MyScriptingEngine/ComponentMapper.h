#pragma once
#ifndef COMPONENT_MAPPER_H
#define COMPONENT_MAPPER_H

#include <typeindex>
#include <string>
#include <unordered_map>

class ComponentMapper {
public:
    ComponentMapper();
    ~ComponentMapper();

    std::string GetMappedName(const std::type_index& typeIndex) const;
    void AddMapping(const std::type_index& typeIndex, const std::string& mappedName);

private:
    std::unordered_map<std::type_index, std::string> mappings;
};

#endif // COMPONENT_MAPPER_H