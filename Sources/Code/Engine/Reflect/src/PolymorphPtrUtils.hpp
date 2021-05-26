#ifndef __POLYMORPH_PTR_UTILS_HPP__
#define __POLYMORPH_PTR_UTILS_HPP__

struct SerializeContext;
class ClassInstance;
class JSONNode;
class MemoryStream;

namespace ReflectUtils {

bool UpdateInstanceClass(ClassInstance& instance, TypeId baseClassId, const std::string& typeName);

bool ReadPolyPtrFrom(const SerializeContext& ctx, ClassInstance& instance, TypeId baseClassId, const JSONNode& node);

bool ReadPolyPtrFrom(const SerializeContext& ctx, ClassInstance& instance, TypeId baseClassId, MemoryStream& stream);

bool WritePolyPtrTo(const SerializeContext& ctx, ClassInstance& instance, JSONNode& node);

bool WritePolyPtrTo(const SerializeContext& ctx, ClassInstance& instance, MemoryStream& stream);

std::string GetPolymorphPtrTypeName(TypeId baseClassId);

}  // namepsace ReflectUtils

#endif /* __POLYMORPH_PTR_UTILS_HPP__ */