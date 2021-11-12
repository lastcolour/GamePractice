namespace Core {

namespace Impl {

int GLOBAL_TYPE_ID = InvalidTypeId;

int GetNextTypeId() {
    ++GLOBAL_TYPE_ID;
    return GLOBAL_TYPE_ID;
}

} // namespace Impl

} // namespace Core