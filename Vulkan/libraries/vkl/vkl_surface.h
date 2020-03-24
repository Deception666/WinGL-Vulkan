#ifndef _VKL_SURFACE_H_
#define _VKL_SURFACE_H_

#include "vkl_instance_fwds.h"
#include "vkl_surface_fwds.h"
#include "vkl_window_fwds.h"

namespace vkl
{

SurfaceHandle CreateSurface(
   const InstanceHandle & instance,
   const WindowHandle & window );

} // namespace vkl

#endif // _VKL_SURFACE_H_