/**
 * @file documentation.h
 * @authors Marco Espinoza/
 *			Jose Campos/
 *			Freddy Zeledon
 * @brief Assignment 3: Unus pro omnibus, omnes pro uno! 
 * @section USAGE
 * This program performs a selected kernel operation using openMP to show how parallelism increases performance in multiprocessor architectures. 
 * 
 * Usage: 
 *
 *		conv -i <source_image> -o <output_image> -k <kernel_id> 
 *
 *		-i source_image  	specify the input image to apply the kernel
 *
 *		-o dest_image  		specify the resulting image	
 *		-k kernel_id		specify the kernel id	
 * 							Where kernel id is:
 *								1. Left Sobel
 *								2. Identity
 *								3. Outline
 *								4. Blur
 *								5. Sharpen
 *								6. Top Sobel
 *		-h 					print help 
 *		-a 					print author information
 *
 * @subsection Kernel Kernel Description
 *  
 *		1. Left Sobel: 
 *		2. Identity:
 *		3. Outline:
 *		4. Blur:
 *		5. Sharpen:
 *		6. Top Sobel
 *
 * @subsection Example Example 
 * Example input image:
 * @image html lena512.bmp
 *
 * Example output image:
 * @image html lena_my.bmp
 */

/**
 * @brief run_kernel is used to perform the selected kernel operation
 * @param srcfile Source image
 * @param outfile Output image
 * @param _kernel Integer value with the kernel chosen:
 *                	1. kernel_left_sobel
 *					2. kernel_identity
 *					3. kernel_outline
 *					4. kernel_blur
 *					5. kernel_sharpen
 *					6. kernel_topsobel
 */
void run_kernel (char *srcfile, char *outfile, int _kernel);

/**
 * @brief Kernel_left_sobel is used to perform the left_sobel kernel operation in the input image, 
 *        It consists in the result of blurring an image by a fraction function.

 * @param _proc_image Processing image
 * @param h matrix height 
 * @param row actual row
 * @param w   matrix width
 * @param col actual column
 * @param depth actual depth
 * @param z   matrix depth 
 * @param c_image output image
 */
void kernel_left_sobel (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);
/**
 * @brief kernel_identity 
 *        Identity is a function that always returns the same value that was used as its argument, in this case
 *			it consist to convulate the original image by an identity matrix.
 * @param _proc_image Processing image
 * @param h Output image
 * @param row actual row
 * @param w   matrix width
 * @param col actual column
 * @param depth actual depth
 * @param z   matrix depth 
 * @param c_image output image
 */

void kernel_identity (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);

/**
 * @brief kernel_outline 
 *			An outline kernel (also called an "edge" kernel) is used to highlight large differences in pixel values. 
 *			A pixel next to neighbor pixels with close to the same intensity will appear black in the new image while 
 *			one next to neighbor pixels that differ strongly will appear white.
 * @param _proc_image Processing image
 * @param h Output image
 * @param row actual row
 * @param w   matrix width
 * @param col actual column
 * @param depth actual depth
 * @param z   matrix depth 
 * @param c_image output image
 */


void kernel_outline (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);

/**
 * @brief kernel_blur:
 *                   The blur kernel de-emphasizes differences in adjacent pixel values.
 * @param _proc_image Processing image
 * @param h Output image
 * @param row actual row
 * @param w   matrix width
 * @param col actual column
 * @param depth actual depth
 * @param z   matrix depth 
 * @param c_image output image
 */


void kernel_blur (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);
/**
 * @brief kernel_sharpen:
 *                    The sharpen kernel emphasizes differences in adjacent pixel values. This makes the image look more vivid.
 * @param _proc_image Processing image
 * @param h Output image
 * @param row actual row
 * @param w   matrix width
 * @param col actual column
 * @param depth actual depth
 * @param z   matrix depth 
 * @param c_image output image
 */


void kernel_sharpen (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);

/**
 * @brief kernel_topsobel:

 *                     The sobel kernels are used to show only the differences in adjacent pixel values in a particular direction.
 * @param _proc_image Processing image
 * @param h Output image
 * @param row actual row
 * @param w   matrix width
 * @param col actual column
 * @param depth actual depth
 * @param z   matrix depth 
 * @param c_image output image
 */

void kernel_topsobel (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);


