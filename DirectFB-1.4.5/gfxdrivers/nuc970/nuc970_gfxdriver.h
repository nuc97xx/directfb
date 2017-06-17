
#ifndef __NUC970_GFXDRIVER_H__
#define __NUC970_GFXDRIVER_H__

typedef struct
{
    unsigned int	src_base_addr;			//Base address of the source image
	unsigned int	src_full_width;			//source image full width
	unsigned int	src_full_height;		//source image full height
	unsigned int	src_start_x;			//coordinate start x of source image
	unsigned int	src_start_y;			//coordinate start y of source image
	unsigned int	src_work_width;			//source image width for work
	unsigned int    src_work_height;		//source image height for work
    unsigned int    src_colormode;

	unsigned int	dst_base_addr;			//Base address of the destination image	
	unsigned int	dst_full_width;			//destination screen full width
	unsigned int	dst_full_height;		//destination screen full width
	unsigned int	dst_start_x;			//coordinate start x of destination screen
	unsigned int	dst_start_y;			//coordinate start y of destination screen
	unsigned int	dst_work_width;			//destination screen width for work
	unsigned int    dst_work_height;		//destination screen height for work
    unsigned int    dst_colormode;

	// Coordinate (X, Y) of clipping window
	unsigned int    cw_x1, cw_y1;
	unsigned int    cw_x2, cw_y2;
    
    // Line address
    unsigned int    line_x1, line_y1;
	unsigned int    line_x2, line_y2;
    
	unsigned char   color_val[8];

	// Alpha blending
    unsigned int	alpha_mode;			//true : enable, false : disable
	unsigned int	alpha_val;
    
    // Transparent
	unsigned int	color_key_mode;			//true : enable, false : disable
	unsigned int	color_key_val;			//transparent color value    
    
	unsigned char   bpp_src;
    unsigned char   rop;        // rop code
    unsigned char   rotate;     // rotate option
    
    // Scale up/down
    unsigned char   scale_mode;
    unsigned int    scale_vfn, scale_vfm, scale_hfn, scale_hfm;
}nuc970_g2d_params;

typedef struct {
    /* validation flags */
    int                      v_flags;

    /** Add shared data here... **/
} NUC970DeviceData;


typedef struct {
    DFBSurfaceBlittingFlags  blittingflags;

    int ge2d_fd;

    nuc970_g2d_params params;
    
    /** Add local data here... **/

} NUC970DriverData;


#endif
