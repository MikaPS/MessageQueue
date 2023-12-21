#pragma once

#include "EasyBMP.h"

class FractalMessage {

	// a message which contains all of the state needed to render a "tile" in the image
	// since the memory this message will write to is independent and in a different
	// are from that written by any other image, this can be done safely

	protected:

		// the image we will output to
		EasyBMP::Image * output_image;

		// the left, top, and size of the fractal region
 		float left, top, xsize, ysize;
		
		// the window in the output image we are outputing to
		size_t min_x, min_y;
	        size_t max_x, max_y;	

		size_t image_x;
		size_t image_y;


	public:

		FractalMessage(EasyBMP::Image * out_im, float l, float t, float xs, float ys, size_t mn_x, size_t mn_y, size_t mx_x, size_t mx_y, size_t i_x, size_t i_y) {
			
			output_image = out_im;

			left = l; top = t;
		       	
			xsize = xs;
	       	ysize = ys;
			
			min_x = mn_x; min_y = mn_y;
	  		max_x = mx_x; max_y = mx_y;
			
			image_x = i_x;
			image_y = i_y;		
		}

		EasyBMP::Image * get_output_image() { return output_image; }

		float get_left() { return left; }
		float get_top() { return top; }
		
		float get_x_size() { return xsize; }
		float get_y_size() { return ysize; }
		
		size_t get_min_x() { return min_x; }
		size_t get_min_y() { return min_y; }
		size_t get_max_x() { return max_x; }
		size_t get_max_y() { return max_y; }

		size_t get_image_x() { return image_x; }
		size_t get_image_y() { return image_y; }

};
