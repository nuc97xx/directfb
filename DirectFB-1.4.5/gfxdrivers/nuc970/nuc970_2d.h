
#ifndef __NUC970_2D_H__
#define __NUC970_2D_H__

#if 0
#define NUC970_SUPPORTED_DRAWINGFLAGS      (DSDRAW_BLEND | \
                                        DSDRAW_SRC_PREMULTIPLY)
#else
#define NUC970_SUPPORTED_DRAWINGFLAGS      DSDRAW_NOFX
#endif

/*#define NUC970_SUPPORTED_DRAWINGFUNCTIONS  (DFXL_FILLRECTANGLE | \
                                        DFXL_DRAWLINE)*/

#define NUC970_SUPPORTED_DRAWINGFUNCTIONS  DFXL_FILLRECTANGLE
                                                                           
#if 0
#define NUC970_SUPPORTED_BLITTINGFLAGS     (DSBLIT_BLEND_ALPHACHANNEL | \
                                        DSBLIT_BLEND_COLORALPHA   | \
                                        DSBLIT_COLORIZE           | \
                                        DSBLIT_ROTATE180          | \
                                        DSBLIT_SRC_PREMULTCOLOR)
#else
#define NUC970_SUPPORTED_BLITTINGFLAGS   DSBLIT_COLORIZE
//#define NUC970_SUPPORTED_BLITTINGFLAGS   DSDRAW_NOFX
#endif

#define NUC970_SUPPORTED_BLITTINGFUNCTIONS (DFXL_BLIT)


DFBResult nuc970EngineSync   ( void                *drv,
                           void                *dev );

void      nuc970EngineReset  ( void                *drv,
                           void                *dev );

void      nuc970EmitCommands ( void                *drv,
                           void                *dev );

void      nuc970CheckState   ( void                *drv,
                           void                *dev,
                           CardState           *state,
                           DFBAccelerationMask  accel );

void      nuc970SetState     ( void                *drv,
                           void                *dev,
                           GraphicsDeviceFuncs *funcs,
                           CardState           *state,
                           DFBAccelerationMask  accel );

bool      nuc970FillRectangle( void                *drv,
                           void                *dev,
                           DFBRectangle        *rect );
/*
bool      nuc970DrawRectangle( void                *drv,
                           void                *dev,
                           DFBRectangle        *rect );
*/
bool      nuc970DrawLine     ( void                *drv,
                           void                *dev,
                           DFBRegion           *line );
/*
bool      nuc970FillTriangle ( void                *drv,
                           void                *dev,
                           DFBTriangle         *tri );
*/
bool      nuc970Blit         ( void                *drv,
                           void                *dev,
                           DFBRectangle        *srect,
                           int                  dx,
                           int                  dy );

/*bool      nuc970StretchBlit  ( void                *drv,
                           void                *dev,
                           DFBRectangle        *srect,
                           DFBRectangle        *drect );
*/
#endif

