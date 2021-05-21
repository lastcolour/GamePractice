#ifndef __POLYMORPH_PTR_UTILS_HPP__
#define __POLYMORPH_PTR_UTILS_HPP__

struct SerializeContext;
class ClassInstance;
class JSONNode;

namespace ReflectUtils {

bool ReadPolyPtrFrom(const SerializeContext& ctx, ClassInstance& instance, TypeId baseClassId, const JSONNode& node);

bool WritePolyPtrTo(const SerializeContext& ctx, ClassInstance& instance, JSONNode& node);

}  // namepsace ReflectUtils

#endif /* __POLYMORPH_PTR_UTILS_HPP__ */