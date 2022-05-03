
/**

// This main function is meant only to illustrate how to use the save_imageXX functions.
// You should get rid of this code, and just paste the save_imageXX functions into your
// raytrace.cpp code. 
int main() {
	int Width = 128;	// Move these to your setup function. The actual resolution will be
	int Height= 128;	// specified in the input file
    char fname3[20] = "sceneP3.ppm"; //This should be set based on the input file
	char fname6[20] = "sceneP6.ppm"; //This should be set based on the input file
	unsigned char *pixels;
	// This will be your image. Note that pixels[0] is the top left of the image and
	// pixels[3*Width*Height-1] is the bottom right of the image.
    pixels = new unsigned char [3*Width*Height];

	// This loop just creates a gradient for illustration purposes only. You will not use it.
	float scale = 128.0 / (float) Width ;
	int k = 0 ;
	for(int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			int c = (i+j)*scale ;
			pixels[k] = c;
			pixels[k+1] = c;
			pixels[k+2] = c;
			k = k + 3 ;
		}
	}
	save_imageP3(Width, Height, fname3, pixels);
	save_imageP6(Width, Height, fname6, pixels);
}

**/
