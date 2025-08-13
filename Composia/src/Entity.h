#ifndef COMPOSIA_ENTITY_H
#define COMPOSIA_ENTITY_H

#include <cstdint> // uint32_t
#include <limits> //  std::numeric_limits
 

namespace Composia {

using Entity = uint32_t;
static constexpr Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();

} // namespace Composia


#endif // !COMPOSIA_ENTITY_H
