
#include <config.h>

#include <directfb.h>

#include <direct/debug.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <core/state.h>
#include <core/surface.h>
#include <core/system.h>

#include <gfx/convert.h>

#include <sys/ioctl.h>
#include <linux/ioctl.h>


#include "nuc970_2d.h"
#include "nuc970_gfxdriver.h"


D_DEBUG_DOMAIN( NUC970__2D, "NUC970/2D", "NUC970 2D Acceleration" );

typedef enum
{
	G2D_BLACK = 0, G2D_RED = 1, G2D_GREEN = 2, G2D_BLUE = 3, G2D_WHITE = 4, 
	G2D_YELLOW = 5, G2D_CYAN = 6, G2D_MAGENTA = 7
} G2D_COLOR;

#define NUC970_GE2D_START_BITBLT		_IO('G',0)
#define NUC970_GE2D_START_BITBLT_ROP	_IO('G',1)
#define NUC970_GE2D_FILL_RECTANGLE	    _IO('G',2)
#define NUC970_GE2D_ROTATION		    _IO('G',3)
#define NUC970_GE2D_LINE		        _IO('G',4)
#define NUC970_GE2D_STRETCH		        _IO('G',5)


/*
 * State validation flags.
 *
 * There's no prefix because of the macros below.
 */
enum {
     DESTINATION  = 0x00000001,
     SCISSOR      = 0x00000002,
     MATRIX       = 0x00000004,
     RENDER_OPTS  = 0x00000008,

     COLOR_DRAW   = 0x00000010,

     SOURCE       = 0x00000100,
     COLOR_BLIT   = 0x00000200,

     BLENDFUNC    = 0x00010000,

     ALL          = 0x0001031F
};

/*
 * State handling macros.
 */

#define NUC970_VALIDATE(flags)        do { ndev->v_flags |=  (flags); } while (0)
#define NUC970_INVALIDATE(flags)      do { ndev->v_flags &= ~(flags); } while (0)

#define NUC970_CHECK_VALIDATE(flag)   do {                                               \
                                       if ((ndev->v_flags & flag) != flag)           \
                                            nuc970_validate_##flag( ndrv, ndev, state ); \
                                  } while (0)

/**********************************************************************************************************************/

/*
 * Called by nuc970SetState() to ensure that the destination parameters are properly set
 * for execution of rendering functions.
 */
static inline void
nuc970_validate_DESTINATION( NUC970DriverData *ndrv,
                         NUC970DeviceData *ndev,
                         CardState    *state )
{
    CoreSurface  *surface = state->destination;

    ndrv->params.dst_work_width  = state->destination->config.size.w;
    ndrv->params.dst_work_height = state->destination->config.size.h;
    ndrv->params.dst_full_width  = state->destination->config.size.w;
    ndrv->params.dst_full_height = state->destination->config.size.h;

    /* set dst phy addr */
    ndrv->params.dst_base_addr = state->dst.phys;

    switch (surface->config.format) 
    {
        case DSPF_RGB332:
            ndrv->params.bpp_src = 8;
            break;
        case DSPF_ARGB1555:
            ndrv->params.bpp_src = 16;
            break;
        case DSPF_RGB16:
            ndrv->params.bpp_src = 16;
            break;
        case DSPF_RGB24:
            ndrv->params.bpp_src = 32;   
            break;
        case DSPF_RGB32:
        case DSPF_ARGB:
            ndrv->params.bpp_src = 32;
            break;
        default:
            D_BUG( "unexpected pixelformat!" );
            break;
    }

    /* Set the flag. */
    NUC970_VALIDATE( DESTINATION );
}

/*
 * Called by nuc970SetState() to ensure that the clip is properly set
 * for execution of rendering functions.
 */
