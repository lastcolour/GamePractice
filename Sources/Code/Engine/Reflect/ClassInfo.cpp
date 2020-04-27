#include "Reflect/ClassInfo.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/JSONNode.hpp"
#include "Core/ETPrimitives.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

template<typename T>
T* getRef(void* value) {
    return reinterpret_cast<T*>(value);
}

} // namespace

ClassInfo::ClassInfo() {
}

ClassInfo::~ClassInfo() {
}

ClassInstanceDeleteFuncT ClassInfo::getDeleteFunction() const {
    return deleteFunc;
}

TypeId ClassInfo::getIntanceTypeId() const {
    return instanceTypeId;
}

bool ClassInfo::serializeInstance(void* instance, const JSONNode& classNode) {
    for(auto& val : values) {
        if(!classNode.hasKey(val.name.c_str())) {
            return false;
        }
        auto ptr = getValueFunc(instance, val.ptr);
        switch (val.type)
        {
        case ClassValueType::Invalid: {
            break;
        }
        case ClassValueType::Bool: {
            bool value = false;
            classNode.read(val.name.c_str(), value);
            *getRef<bool>(ptr) = value;
            break;
        }
        case ClassValueType::Int: {
            int value = false;
            classNode.read(val.name.c_str(), value);
            *getRef<int>(ptr) = value;
            break;
        }
        case ClassValueType::Float: {
            float value = 0.f;
            classNode.read(val.name.c_str(), value);
            *getRef<float>(ptr) = value;
            break;
        }
        case ClassValueType::String: {
            std::string value;
            classNode.read(val.name.c_str(), value);
            *getRef<std::string>(ptr) = value;
            break;
        }
        case ClassValueType::Object: {
            auto objectNode = classNode.object(val.name.c_str());
            if(!objectNode) {
                return false;
            }
            ClassInfo* classInfo = nullptr;
            ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, val.typeId);
            if(!classInfo) {
                return false;
            }
            if(!classInfo->serializeInstance(ptr, objectNode)) {
                return false;
            }
            break;
        }
        default:
            return false;
        }
    }
    return true;
}

const char* ClassInfo::getName() const {
    return className.c_str();
}

ClassInfo* ClassInfo::findClassInfo(TypeId instanceTypeId) const {
    ClassInfo* classInfo = nullptr;
    ET_SendEventReturn(classInfo, &ETClassInfoManager::ET_findClassInfoByTypeId, instanceTypeId);
    return classInfo;
}

ClassInstance ClassInfo::createInstance(const JSONNode& node) {
    if(!createFunc) {
        return ClassInstance();
    }
    auto object = createFunc();
    if(!object) {
        return ClassInstance();
    }
    if(!serializeInstance(object, node)) {
        deleteFunc(object);
        return ClassInstance();
    }
    return ClassInstance(*this, object);
}

bool ClassInfo::isFieldNameValid(const char* name) const {
    return true;
}

bool ClassInfo::isClassNameValid(const char* name) const {
    if(!name || !name[0]) {
        return false;
    }
    ClassInfo* clsInfo = nullptr;
    ET_SendEventReturn(clsInfo, &ETClassInfoManager::ET_findClassInfoByName, name);
    if(clsInfo) {
        return false;
    }
    return true;
}

bool ClassInfo::reflectEmbebedClass(const std::function<void(ReflectContext& ctx)>& reflectFunc) {
    ReflectContext ctx;
    return ctx.reflectEmbedded(reflectFunc);
}

 void ClassInfo::makeReflectModel(JSONNode& node) {
    for(auto& value : values) {
        node.write(value.name.c_str(), value.getTypeName());
    }
}