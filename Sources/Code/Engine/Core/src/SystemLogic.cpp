#include "Core/SystemLogic.hpp"
#include "Core/ETSystem.hpp"

SystemLogic::SystemLogic() :
    entityId(GetETSystem()->createNewEntityId()) {
}