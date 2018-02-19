#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct _IMAGE_BUF
{
	int             width;
	int             height;
	int             Total;
	unsigned char*  buf;
}IMAGE_BUF;


int clamp(int val) {
	if (val>255) return 255;
	if (val<0) return 0;
	return val;
}


int main()
{
	FILE        *f_img, *f_out;
	FILE        *f_y, *f_u, *f_v, *f_buff, *f_RGB;
	int         i, j;
	IMAGE_BUF   y, u, v, buff, RGB;
	int         frm, numfrm;
	int         face_num;
	FILE        *f_outy, *f_outu, *f_outv;
	int         width, height;

	width = 4656;
	height = 3492;

	f_img = fopen("C:/Photo/YUV/nr_inp/0_15_48_24_hdr_input_4656x3492_0_0_0_-4_-8_0.yuv", "rb");
	f_RGB = fopen("C:/Photo/YUV/nr_inp/RGB_416x240_30.rgb", "wb");
	f_buff = fopen("C:/Photo/YUV/nr_inp/buff_416x240_30.yuv", "wb");

	fseek(f_img, 0, SEEK_END);
	numfrm = ftell(f_img);
	numfrm /= ((width * height * 3) >> 1);
	fseek(f_img, 0, SEEK_SET);

	buff.buf = (unsigned char*)malloc(width * height *1.5*sizeof(unsigned  char));
	RGB.buf = (unsigned char*)malloc(width * height *3*sizeof(unsigned char));

	for (frm = 0; frm < numfrm; frm++)
	{
		fread(buff.buf, sizeof(unsigned char), (width * height)*1.5, f_img);

		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				unsigned int y = buff.buf[j*width + i];
				unsigned int u = 128;// buff.buf[(j / 2)*(width / 2) + (i / 2) + (width*height)];
				unsigned int v = 128; //buff.buf[(j / 2)*(width / 2) + (i / 2) + (width*height) + 1]; // +((width*height) / 4)];

				//convert to RGB
				unsigned int R = clamp(y + (1.370705 * (v - 128)));
				unsigned int G = clamp(y - (0.698001 * (v - 128)) - (0.337633 * (u - 128)));
				unsigned int B = clamp(y + (1.732446 * (u - 128)));
				unsigned int RGBval = (0xFF << 24) | ((unsigned int)R << 16) | ((unsigned int)G << 8) | B;
				//RGB.buf[j*width + i] = RGBval;
				RGB.buf[j*width + i] = (unsigned char)clamp(y + (1.370705 * (v - 128)));
				RGB.buf[height*width + j*width + i] = (unsigned char)clamp(y - (0.698001 * (v - 128)) - (0.337633 * (u - 128)));
				RGB.buf[height*width*2 + j*width + i] = (unsigned char)clamp(y + (1.732446 * (u - 128)));
				//RGB.buf[j*width + i] = R;
				//RGB.buf[j*width + i + 1] = G;
				//RGB.buf[j*width + i + 2] = B; 
			}
		}
		fwrite(buff.buf, sizeof(unsigned char), (width * height)*1.5, f_buff);
		//fwrite(RGB.buf, sizeof(unsigned char), (width * height), f_RGB);
	}

	std::string img_path = "C:/Photo/YUV/nr_inp/RGB_416x240_30.png";

	std::remove(img_path.c_str());

	int stride_in_bytes = 3*width;

	if (!stbi_write_png(img_path.c_str(), width, height, 3, RGB.buf, stride_in_bytes)) {

		std::cerr << "Unable to write output image " << std::endl;
		return false;
	}

	fclose(f_buff);
	fclose(f_RGB);

	free(buff.buf);
	free(RGB.buf);

	fclose(f_img);
	return 0;
}