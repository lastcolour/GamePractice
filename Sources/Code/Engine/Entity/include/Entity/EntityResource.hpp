#ifndef __ENTITY_RESOURCE_HPP__
#define __ENTITY_RESOURCE_HPP__

#include "Reflect/Resource.hpp"

struct EntityResourceTypeDescriptor {
    using RuntimeType = std::string;
    using StorageType = std::string;
    const char* TypeName = "Entity";
};

class EntityResource : private Reflect::Resource<EntityResourceTypeDescriptor> {
public:

    using SpecType = EntityResourceTypeDescriptor;

public:

    bool isValid() const { return isSet() && !get().empty(); }
    const char* getPath() const;
    void setPath(const char* newPath);
    EntityId createInstance() const;
};

#endif /* __ENTITY_RESOURCE_HPP__ */