static inline void
nuc970_validate_SCISSOR( NUC970DriverData *ndrv,
                     NUC970DeviceData *ndev,
                     CardState    *state )
{
    D_DEBUG_AT( NUC970__2D, "%s()\n", __FUNCTION__ );

    ndrv->params.cw_x1 = state->clip.x1;
    ndrv->params.cw_y1 = state->clip.y1;
    ndrv->params.cw_x2 = state->clip.x2;
    ndrv->params.cw_y2 = state->clip.y2;

    /* Set the flag. */
    NUC970_VALIDATE( SCISSOR );
}

/*
 * Called by nuc970_validate_SOURCE() to ensure that the source registers are properly set
 * for execution of blitting functions.
 */
static inline void
nuc970_validate_SOURCE( NUC970DriverData *ndrv,
                        NUC970DeviceData *ndev,
                        CardState    *state )
{
    
    D_DEBUG_AT( NUC970__2D, "%s()\n", __FUNCTION__ );

    /* set src phy addr */
    ndrv->params.src_base_addr = state->src.phys;
    
    ndrv->params.src_work_width  = state->source->config.size.w;
    ndrv->params.src_work_height = state->source->config.size.h;
    ndrv->params.src_full_width  = state->source->config.size.w;
    ndrv->params.src_full_height = state->source->config.size.h;

     /* Set the flag. */
     NUC970_INVALIDATE( SOURCE );
}

/*
 * Called by nuc970SetState() to ensure that the color is properly set
 * for execution of drawing functions.
 */
static inline void
nuc970_validate_COLOR_DRAW( NUC970DriverData *ndrv,
                        NUC970DeviceData *ndev,
                        CardState    *state )
{
    D_DEBUG_AT( NUC970__2D, "%s()\n", __FUNCTION__ );

    if (state->drawingflags & DSDRAW_SRC_PREMULTIPLY) {
        ndrv->params.alpha_mode = 1;
        ndrv->params.alpha_val = state->color.a;
    }

    ndrv->params.color_val[G2D_RED] = state->color.r;
    ndrv->params.color_val[G2D_GREEN] = state->color.g;
    ndrv->params.color_val[G2D_BLUE] = state->color.b;

    /* Set the flag. */
    NUC970_VALIDATE( COLOR_DRAW );

    /* Invalidates blitting color. */
    NUC970_INVALIDATE( COLOR_BLIT );
}

/*
 * Called by nuc970SetState() to ensure that the color is properly set
 * for execution of blitting functions.
 */
static inline void
nuc970_validate_COLOR_BLIT( NUC970DriverData *ndrv,
                        NUC970DeviceData *ndev,
                        CardState    *state )
{
    D_DEBUG_AT( NUC970__2D, "%s()\n", __FUNCTION__ );

    if (state->blittingflags & DSBLIT_BLEND_COLORALPHA)
        ndrv->params.alpha_mode = 1;

    if (state->blittingflags & DSBLIT_SRC_PREMULTCOLOR) {
        ndrv->params.alpha_mode = 1;
    }

    ndrv->params.alpha_val = state->color.a;
    ndrv->params.color_val[G2D_RED] = state->color.r;
    ndrv->params.color_val[G2D_GREEN] = state->color.g;
    ndrv->params.color_val[G2D_BLUE] = state->color.b;

    /* Set the flag. */
    NUC970_VALIDATE( COLOR_BLIT );

    /* Invalidates drawing color. */
    NUC970_INVALIDATE( COLOR_DRAW );
}

/**********************************************************************************************************************/

/*
 * Wait for the blitter to be idle.
 *
 * This function is called before memory that has been written to by the hardware is about to be
 * accessed by the CPU (software driver) or another hardware entity like video encoder (by Flip()).
 * It can also be called by applications explicitly, e.g. at the end of a benchmark loop to include
 * execution time of queued commands in the measurement.
 */
DFBResult
nuc970EngineSync( void *drv, void *dev )
{
    return DFB_OK;
}

/*
 * Reset the graphics engine.
 */
void
nuc970EngineReset( void *drv, void *dev )
{
    D_DEBUG_AT( NUC970__2D, "%s()\n", __FUNCTION__ );
}

