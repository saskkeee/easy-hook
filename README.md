**Example**
```c++
#include "easy_hook.hpp"

easy_hook get_fov { };

float get_number(void* rcx)
{
    if (true)
      return get_fov.call<float,fastcall_tag_t>(rcx);

    return 90.0f;
}

void init()
{
    get_fov.hook(reinterpret_cast<void*>(0x5C7AB0),&get_number); // 0x5C7AB0 - Function Address
    get_fov.virtual_hook(reinterpret_cast<void*>(0x5C7AB0),12,&get_number); // 12 - Function Index
}
```
