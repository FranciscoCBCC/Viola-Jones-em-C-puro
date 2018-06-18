#include <stdio.h>
#include <stdlib.h>
#include "bitmapHeader.h"
//Feito com o auxilio do site: https://stackoverflow.com/questions/19624159/strange-values-when-reading-pixels-from-24-bit-bitmap

int main(){
	
	BITMAP imagemBitmap;
	FILE *imagemEntrada, *imagemSaida;
	unsigned int ImagePixelAmount = 0;
	
	imagemEntrada = fopen("image.bmp", "rb");
	fseek(imagemEntrada, 0, SEEK_SET);	
	fread(&imagemBitmap.FILEHEADER, sizeof(BITMAPFILEHEADER), 1, imagemEntrada); //FILE READER
	
	/*
	printf("<<Bitmap File Header Data Values>> \n");
	printf("Tamanho imagem:  %d\n", imagemBitmap.FILEHEADER.bfSize);
	printf("Byte Offset:  %d\n", imagemBitmap.FILEHEADER.bfOffBits);
	*/
	
	fread(&imagemBitmap.INFOHEADER, sizeof(BITMAPINFOHEADER), 1, imagemEntrada); //INFO READER
	
	/*
	printf("\n<<Bitmap Info Header Data Values>>\n");
	printf("Size of Bitmap Info Header: %d\n", imagemBitmap.INFOHEADER.biSize);
	printf("Width of Bitmap: %d\n", imagemBitmap.INFOHEADER.biWidth);
	printf("Height of Bitmap: %d\n", imagemBitmap.INFOHEADER.biHeight);
	printf("Bit Count: %d\n", imagemBitmap.INFOHEADER.biBitCount);
	printf("Amount of color indexes: %d\n", imagemBitmap.INFOHEADER.biClrUsed);   
	*/ 
	
	ImagePixelAmount = imagemBitmap.INFOHEADER.biHeight * imagemBitmap.INFOHEADER.biWidth; 
	
	//Create space in memory and read in pixel data from the image
	imagemBitmap.IMAGEDATA = (rgbPIXEL*)malloc(sizeof(rgbPIXEL) * ImagePixelAmount);	
	fseek(imagemEntrada, imagemBitmap.FILEHEADER.bfOffBits, SEEK_SET);
	fread(imagemBitmap.IMAGEDATA, ImagePixelAmount * sizeof(rgbPIXEL), 1, imagemEntrada);
	
	//Conver para escala de cinza, 0.3*R +0.59*G +0.11*B
	for(int y = 0; y < imagemBitmap.INFOHEADER.biHeight; y++){
    	for(int x = 0; x < imagemBitmap.INFOHEADER.biWidth; x++){
			char buffer;
			buffer = (0.21*(imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].r)+ //Vermelho
					 0.72*(imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].g)+ //Verde
					 0.07*(imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].b)); //Azul
			imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].r = buffer;
			imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].g = buffer;
			imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].b = buffer;
    	}
	}
	
	
	//Calcula a Imagem Integral
	char *imagemIntegralR, *imagemIntegralG, *imagemIntegralB;
	char valorVetorR, valorVetorG, valorVetorB;
    imagemIntegralR = (char *) malloc(sizeof(char)*imagemBitmap.INFOHEADER.biHeight*imagemBitmap.INFOHEADER.biWidth);
    imagemIntegralG = (char *) malloc(sizeof(char)*imagemBitmap.INFOHEADER.biHeight*imagemBitmap.INFOHEADER.biWidth);
    imagemIntegralB = (char *) malloc(sizeof(char)*imagemBitmap.INFOHEADER.biHeight*imagemBitmap.INFOHEADER.biWidth);    
    
	for(int y = 0; y < imagemBitmap.INFOHEADER.biHeight; y++){
    	for(int x = 0; x < imagemBitmap.INFOHEADER.biWidth; x++){
    		valorVetorR = imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].r;
    		valorVetorG = imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].g;
    		valorVetorB = imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].b;
    		
    		if(y>0){
    			valorVetorR += imagemIntegralR[(y-1)*imagemBitmap.INFOHEADER.biWidth+x];
    			valorVetorG += imagemIntegralG[(y-1)*imagemBitmap.INFOHEADER.biWidth+x];
    			valorVetorB += imagemIntegralB[(y-1)*imagemBitmap.INFOHEADER.biWidth+x];
			}
			
			if(x>0){
				valorVetorR += imagemIntegralR[y*imagemBitmap.INFOHEADER.biWidth+x-1];
    			valorVetorG += imagemIntegralG[y*imagemBitmap.INFOHEADER.biWidth+x-1];
    			valorVetorB += imagemIntegralB[y*imagemBitmap.INFOHEADER.biWidth+x-1];
			}
			
			if(x>0 && y>0){
				valorVetorR -= imagemIntegralR[(y-1)*imagemBitmap.INFOHEADER.biWidth+x-1];
    			valorVetorG -= imagemIntegralG[(y-1)*imagemBitmap.INFOHEADER.biWidth+x-1];
    			valorVetorB -= imagemIntegralB[(y-1)*imagemBitmap.INFOHEADER.biWidth+x-1];
			}
			
			imagemIntegralR[y*imagemBitmap.INFOHEADER.biWidth+x] = valorVetorR;
			imagemIntegralG[y*imagemBitmap.INFOHEADER.biWidth+x] = valorVetorG;
			imagemIntegralB[y*imagemBitmap.INFOHEADER.biWidth+x] = valorVetorB;
    		
    	}
	}
	
	//Salva o valor da imagem integral no arquivo bitmap
	for(int y = 0; y < imagemBitmap.INFOHEADER.biHeight; y++){
    	for(int x = 0; x < imagemBitmap.INFOHEADER.biWidth; x++){
    		imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].r = imagemIntegralR[y*imagemBitmap.INFOHEADER.biWidth+x];
			imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].g = imagemIntegralG[y*imagemBitmap.INFOHEADER.biWidth+x];
			imagemBitmap.IMAGEDATA[y * imagemBitmap.INFOHEADER.biWidth + x].b = imagemIntegralB[y*imagemBitmap.INFOHEADER.biWidth+x];
    	}
    }
	
	//Imagem de saida
	imagemSaida = fopen("gray.bmp", "wb");
	fseek(imagemSaida, 0, SEEK_SET);	
	fwrite(&imagemBitmap.FILEHEADER, sizeof(BITMAPFILEHEADER), 1, imagemSaida);
	fwrite(&imagemBitmap.INFOHEADER, sizeof(BITMAPINFOHEADER), 1, imagemSaida);
	fseek(imagemSaida, imagemBitmap.FILEHEADER.bfOffBits, SEEK_SET);
	fwrite(imagemBitmap.IMAGEDATA, ImagePixelAmount * sizeof(rgbPIXEL), 1, imagemSaida);
	
	fclose(imagemEntrada);
	fclose(imagemSaida);
	return 0;
}