/*
 * Start processing of queued commands if required.
 *
 * This function is called before returning from the graphics core to the application.
 * Usually that's after each rendering function. The only functions causing multiple commands
 * to be queued with a single emition at the end are DrawString(), TileBlit(), BatchBlit(),
 * DrawLines() and possibly FillTriangle() which is emulated using multiple FillRectangle() calls.
 */
void
nuc970EmitCommands( void *drv, void *dev )
{

}

/**********************************************************************************************************************/

/*
 * Check for acceleration of 'accel' using the given 'state'.
 */
void
nuc970CheckState( void                *drv,
              void                *dev,
              CardState           *state,
              DFBAccelerationMask  accel )
{
    D_DEBUG_AT( NUC970__2D, "%s( state %p, accel 0x%08x ) <- dest %p [%lu]\n", __FUNCTION__,
                 state, accel, state->destination, state->dst.offset );

     /* Return if the desired function is not supported at all. */
     if (accel & ~(NUC970_SUPPORTED_DRAWINGFUNCTIONS | NUC970_SUPPORTED_BLITTINGFUNCTIONS)) {
          D_DEBUG_AT( NUC970__2D, "  -> unsupported function\n" );
          return;
     }

     /* Return if the destination format is not supported. */
     switch (state->destination->config.format) {
          //case DSPF_ARGB1555:
          case DSPF_RGB16:
          //case DSPF_ARGB:
               break;
          default:
               D_DEBUG_AT( NUC970__2D, "  -> unsupported destination format %s\n",
                           dfb_pixelformat_name(state->destination->config.format) );
               return;
     }

     /* Check if drawing or blitting is requested. */
     if (DFB_DRAWING_FUNCTION( accel )) {
          /* Return if unsupported drawing flags are set. */
          if (state->drawingflags & ~NUC970_SUPPORTED_DRAWINGFLAGS) {
               D_DEBUG_AT( NUC970__2D, "  -> unsupported drawing flags 0x%08x\n", state->drawingflags );
               return;
          }
     }
     else {
          /* Return if the source format is not supported. */
          switch (state->source->config.format) {
               //case DSPF_ARGB1555:
               case DSPF_RGB16:
               //case DSPF_ARGB:
                    break;
               default:
                    D_DEBUG_AT( NUC970__2D, "  -> unsupported source format %s\n",
                                dfb_pixelformat_name(state->source->config.format) );
                    return;
          }

          /* Return if unsupported blitting flags are set. */
          if (state->blittingflags & ~NUC970_SUPPORTED_BLITTINGFLAGS) {
               D_DEBUG_AT( NUC970__2D, "  -> unsupported blitting flags 0x%08x\n", state->blittingflags );
               return;
          }
     }

     /* Enable acceleration of the function. */
     state->accel |= accel;

     D_DEBUG_AT( NUC970__2D, "  => OK\n" );
}

/*
 * Make sure that the hardware is programmed for execution of 'accel' according to the 'state'.
 */
