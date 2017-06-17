
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <directfb.h>

#include <direct/debug.h>
#include <direct/messages.h>

#include <core/gfxcard.h>
#include <core/system.h>

#include <core/screens.h>
#include <core/state.h>
#include <core/coredefs.h>
#include <core/layers.h>
#include <core/surface.h>

#include <misc/conf.h>

#include <fbdev/fb.h>
#include <fbdev/fbdev.h>

#include <sys/ioctl.h>
#include <linux/ioctl.h>

#include "nuc970_2d.h"
#include "nuc970_gfxdriver.h"

D_DEBUG_DOMAIN( NUC970_Driver, "NUC970/Driver", "NUC970 graphics driver" );

#include <core/graphics_driver.h>

DFB_GRAPHICS_DRIVER( NUC970 )

/**********************************************************************************************************************/

static int
driver_probe( CoreGraphicsDevice *device )
{    
    switch (dfb_system_type()) {
          default:
               break;
     }

     return 1;
}

static void
driver_get_info( CoreGraphicsDevice *device,
                 GraphicsDriverInfo *info )
{
     /* fill driver info structure */
     snprintf( info->name,
               DFB_GRAPHICS_DRIVER_INFO_NAME_LENGTH,
               "NUC970 Driver" );

     snprintf( info->vendor,
               DFB_GRAPHICS_DRIVER_INFO_VENDOR_LENGTH,
               "nuvoton" );

     info->version.major = 0;
     info->version.minor = 1;

     info->driver_data_size = sizeof(NUC970DriverData);
     info->device_data_size = sizeof(NUC970DeviceData);
}

static DFBResult
driver_init_driver( CoreGraphicsDevice  *device,
                    GraphicsDeviceFuncs *funcs,
                    void                *driver_data,
                    void                *device_data,
                    CoreDFB             *core )
{
     D_DEBUG_AT( NUC970_Driver, "%s()\n", __FUNCTION__ );

     /* initialize function pointers */
     funcs->EngineSync    = nuc970EngineSync;
     funcs->EngineReset   = nuc970EngineReset;
     funcs->EmitCommands  = nuc970EmitCommands;
     funcs->CheckState    = nuc970CheckState;
     funcs->SetState      = nuc970SetState;
     funcs->FillRectangle = nuc970FillRectangle;
//     funcs->DrawRectangle = nuc970DrawRectangle;
     //funcs->DrawLine      = nuc970DrawLine;
//     funcs->FillTriangle  = nuc970FillTriangle;
     funcs->Blit          = nuc970Blit;
//     funcs->StretchBlit   = nuc970StretchBlit;

     return DFB_OK;
}

static DFBResult
driver_init_device( CoreGraphicsDevice *device,
                    GraphicsDeviceInfo *device_info,
                    void               *driver_data,
                    void               *device_data )
{
    NUC970DriverData *drv = (NUC970DriverData*) driver_data;
    NUC970DeviceData *dev = (NUC970DeviceData*) device_data;

    drv->ge2d_fd = open("/dev/ge2d", O_RDWR);

    if(drv->ge2d_fd < 0){
        printf("Cannot open ge2d!\n");
    }

     /* fill device info */
     snprintf( device_info->vendor, DFB_GRAPHICS_DEVICE_INFO_VENDOR_LENGTH, "Nuvoton Acceleration -" );
     snprintf( device_info->name,   DFB_GRAPHICS_DEVICE_INFO_NAME_LENGTH,   "%s", "nuc970" );

     /* device limitations */
     //device_info->limits.surface_byteoffset_alignment = 8;
     //device_info->limits.surface_bytepitch_alignment  = 8;

     //device_info->caps.flags    = CCF_CLIPPING | CCF_RENDEROPTS;
   
     device_info->caps.accel    = NUC970_SUPPORTED_DRAWINGFUNCTIONS |
                                  NUC970_SUPPORTED_BLITTINGFUNCTIONS;
     device_info->caps.drawing  = NUC970_SUPPORTED_DRAWINGFLAGS;
     device_info->caps.blitting = NUC970_SUPPORTED_BLITTINGFLAGS;
     
     return DFB_OK;
}

static void
driver_close_device( CoreGraphicsDevice *device,
                     void               *driver_data,
                     void               *device_data )
{
    NUC970DriverData *drv = (NUC970DriverData*) driver_data;
    NUC970DeviceData *dev = (NUC970DeviceData*) device_data;

    close(drv->ge2d_fd);
}

static void
driver_close_driver( CoreGraphicsDevice *device,
                     void               *driver_data )
{

}

