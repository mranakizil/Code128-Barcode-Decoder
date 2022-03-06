#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

int decode(unsigned char* img, int scan_line_no, char* text);

void error(int16 bitsPerPixel, int32 width, int32 height){

        if(bitsPerPixel != 24 || width != 600 || height != 50){
                printf("BMP file is not given in the required format!\n");
        }
}

//***Inputs*****
//fileName: The name of the file to open 
//***Outputs****
//pixels: A pointer to a byte array. This will contain the pixel data
//width: An int pointer to store the width of the image in pixels
//height: An int pointer to store the height of the image in pixels
//bytesPerPixel: An int pointer to store the number of bytes per pixel that are used in the image
void ReadImage(const char *fileName, byte **pixels, int32 *width, int32 *height, int32 *bytesPerPixel) {
        //Open the file for reading in binary mode
        FILE *imageFile = fopen(fileName, "rb");
        //Read data offset
        int32 dataOffset;
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
        fread(&dataOffset, 4, 1, imageFile);
        //Read width
        fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
        fread(width, 4, 1, imageFile);
        //Read height
        fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
        fread(height, 4, 1, imageFile);
        //Read bits per pixel
        int16 bitsPerPixel;
        fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
        fread(&bitsPerPixel, 2, 1, imageFile);
        //Allocate a pixel array
        *bytesPerPixel = ((int32)bitsPerPixel) / 8;

        error(bitsPerPixel, *width, *height);

        //Rows are stored bottom-up
        //Each row is padded to be a multiple of 4 bytes. 
        //We calculate the padded row size in bytes
        int paddedRowSize = (int)(4 * ceil((float)(*width) / 4.0f))*(*bytesPerPixel);
        //We are not interested in the padded bytes, so we allocate memory just for
        //the pixel data
        int unpaddedRowSize = (*width)*(*bytesPerPixel);
        //Total size of the pixel data in bytes
        int totalSize = unpaddedRowSize*(*height);
        *pixels = (byte*)malloc(totalSize);
        //Read the pixel data Row by Row.
        //Data is padded and stored bottom-up
        int i = 0;
        //point to the last row of our pixel array (unpadded)
        byte *currentRowPointer = *pixels+((*height-1)*unpaddedRowSize);
        for (i = 0; i < *height; i++)
        {
                //put file cursor in the next row from top to bottom
	        fseek(imageFile, dataOffset+(i*paddedRowSize), SEEK_SET);
	        //read only unpaddedRowSize bytes (we can ignore the padding bytes)
	        fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
	        //point to the next row (from bottom to top)
	        currentRowPointer -= unpaddedRowSize;
                int currentRow = (int)(*currentRowPointer);
                
        }
        int result;

        char output[100];
            result = decode(*pixels + dataOffset, 12, output);
            if(result == 0) {
                printf("Decoded text: %s\n", output);
            }

        fclose(imageFile);
}


int main()
{
        int i=0;
        byte *pixels;
        int32 width;
        int32 height;
        int32 bytesPerPixel;
        ReadImage("abcd.bmp", &pixels, &width, &height,&bytesPerPixel);
        free(pixels);
        return 0;
}