void
nuc970SetState( void                *drv,
            void                *dev,
            GraphicsDeviceFuncs *funcs,
            CardState           *state,
            DFBAccelerationMask  accel )
{
     NUC970DriverData           *ndrv     = drv;
     NUC970DeviceData           *ndev     = dev;
     StateModificationFlags  modified = state->mod_hw;

     D_DEBUG_AT( NUC970__2D, "%s( state %p, accel 0x%08x ) <- dest %p, modified 0x%08x\n", __FUNCTION__,
                 state, accel, state->destination, modified );

     /*
      * 1) Invalidate hardware states
      *
      * Each modification to the hw independent state invalidates one or more hardware states.
      */

     /* Simply invalidate all? */
     if (modified == SMF_ALL) {
          NUC970_INVALIDATE( ALL );
     }
     else if (modified) {
          if (modified & SMF_DESTINATION)
               NUC970_INVALIDATE( DESTINATION );
          if (modified & SMF_COLOR)
               NUC970_INVALIDATE( COLOR_DRAW | COLOR_BLIT );

          if (modified & SMF_DRAWING_FLAGS)
               NUC970_INVALIDATE( COLOR_DRAW );

          if (modified & SMF_BLITTING_FLAGS)
               NUC970_INVALIDATE( COLOR_BLIT );
     }

     /*
      * 2) Validate hardware states
      *
      * Each function has its own set of states that need to be validated.
      */

     /* Always requiring valid destination, clip, matrix and options... */
     NUC970_CHECK_VALIDATE( DESTINATION );

     /* Depending on the function... */
     switch (accel) {
          case DFXL_FILLRECTANGLE:
          case DFXL_DRAWRECTANGLE:
          case DFXL_DRAWLINE:
               /* ...require valid drawing color. */
               NUC970_CHECK_VALIDATE( COLOR_DRAW );

               /*
                * 3) Tell which functions can be called without further validation, i.e. SetState()
                *
                * When the hw independent state is changed, this collection is reset.
                */
               state->set = NUC970_SUPPORTED_DRAWINGFUNCTIONS;
               break;

          case DFXL_BLIT:
          case DFXL_STRETCHBLIT:

               /* ...require valid source. */
               NUC970_CHECK_VALIDATE( SOURCE );
          
               /* If colorizing or premultiplication of global alpha is used... */
               if (state->blittingflags & (DSBLIT_COLORIZE | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_BLEND_COLORALPHA)) {
                    /* ...require valid color. */
                    NUC970_CHECK_VALIDATE( COLOR_BLIT );

               }

               /*
                * 3) Tell which functions can be called without further validation, i.e. SetState()
                *
                * When the hw independent state is changed, this collection is reset.
                */
               state->set = NUC970_SUPPORTED_BLITTINGFUNCTIONS;
               break;

          default:
               D_BUG( "unexpected drawing/blitting function" );
               break;
     }

     ndrv->blittingflags = state->blittingflags;

     /*
      * 4) Clear modification flags
      *
      * All flags have been evaluated in 1) and remembered for further validation.
      * If the hw independent state is not modified, this function won't get called
      * for subsequent rendering functions, unless they aren't defined by 3).
      */
     state->mod_hw = 0;
}

/**********************************************************************************************************************/

/*
 * Render a filled rectangle using the current hardware state.
 */
bool
nuc970FillRectangle( void *drv, void *dev, DFBRectangle *rect )
{
    NUC970DriverData           *ndrv     = drv;
    
    ndrv->params.dst_start_x = rect->x;
    ndrv->params.dst_start_y = rect->y;
    ndrv->params.dst_work_width =  rect->w;
    ndrv->params.dst_work_height =  rect->h;
	
	ioctl(ndrv->ge2d_fd, NUC970_GE2D_FILL_RECTANGLE, &ndrv->params); 

    return true;
}

/*
 * Render a line using the current hardware state.
 */
bool
nuc970DrawLine( void *drv, void *dev, DFBRegion *line )
{
    NUC970DriverData           *ndrv     = drv;

    ndrv->params.line_x1 = line->x1;
    ndrv->params.line_y1 = line->y1;

    ndrv->params.line_x2 = line->x2;
    ndrv->params.line_y2 = line->y2;

    ioctl(ndrv->ge2d_fd, NUC970_GE2D_LINE, &ndrv->params);

    return true;
}

/*
 * Blit a rectangle using the current hardware state.
 */
bool
nuc970Blit( void *drv, void *dev, DFBRectangle *srect, int dx, int dy )
{
    NUC970DriverData           *ndrv     = drv;
    
    ndrv->params.src_start_x = srect->x;
    ndrv->params.src_start_y = srect->y;
    ndrv->params.src_work_width = srect->w;
    ndrv->params.src_work_height = srect->h;

    ndrv->params.dst_start_x = dx;
    ndrv->params.dst_start_y = dy;

    ioctl(ndrv->ge2d_fd, NUC970_GE2D_START_BITBLT, &ndrv->params);

    return true;
}

