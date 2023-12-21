#pragma once

#include "EasyBMP.h"

#include <iostream>

#define MAXCOUNT 30

void fractal(EasyBMP::Image * output_image, float frac_min_x, float frac_min_y, float frac_max_x, float frac_max_y, 
		                            size_t img_min_x, size_t img_min_y, size_t img_max_x, size_t img_max_y, 
					    size_t image_size_x, size_t image_size_y)
{


    // the extent of the image tile
    size_t img_tile_extent_x = img_max_x - img_min_x;
    size_t img_tile_extent_y = img_max_y - img_min_y;
  
    // the exent of the fractal tile 
    float frac_tile_extent_x = frac_max_x - frac_min_x;
    float frac_tile_extent_y = frac_max_y - frac_min_y;

    float sx = (float) frac_tile_extent_x / (float) img_tile_extent_x;
    float sy = (float) frac_tile_extent_y / (float) img_tile_extent_y;

    // std::cout << "xscale = " << sx << " yscale = " << sx << std::endl; 

    // scan though every point in that rectangular area.
    // Each point represents a Complex number (x + yi).
    // Iterate that complex number
    
    float zx, zy, cx, tempx, cy;
    int count;
    
    for (int y = 0; y < img_tile_extent_x; y++) {
        for (int x = 0; x < img_tile_extent_y; x++)
        {
            // c_real
            cx = (x * sx) + frac_min_x;
  
            // c_imaginary
            cy = (y * sy) + frac_min_y;
  
            // z_real
            zx = 0;
  
            // z_imaginary
            zy = 0;
            count = 0;
  
            // Calculate whether c(c_real + c_imaginary) belongs
            // to the Mandelbrot set or not and draw a pixel
            // at coordinates (x, y) accordingly
	    //
            // If you reach the Maximum number of iterations
            // and If the distance from the origin is
            // greater than 2 exit the loop
            
	    while ((zx * zx + zy * zy < 4) && (count < MAXCOUNT)) {

                // Calculate Mandelbrot function
                // z = z*z + c where z is a complex number
  
                // tempx = z_real*_real - z_imaginary*z_imaginary + c_real
                tempx = zx * zx - zy * zy + cx;
  
                // 2*z_real*z_imaginary + c_imaginary
                zy = 2 * zx * zy + cy;
  
                // Updating z_real = tempx
                zx = tempx;
  
                // Increment count
                count = count + 1;
            }

	    int c = 255 * ((float) count / (float) MAXCOUNT);
	    EasyBMP::RGBColor color(c, c, c);

            // output the pixel to the output image 
            output_image->SetPixel(img_min_x + x, img_min_y + y, color);
        }
    }
}
