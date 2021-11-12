#ifndef __POLYMORPH_PTR_UTILS_HPP__
#define __POLYMORPH_PTR_UTILS_HPP__

#include "Reflect/PolymorphPtr.hpp"

class JSONNode;

namespace Reflect {

bool UpdateInstanceClass(ClassInstance& instance, Core::TypeId baseClassId, const std::string& typeName);

bool ReadPolyPtrFrom(const SerializeContext& ctx, ClassInstance& instance, Core::TypeId baseClassId, const JSONNode& node);

bool ReadPolyPtrFrom(const SerializeContext& ctx, ClassInstance& instance, Core::TypeId baseClassId, Memory::MemoryStream& stream);

bool WritePolyPtrTo(const SerializeContext& ctx, ClassInstance& instance, JSONNode& node);

bool WritePolyPtrTo(const SerializeContext& ctx, ClassInstance& instance, Memory::MemoryStream& stream);

std::string GetPolymorphPtrTypeName(Core::TypeId baseClassId);

}  // namepsace Reflect

#endif /* __POLYMORPH_PTR_UTILS_HPP__ */