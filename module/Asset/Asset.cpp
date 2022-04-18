#include "Eyestack/Asset.hpp"

static inline void
__eyestack_init_resource()
{
  // Q_INIT_RESOURCE 这个宏不能在命名空间中被调用！
  Q_INIT_RESOURCE(asset);
}

namespace Eyestack::Asset {

void
ginit()
{
  __eyestack_init_resource();
}

}
