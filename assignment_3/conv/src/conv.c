#define PNGSUITE_PRIMARY
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_DEFINE
#include "stb.h"

//#define PNGSUITE_PRIMARY

void calc_convolution (char *srcfile, char *outfile, int _kernel);
void kernel_left_sobel (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);
void kernel_identity (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);
void kernel_outline (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);
void kernel_blur (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);
void kernel_sharpen (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);
void kernel_topsobel (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]);

int main(int argc, char **argv)
{

	int c;
	char *src_image = NULL;
	char *out_image = NULL;
	int kernel = -1;

//This while is used to read the arguments added to through the command line to the application
	while ((c = getopt (argc, argv, "ha:i:o:k:")) != -1){
/*
The following switch will identify the arguments read, and it will call the proper method
and set the proper variables in order to execute the required methods
*/

		switch (c) {
			case 'a':
			//'a' means the autor information will be printed
			//	print_autor();
				break;
			case 'i':
			//'b' It will have the path for the binary file to be executed
				src_image = optarg;
				break;			
			case 'h':
			//'h' is used to print the help menu
			//	print_help();
				break;
			case 'o':
			//'w' has the address which will be monitored during the test execution
				out_image = optarg;
				break;
			case 'k':
			//'s' will execute the binary file step by step
				kernel = atoi(optarg);
				break;

			case '?':
				if (optopt == 'c')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				        return 1;
			default:
			//In case the argument read is not supported
				printf("Option '-%i' not recognized.\n",c);
			}
	}

	/*
	-i argument is required for the program to apply the kernel to the src image. 
	-o argument is required for the program to store the result from the convolution between the src image and the kernel selected.
	-k argument is required for the program to know the kernel to apply. 
	If one of those arguments is not added, the program will fail due to there is not a required argument to have the result succesfully. 
	The following "if" will check if those argument were added in the command line, and if one of the 
	argument was not added, the program will fail.	
	*/

	if(src_image==NULL){
		fprintf(stderr, "-Error- Argument -i <source_image> is required\n");
		exit(0);
	}else{
		fprintf(stderr, "-Info- Convolution program will use the image %s as the source image\n", src_image);
	}

	if(out_image==NULL){
		fprintf(stderr, "-Error- Argument -o <output_image> is required\n");
		exit(0);
	}else{
		fprintf(stderr, "-Info- Convolution program will store the image resultant in the file %s\n", out_image);
	}

	if(kernel==-1){
		fprintf(stderr, "-Error- Argument -k <kernel desired> is required\n");
		exit(0);
	}else{
		fprintf(stderr, "-Info- Convolution program will apply the kernel number %i to the src image\n", kernel);
	}

	calc_convolution(src_image, out_image, kernel);

	return 0;

}

void calc_convolution (char *srcfile, char *outfile, int _kernel){

	uint8 *org_image, *result;
	int *x; int *y; int req_comp;
	int w,h, depth;
	org_image = stbi_load(srcfile, &w, &h, &depth, req_comp);
	result = stbi_load(srcfile, &w, &h, &depth, req_comp);
	
	uint8 c_image[h][w][3];

	printf("------- Image Information -----------\n");
	printf("-I- Src Image width     %i\n", w);	
	printf("-I- Src Image height    %i\n", h);	
	printf("-I- Src Image depth     %i\n", depth);		

	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			for (int z = 0; z < depth; z++) {
				c_image[row][col][z] = org_image[(row * w + col) * depth + z];
				result[(row * w + col) * depth + z] = -1;
			}
		}
	}

	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			for (int z = 0; z < depth; z++) {

				switch (_kernel) {
					case 1:
						kernel_left_sobel(result, h, row, w, col, depth, z, c_image);	
					break;
					
					case 2:
						kernel_identity(result, h, row, w, col, depth, z, c_image);							
					break;			
				
					case 3:
						kernel_outline(result, h, row, w, col, depth, z, c_image);		
					break;
	
					case 4:
						kernel_blur(result, h, row, w, col, depth, z, c_image);		
					break;

					case 5:
						kernel_sharpen(result, h, row, w, col, depth, z, c_image);		
					break;

					case 6:
						kernel_topsobel(result, h, row, w, col, depth, z, c_image);		
					break;	
					default:
						printf("-Error- Kernel %i is not found, exiting from the program ....\n", _kernel);
						exit(0);
				}
			}
		}
	}
	
