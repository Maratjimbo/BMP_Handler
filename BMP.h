#ifndef CLIONPROJECTS_BMP_H
#define CLIONPROJECTS_BMP_H

#include "HeaderBMP.h"

class BMP {
private:
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    std::vector<std::vector<RGBTRIPLE>> rgb;
    std::vector<BYTE> trash;
public:
    BMP();

    BMP(BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, std::vector<std::vector<RGBTRIPLE>> rgb);

    BITMAPINFOHEADER getInfoHeader();

    BITMAPFILEHEADER getFileHeader();

    std::vector<std::vector<RGBTRIPLE>> getRGB();

    void onlyOneColor();//3

    void correlation();//4

    void RGBtoYCbCr();//5,6

    void YCbCrToRGB();//7

    void decimation(int compressNum);//8

    void frequency();//12

    void entropy();

    void DPCM(bool flagForSave);

    void readRGB(std::ifstream * pFile);

    void writeRGB(std::ofstream * oFile);

    void loadBMP(const char *fileName);

    void saveBMP(const char *fileName);

    void onlyBlue();

    void onlyGreen();

    void onlyRed();

    double calcCorrelation(BMP *otherBMP);

    void RGBtoYCbCrCalc();

    void YCbCrtoRGBCalc();

    BYTE clipping(double num);

    void onlyY();

    void onlyCb();

    void onlyCr();

    void saveY(const char *fileName);

    void saveCb(const char *fileName);

    void saveCr(const char *fileName);

    void PSNR(BMP *otherBMP);

    BYTE arifmCalc4(BYTE x1, BYTE x2, BYTE x3, BYTE x4);

    BYTE arifmCalc3(BYTE x1, BYTE x2, BYTE x3);

    BYTE arifmCalc2(BYTE x1, BYTE x2);

    void calcFreq(std::vector<float>&freqB, std::vector<float>&freqG, std::vector<float>&freqR ,std::vector<float>&freqY, std::vector<float>&freqCb, std::vector<float>&freqCr);

    void DPCM1(std::vector<std::vector<int>>&d, int color);

    void DPCM2(std::vector<std::vector<int>>&d, int color);

    void DPCM3(std::vector<std::vector<int>>&d, int color);

    void DPCM4(std::vector<std::vector<int>>&d, int color);

    void calcFreqDPCM(std::vector<std::vector<int>>&d, std::map<int,double> &map);

    void writeDPCM(std::vector<std::vector<int>>&dB, std::vector<std::vector<int>>&dG, std::vector<std::vector<int>>&dR, std::vector<std::vector<int>>&dY, std::vector<std::vector<int>>&dCb, std::vector<std::vector<int>>&dCr, const char *fileName);

    void entropyDPCM(std::vector<std::vector<int>>&d, const char * name);

    void bitPlane();

    double calcCorrelationBitPlane(std::vector<std::vector<BYTE>>plane1, std::vector<std::vector<BYTE>> plane2);

    void rgbBit(std::vector<std::vector<BYTE>> bitPlane, std::vector<std::vector<RGBTRIPLE>>&rgbB);

    void setRGB(std::vector<std::vector<RGBTRIPLE>>rgbB);

    void autocor();

    int mod(int a, int b);

    double correlationCofRGB(std::vector<std::vector<RGBTRIPLE>> rgb, std::vector<std::vector<RGBTRIPLE>> rgbB);

    std::vector<std::vector<RGBTRIPLE>> onlyBlueRGB(std::vector<std::vector<RGBTRIPLE>> rgb);

    std::vector<std::vector<RGBTRIPLE>> onlyGreenRGB(std::vector<std::vector<RGBTRIPLE>> rgb);

    std::vector<std::vector<RGBTRIPLE>> onlyRedRGB(std::vector<std::vector<RGBTRIPLE>> rgb);
};


#endif //CLIONPROJECTS_BMP_H