//	stbi_write_png("algo.png",w,h,comp,pixels, w);
//	stbi_write_bmp("algo.bmp",w,h,depth,org_image);
	stbi_write_bmp(outfile,w,h,depth,result);
}

void kernel_left_sobel (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]){

	_proc_image[(row * w + col) * depth + z] = (
						c_image[row-1][col-1][z]*1 + c_image[row-1][col][z]*0 + c_image[row-1][col+1][z]*-1 + 
						c_image[row]  [col-1][z]*2 + c_image[row]  [col][z]*0  + c_image[row]  [col+1][z]*-2 + 
						c_image[row+1][col-1][z]*1 + c_image[row+1][col][z]*0 + c_image[row+1][col+1][z]*-1);
}

void kernel_identity (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]){

	_proc_image[(row * w + col) * depth + z] = (
						c_image[row-1][col-1][z]*0 + c_image[row-1][col][z]*0 + c_image[row-1][col+1][z]*0 + 
						c_image[row]  [col-1][z]*0 + c_image[row]  [col][z]*1  + c_image[row]  [col+1][z]*0 + 
						c_image[row+1][col-1][z]*0 + c_image[row+1][col][z]*0 + c_image[row+1][col+1][z]*0);
}

void kernel_outline (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]){
	_proc_image[(row * w + col) * depth + z] = (
						c_image[row-1][col-1][z]*-1 + c_image[row-1][col][z]*-1 + c_image[row-1][col+1][z]*-1 + 
						c_image[row]  [col-1][z]*-1 + c_image[row]  [col][z]*8  + c_image[row]  [col+1][z]*-1 + 
						c_image[row+1][col-1][z]*-1 + c_image[row+1][col][z]*-1 + c_image[row+1][col+1][z]*-1);
}

void kernel_blur (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]){
	_proc_image[(row * w + col) * depth + z] = (
						c_image[row-1][col-1][z]*0.0625 + c_image[row-1][col][z]*0.125 + c_image[row-1][col+1][z]*0.0625 + 
						c_image[row]  [col-1][z]*0.125 + c_image[row]  [col][z]*0.25  + c_image[row]  [col+1][z]*0.125 + 
						c_image[row+1][col-1][z]*0.0625 + c_image[row+1][col][z]*0.125 + c_image[row+1][col+1][z]*0.0625);
}

void kernel_sharpen (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]){
	_proc_image[(row * w + col) * depth + z] = (
						c_image[row-1][col-1][z]*0 + c_image[row-1][col][z]*-1 + c_image[row-1][col+1][z]*0 + 
						c_image[row]  [col-1][z]*-1 + c_image[row]  [col][z]*5  + c_image[row]  [col+1][z]*-1 + 
						c_image[row+1][col-1][z]*0 + c_image[row+1][col][z]*-1 + c_image[row+1][col+1][z]*0);
}

void kernel_topsobel (uint8 *_proc_image, int h, int row, int w, int col, int depth, int z, uint8 c_image[h][w][depth]){
	_proc_image[(row * w + col) * depth + z] = (
						c_image[row-1][col-1][z]*1 + c_image[row-1][col][z]*2 + c_image[row-1][col+1][z]*1 + 
						c_image[row]  [col-1][z]*0 + c_image[row]  [col][z]*0  + c_image[row]  [col+1][z]*0 + 
						c_image[row+1][col-1][z]*-1 + c_image[row+1][col][z]*-2 + c_image[row+1][col+1][z]*-1);
}
