#include <iomanip>
#include "BMP.h"

BMP::BMP() = default;

BMP::BMP(BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header, std::vector<std::vector<RGBTRIPLE>> rgb){
    this->file_header = file_header;
    this->info_header = info_header;
    this->rgb = rgb;
}

BITMAPINFOHEADER BMP::getInfoHeader() {
    return info_header;
}

BITMAPFILEHEADER BMP::getFileHeader() {
    return file_header;
}

std::vector<std::vector<RGBTRIPLE>> BMP::getRGB() {
    return rgb;
}

void BMP::readRGB(std::ifstream * pFile) {
    for (int i = 0; i < info_header.biHeight; i++) {
        std::vector<RGBTRIPLE> buf;
        for (int j = 0; j < info_header.biWidth; j++) {
            RGBTRIPLE rgbtriple;
            pFile->read(reinterpret_cast<char *>(&rgbtriple.rgbBlue), sizeof(BYTE));
            pFile->read(reinterpret_cast<char *>(&rgbtriple.rgbGreen), sizeof(BYTE));
            pFile->read(reinterpret_cast<char *>(&rgbtriple.rgbRed), sizeof(BYTE));
            buf.push_back(rgbtriple);
        }
        rgb.push_back(buf);
    }
    if((info_header.biWidth * 3) % 4 != 0){
        int tmp = (info_header.biWidth * 3) % 4;
        int size_trash = 4 - tmp;
        for(int i = 0; i < size_trash; i++){
            pFile->read(reinterpret_cast<char *>(&trash), sizeof(BYTE));
        }
    }
}

void BMP::writeRGB(std::ofstream *oFile) {
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            oFile->write(reinterpret_cast<char *>(&rgb[i][j].rgbBlue), sizeof(BYTE));
            oFile->write(reinterpret_cast<char *>(&rgb[i][j].rgbGreen), sizeof(BYTE));
            oFile->write(reinterpret_cast<char *>(&rgb[i][j].rgbRed), sizeof(BYTE));
        }
    }
    if((info_header.biWidth * 3) % 4 != 0){
        int tmp = (info_header.biWidth * 3) % 4;
        int size_trash = 4 - tmp;
        for(int i = 0; i < size_trash; i++){
            oFile->write(reinterpret_cast<char *>(&trash), sizeof(BYTE));
        }
    }
}

void BMP::loadBMP(const char *fileName) {
    std::ifstream pFile(fileName, std::ios_base::in | std::ios_base::binary);

    if(!pFile.is_open()){
        std::cout << "File not found!";
        return;
    }

    pFile.read(reinterpret_cast<char *>(&file_header), sizeof(BITMAPFILEHEADER));
    pFile.read(reinterpret_cast<char *>(&info_header), sizeof(BITMAPINFOHEADER));

    readRGB(&pFile);
    pFile.close();
}

void BMP::saveBMP(const char *fileName) {
    std::ofstream oFile(fileName, std::ios_base::out | std::ios_base::binary);

    oFile.write(reinterpret_cast<char *>(&file_header), sizeof(BITMAPFILEHEADER));
    oFile.write(reinterpret_cast<char *>(&info_header), sizeof(BITMAPINFOHEADER));

    writeRGB(&oFile);
    oFile.close();
}

void BMP::onlyBlue() {
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            rgb[i][j].rgbGreen = 0;
            rgb[i][j].rgbRed = 0;
        }
    }
}

void BMP::onlyGreen() {
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            rgb[i][j].rgbBlue = 0;
            rgb[i][j].rgbRed = 0;
        }
    }
}

void BMP::onlyRed() {
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            rgb[i][j].rgbBlue = 0;
            rgb[i][j].rgbGreen = 0;
        }
    }
}

double BMP::calcCorrelation(BMP *otherBMP) {
    std::vector<std::vector<RGBTRIPLE>> rgbOther;
    rgbOther = otherBMP->getRGB();
    int W = otherBMP->getInfoHeader().biWidth;
    int H = otherBMP->getInfoHeader().biHeight;

    double MA = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            MA += rgb[i][j].rgbBlue + rgb[i][j].rgbRed + rgb[i][j].rgbGreen;
        }
    }
    MA /= W * H;

    double MB = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            MB += rgbOther[i][j].rgbBlue + rgbOther[i][j].rgbRed + rgbOther[i][j].rgbGreen;
        }
    }
    MB /= W * H;

    double sigmaA = 0;
    double tmp = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            tmp += pow((rgb[i][j].rgbBlue + rgb[i][j].rgbRed + rgb[i][j].rgbGreen) - MA, 2);
        }
    }
    sigmaA = sqrt(tmp / ((H * W) - 1));


    double sigmaB = 0;
    tmp = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            tmp += pow((rgbOther[i][j].rgbBlue + rgbOther[i][j].rgbRed + rgbOther[i][j].rgbGreen) - MB, 2);
        }
    }
    sigmaB = sqrt(tmp / ((H * W) - 1));

    double M_AB = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            M_AB += ((rgb[i][j].rgbBlue + rgb[i][j].rgbRed + rgb[i][j].rgbGreen) - MA) *
                    ((rgbOther[i][j].rgbBlue + rgbOther[i][j].rgbRed + rgbOther[i][j].rgbGreen) - MB);
        }
    }
    M_AB /= W * H;
    double sigmaAB = sigmaA * sigmaB;
    return M_AB / sigmaAB;
}

BYTE BMP::clipping(double x) {
    if (x > 255) {
        x = 255;
    } else if (x < 0) {
        x = 0;
    }
    return static_cast<BYTE>(round(x));
}

void BMP::RGBtoYCbCrCalc(){
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            RGBTRIPLE newTriple;
            double tmp_num = 0;
            tmp_num = 0.299 * rgb[i][j].rgbRed + 0.587 * rgb[i][j].rgbGreen + 0.114 * rgb[i][j].rgbBlue;
            newTriple.rgbBlue = clipping(tmp_num);
            tmp_num = 0.5643 * (rgb[i][j].rgbBlue - newTriple.rgbBlue) + 128;
            newTriple.rgbGreen = clipping(tmp_num);
            tmp_num = 0.7132 * (rgb[i][j].rgbRed - newTriple.rgbBlue) + 128;
            newTriple.rgbRed = clipping(tmp_num);
            rgb[i][j] = newTriple;
        }
    }
}

void BMP::RGBtoYCbCr() {
    BMP * fileYCbCr = new BMP();
    fileYCbCr->loadBMP("D:\\CLionProjects\\barbara.bmp");
    fileYCbCr->RGBtoYCbCrCalc();

    BMP * YBMP = new BMP(fileYCbCr->getFileHeader(), fileYCbCr->getInfoHeader(), fileYCbCr->getRGB());
    YBMP->onlyY();

    BMP * CbBMP = new BMP(fileYCbCr->getFileHeader(), fileYCbCr->getInfoHeader(), fileYCbCr->getRGB());
    CbBMP->onlyCb();

    BMP * CrBMP = new BMP(fileYCbCr->getFileHeader(), fileYCbCr->getInfoHeader(), fileYCbCr->getRGB());
    CrBMP->onlyCr();
    std::cout << std::endl;
    std::cout << "Correlation coefficient between Y and Cb: " << YBMP->calcCorrelation(CbBMP) << std::endl;
    std::cout << "Correlation coefficient between Y and Cr: " << YBMP->calcCorrelation(CrBMP) << std::endl;
    std::cout << "Correlation coefficient between Cb and Cr: " << CbBMP->calcCorrelation(CrBMP) << std::endl;

    YBMP->saveY("D:\\CLionProjects\\6\\onlyY.bmp");
    CbBMP->saveCb("D:\\CLionProjects\\6\\onlyCb.bmp");
    CrBMP->saveCr("D:\\CLionProjects\\6\\onlyCr.bmp");
}

void BMP::onlyY() {
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            rgb[i][j].rgbRed = 0;
            rgb[i][j].rgbGreen = 0;
        }
    }
}

void BMP::onlyCb() {
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            rgb[i][j].rgbBlue = 0;
            rgb[i][j].rgbRed = 0;
        }
    }
}

void BMP::onlyCr() {
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            rgb[i][j].rgbBlue = 0;
            rgb[i][j].rgbGreen = 0;
        }
    }
}

void BMP::saveY(const char *fileName) {
    std::ofstream oFile(fileName, std::ios_base::out | std::ios_base::binary);

    oFile.write(reinterpret_cast<char *>(&file_header), sizeof(BITMAPFILEHEADER));
    oFile.write(reinterpret_cast<char *>(&info_header), sizeof(BITMAPINFOHEADER));

    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbBlue), sizeof(BYTE));
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbBlue), sizeof(BYTE));
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbBlue), sizeof(BYTE));
        }
    }

    oFile.close();
}

void BMP::saveCb(const char *fileName) {
    std::ofstream oFile(fileName, std::ios_base::out | std::ios_base::binary);

    oFile.write((char *)(&file_header), sizeof(BITMAPFILEHEADER));
    oFile.write((char *)(&info_header), sizeof(BITMAPINFOHEADER));

    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbGreen), sizeof(BYTE));
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbGreen), sizeof(BYTE));
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbGreen), sizeof(BYTE));
        }
    }

    oFile.close();
}

void BMP::saveCr(const char *fileName) {
    std::ofstream oFile(fileName, std::ios_base::out | std::ios_base::binary);

    oFile.write(reinterpret_cast<char *>(&file_header), sizeof(BITMAPFILEHEADER));
    oFile.write(reinterpret_cast<char *>(&info_header), sizeof(BITMAPINFOHEADER));

    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbRed), sizeof(BYTE));
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbRed), sizeof(BYTE));
            oFile.write(reinterpret_cast<char *>(&rgb[i][j].rgbRed), sizeof(BYTE));
        }
    }

    oFile.close();
}

void BMP::YCbCrtoRGBCalc() {
    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            RGBTRIPLE rgbTriple;
            double tmp_num = 0;
            tmp_num = rgb[i][j].rgbBlue - (0.714 * (rgb[i][j].rgbRed - 128)) - (0.334 * (rgb[i][j].rgbGreen - 128));
            rgbTriple.rgbGreen = clipping(tmp_num);
            tmp_num = rgb[i][j].rgbBlue + (1.402 * (rgb[i][j].rgbRed - 128));
            rgbTriple.rgbRed = clipping(tmp_num);
            tmp_num = rgb[i][j].rgbBlue + (1.772 * (rgb[i][j].rgbGreen - 128));
            rgbTriple.rgbBlue = clipping(tmp_num);
            rgb[i][j] = rgbTriple;
        }
    }
}

void BMP::YCbCrToRGB() {
    BMP * origFile = new BMP();
    origFile->loadBMP("D:\\CLionProjects\\barbara.bmp");
    BMP * fileYCbCr = new BMP();
    fileYCbCr->loadBMP("D:\\CLionProjects\\barbara.bmp");
    fileYCbCr->RGBtoYCbCrCalc();
    fileYCbCr->YCbCrtoRGBCalc();
    fileYCbCr->saveBMP("D:\\CLionProjects\\7\\YCbCrToRGB.bmp");
    origFile->PSNR(fileYCbCr);
}

void BMP::PSNR(BMP *otherBMP) {
    std::vector<std::vector<RGBTRIPLE>> rgbOther;
    rgbOther = otherBMP->getRGB();
    int W = info_header.biWidth;
    int H = info_header.biHeight;
    double Ib = 0;
    double Ig = 0;
    double Ir = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            Ib += std::pow(rgb[i][j].rgbBlue - rgbOther[i][j].rgbBlue, 2);
            Ig += std::pow(rgb[i][j].rgbGreen - rgbOther[i][j].rgbGreen, 2);
            Ir += std::pow(rgb[i][j].rgbRed - rgbOther[i][j].rgbRed, 2);
        }
    }

    double PSNRb = 0;
    PSNRb = 10 * (std::log10((W * H) * std::pow((std::pow(2,8) - 1),2)/ Ib));
    double PSNRg = 0;
    PSNRg = 10 * (std::log10((W * H) * std::pow((std::pow(2,8) - 1),2)/ Ig));
    double PSNRr = 0;
    PSNRr = 10 * (std::log10((W * H) * std::pow((std::pow(2,8) - 1),2)/ Ir));


    std::cout << "PSNR for blue : " << PSNRb << std::endl;
    std::cout << "PSNR for green : " << PSNRg << std::endl;
    std::cout << "PSNR for red : " << PSNRr << std::endl;

}

void BMP::onlyOneColor() {
    BMP * blueBMP = new BMP(getFileHeader(), getInfoHeader(), getRGB());
    blueBMP->onlyBlue();
    blueBMP->saveBMP("D:\\CLionProjects\\3\\onlyBlue.bmp");

    BMP * greenBMP = new BMP(getFileHeader(), getInfoHeader(), getRGB());
    greenBMP->onlyGreen();
    greenBMP->saveBMP("D:\\CLionProjects\\3\\onlyGreen.bmp");

    BMP * redBMP = new BMP(getFileHeader(), getInfoHeader(), getRGB());
    redBMP->onlyRed();
    redBMP->saveBMP("D:\\CLionProjects\\3\\onlyRed.bmp");
}

void BMP::correlation() {
    BMP * blueBMP = new BMP(getFileHeader(), getInfoHeader(), getRGB());
    blueBMP->onlyBlue();

    BMP * greenBMP = new BMP(getFileHeader(), getInfoHeader(), getRGB());
    greenBMP->onlyGreen();

    BMP * redBMP = new BMP(getFileHeader(), getInfoHeader(), getRGB());
    redBMP->onlyRed();

    std::cout << "Correlation coefficient between red and green: " << redBMP->calcCorrelation(greenBMP) << std::endl;
    std::cout << "Correlation coefficient between red and blue: " << redBMP->calcCorrelation(blueBMP) << std::endl;
    std::cout << "Correlation coefficient between blue and green: " << blueBMP->calcCorrelation(greenBMP) << std::endl;
}

BYTE BMP::arifmCalc4(BYTE x1, BYTE x2, BYTE x3, BYTE x4) {
    return ((x1 + x2 + x3 + x4) / 4);
}

BYTE BMP::arifmCalc3(BYTE x1, BYTE x2, BYTE x3) {
    return ((x1 + x2 + x3) / 3);
}

BYTE BMP::arifmCalc2(BYTE x1, BYTE x2) {
    return ((x1 + x2) / 2);
}

void BMP::decimation(int compressNum) {
    BMP * fileYCbCr = new BMP();
    fileYCbCr->loadBMP("D:\\CLionProjects\\barbara.bmp");
    fileYCbCr->RGBtoYCbCrCalc();

    BMP * origYCbCr = new BMP();
    origYCbCr->loadBMP("D:\\CLionProjects\\barbara.bmp");
    origYCbCr->RGBtoYCbCrCalc();

    BMP * origRGB = new BMP();
    origRGB->loadBMP("D:\\CLionProjects\\barbara.bmp");

    std::vector<std::vector<BYTE>> onlyY;
    std::vector<std::vector<BYTE>> onlyCb;
    std::vector<std::vector<BYTE>> onlyCr;

    for (int i = 0; i < fileYCbCr->info_header.biHeight; i++) {
        std::vector<BYTE> bufY;
        std::vector<BYTE> bufCb;
        std::vector<BYTE> bufCr;
        for (int j = 0; j < fileYCbCr->info_header.biWidth; j++) {
            bufY.push_back(fileYCbCr->rgb[i][j].rgbBlue);
            bufCb.push_back(fileYCbCr->rgb[i][j].rgbGreen);
            bufCr.push_back(fileYCbCr->rgb[i][j].rgbRed);
        }
        onlyY.push_back(bufY);
        onlyCb.push_back(bufCb);
        onlyCr.push_back(bufCr);
    }

    //decimation - 2x
    //method A
    std::vector<std::vector<BYTE>> onlyCbA;
    std::vector<std::vector<BYTE>> onlyCrA;

    for (int i = 0; i < onlyCb.size(); i++) {
        if (i % 2 == 1){
            continue;
        }
        std::vector<BYTE> bufCb;
        std::vector<BYTE> bufCr;
        for (int j = 0; j < onlyCb[i].size(); j++) {
            if (j % 2 == 1){
                continue;
            }
            bufCb.push_back(onlyCb[i][j]);
            bufCr.push_back(onlyCr[i][j]);
        }
        onlyCbA.push_back(bufCb);
        onlyCrA.push_back(bufCr);
    }

    std::vector<std::vector<BYTE>> onlyCbA4;
    std::vector<std::vector<BYTE>> onlyCrA4;
    //decimation - 4x
    if(compressNum == 4){
        for (int i = 0; i < onlyCbA.size(); i++) {
            if (i % 2 == 1){
                continue;
            }
            std::vector<BYTE> bufCb;
            std::vector<BYTE> bufCr;
            for (int j = 0; j < onlyCbA[i].size(); j++) {
                if (j % 2 == 1){
                    continue;
                }
                bufCb.push_back(onlyCbA[i][j]);
                bufCr.push_back(onlyCrA[i][j]);
            }
            onlyCbA4.push_back(bufCb);
            onlyCrA4.push_back(bufCr);
        }
    }

    //method B
    std::vector<std::vector<BYTE>> onlyCbB;
    std::vector<std::vector<BYTE>> onlyCrB;


    for (int i = 0; i < onlyCb.size(); i += 2) {
        std::vector<BYTE> bufCb;
        std::vector<BYTE> bufCr;
        for (int j = 0; j < onlyCb[i].size(); j += 2) {
            bufCb.push_back(arifmCalc4(onlyCb[i][j], onlyCb[i][j + 1], onlyCb[i + 1][j], onlyCb[i + 1][j + 1]));
            bufCr.push_back(arifmCalc4(onlyCr[i][j], onlyCr[i][j + 1], onlyCr[i + 1][j], onlyCr[i + 1][j + 1]));
        }
        onlyCbB.push_back(bufCb);
        onlyCrB.push_back(bufCr);
    }


    //decimation - 4x
    std::vector<std::vector<BYTE>> onlyCbB4;
    std::vector<std::vector<BYTE>> onlyCrB4;
    if(compressNum == 4){
        for (int i = 0; i < onlyCbB.size(); i += 2) {
            std::vector<BYTE> bufCb;
            std::vector<BYTE> bufCr;
            for (int j = 0; j < onlyCbB[i].size(); j += 2) {
                bufCb.push_back(arifmCalc4(onlyCbB[i][j], onlyCbB[i][j + 1], onlyCbB[i + 1][j], onlyCbB[i + 1][j + 1]));
                bufCr.push_back(arifmCalc4(onlyCrB[i][j], onlyCrB[i][j + 1], onlyCrB[i + 1][j], onlyCrB[i + 1][j + 1]));
            }
            onlyCbB4.push_back(bufCb);
            onlyCrB4.push_back(bufCr);
        }
    }

    //redecimation 2x
    //method A
    std::vector<std::vector<BYTE>> onlyCbARestore;
    std::vector<std::vector<BYTE>> onlyCrARestore;
    int k = 0;

    for(int i = 0; i < onlyCb.size(); i++){
        std::vector<BYTE> bufCb;
        std::vector<BYTE> bufCr;
        if(i % 2 == 0) {
            for (int j = 0; j < onlyCbA[k].size(); j++) {
                bufCb.push_back(onlyCbA[k][j]);
                bufCb.push_back(onlyCbA[k][j]);
                bufCr.push_back(onlyCrA[k][j]);
                bufCr.push_back(onlyCrA[k][j]);
            }
            k++;
        }else {
            for(int j = 0; j < onlyCbARestore[i - 1].size(); j++) {
                bufCb.push_back(onlyCbARestore[i - 1][j]);
                bufCr.push_back(onlyCrARestore[i - 1][j]);
            }
        }
        onlyCbARestore.push_back(bufCb);
        onlyCrARestore.push_back(bufCr);
    }

    for (int i = 0; i < fileYCbCr->info_header.biHeight; i++) {
        for (int j = 0; j < fileYCbCr->info_header.biWidth; j++) {
            RGBTRIPLE newTriple;
            newTriple.rgbBlue = onlyY[i][j];
            newTriple.rgbGreen = onlyCbARestore[i][j];
            newTriple.rgbRed = onlyCrARestore[i][j];
            fileYCbCr->rgb[i][j] = newTriple;
        }
    }

    std::cout << std::endl;
    std::cout << "PSNR for Cb, Cr (decimation 2x method A): " << std::endl << "Green == Cb, Red = Cr" << std::endl;
    origYCbCr->PSNR(fileYCbCr);

    fileYCbCr->YCbCrtoRGBCalc();

    std::cout << std::endl;
    std::cout << "PSNR for B, G, R (decimation 2x method A): " << std::endl;
    origRGB->PSNR(fileYCbCr);

    fileYCbCr->saveBMP("D:\\CLionProjects\\9\\barbaraAfterDecimationA2x.bmp");

    if(compressNum == 4){
        std::vector<std::vector<BYTE>> onlyCbA4Restore;
        std::vector<std::vector<BYTE>> onlyCrA4Restore;

        std::vector<std::vector<BYTE>> tmponlyCbA4;
        std::vector<std::vector<BYTE>> tmponlyCrA4;
        k = 0;
        for(int i = 0; i < onlyCbA.size(); i++){
            std::vector<BYTE> bufCb;
            std::vector<BYTE> bufCr;
            if(i % 2 == 0) {
                for (int j = 0; j < onlyCbA4[k].size(); j++) {
                    bufCb.push_back(onlyCbA4[k][j]);
                    bufCb.push_back(onlyCbA4[k][j]);
                    bufCr.push_back(onlyCrA4[k][j]);
                    bufCr.push_back(onlyCrA4[k][j]);
                }
                k++;
            }else {
                for(int j = 0; j < tmponlyCbA4[i - 1].size(); j++) {
                    bufCb.push_back(tmponlyCbA4[i - 1][j]);
                    bufCr.push_back(tmponlyCrA4[i - 1][j]);
                }
            }
            tmponlyCbA4.push_back(bufCb);
            tmponlyCrA4.push_back(bufCr);
        }

        k = 0;
        for(int i = 0; i < onlyCb.size(); i++){
            std::vector<BYTE> bufCb;
            std::vector<BYTE> bufCr;
            if(i % 2 == 0) {
                for (int j = 0; j < tmponlyCbA4[k].size(); j++) {
                    bufCb.push_back(tmponlyCbA4[k][j]);
                    bufCb.push_back(tmponlyCbA4[k][j]);
                    bufCr.push_back(tmponlyCrA4[k][j]);
                    bufCr.push_back(tmponlyCrA4[k][j]);
                }
                k++;
            }else {
                for(int j = 0; j < onlyCbA4Restore[i - 1].size(); j++) {
                    bufCb.push_back(onlyCbA4Restore[i - 1][j]);
                    bufCr.push_back(onlyCrA4Restore[i - 1][j]);
                }
            }
            onlyCbA4Restore.push_back(bufCb);
            onlyCrA4Restore.push_back(bufCr);
        }

        for (int i = 0; i < fileYCbCr->info_header.biHeight; i++) {
            for (int j = 0; j < fileYCbCr->info_header.biWidth; j++) {
                RGBTRIPLE newTriple;
                newTriple.rgbBlue = onlyY[i][j];
                newTriple.rgbGreen = onlyCbA4Restore[i][j];
                newTriple.rgbRed = onlyCrA4Restore[i][j];
                fileYCbCr->rgb[i][j] = newTriple;
            }
        }

        std::cout << std::endl;
        std::cout << "PSNR for Cb, Cr (decimation 4x method A): " << std::endl << "Green == Cb, Red = Cr" << std::endl;
        origYCbCr->PSNR(fileYCbCr);

        fileYCbCr->YCbCrtoRGBCalc();

        std::cout << std::endl;
        std::cout << "PSNR for B, G, R (decimation 4x method A): " << std::endl;
        origRGB->PSNR(fileYCbCr);

        fileYCbCr->saveBMP("D:\\CLionProjects\\11\\barbaraAfterDecimationA4x.bmp");
    }

    //method B

    std::vector<std::vector<BYTE>> onlyCbBRestore;
    std::vector<std::vector<BYTE>> onlyCrBRestore;
    k = 0;

    for(int i = 0; i < onlyCb.size(); i++){
        std::vector<BYTE> bufCb;
        std::vector<BYTE> bufCr;
        if(i % 2 == 0){
            int numCb = 0;
            int numCr = 0;
            for(int j = 0; j < onlyCbB[0].size(); j++) {
                if (i == 0) {
                    bufCb.push_back(onlyCbB[k][j]);
                    bufCb.push_back(onlyCbB[k][j]);
                    bufCr.push_back(onlyCrB[k][j]);
                    bufCr.push_back(onlyCrB[k][j]);
                    continue;
                }
                else {
                    bufCb.push_back(onlyCbB[k][j]);
                    numCb++;
                    bufCb.push_back(arifmCalc2(onlyCbB[k][j], onlyCbBRestore[i - 1][numCb]));
                    numCb++;
                    bufCr.push_back(onlyCrB[k][j]);
                    numCr++;
                    bufCr.push_back(arifmCalc2(onlyCrB[k][j], onlyCrBRestore[i - 1][numCr]));
                    numCr++;
                }
            }
            k++;
        }else{
            BYTE prev_numCb = 0;
            BYTE prev_numCr = 0;
            for(int j = 0; j < onlyCb[0].size(); j++) {
                if(j == 0){
                    bufCb.push_back(onlyCbBRestore[i - 1][j]);
                    bufCr.push_back(onlyCrBRestore[i - 1][j]);
                    prev_numCb = onlyCbBRestore[i - 1][j];
                    prev_numCr = onlyCrBRestore[i - 1][j];
                }else{
                    prev_numCb = arifmCalc2(prev_numCb, onlyCbBRestore[i - 1][j]);
                    prev_numCr = arifmCalc2(prev_numCr, onlyCrBRestore[i - 1][j]);
                    bufCb.push_back(prev_numCb);
                    bufCr.push_back(prev_numCr);
                }
            }
        }
        onlyCbBRestore.push_back(bufCb);
        onlyCrBRestore.push_back(bufCr);
    }


    for (int i = 0; i < fileYCbCr->info_header.biHeight; i++) {
        for (int j = 0; j < fileYCbCr->info_header.biWidth; j++) {
            RGBTRIPLE newTriple;
            newTriple.rgbBlue = onlyY[i][j];
            newTriple.rgbGreen = onlyCbBRestore[i][j];
            newTriple.rgbRed = onlyCrBRestore[i][j];
            fileYCbCr->rgb[i][j] = newTriple;
        }
    }

    std::cout << std::endl;
    std::cout << "PSNR for Cb, Cr (decimation 2x method B): " << std::endl << "Green == Cb, Red = Cr" << std::endl;
    origYCbCr->PSNR(fileYCbCr);

    fileYCbCr->YCbCrtoRGBCalc();

    std::cout << std::endl;
    std::cout << "PSNR for B, G, R (decimation 2x method B): " << std::endl;
    origRGB->PSNR(fileYCbCr);

    fileYCbCr->saveBMP("D:\\CLionProjects\\9\\barbaraAfterDecimationB2x.bmp");

    if(compressNum == 4){
        std::vector<std::vector<BYTE>> onlyCbB4Restore;
        std::vector<std::vector<BYTE>> onlyCrB4Restore;

        std::vector<std::vector<BYTE>> tmponlyCbB4;
        std::vector<std::vector<BYTE>> tmponlyCrB4;
        k = 0;
        for(int i = 0; i < onlyCbB.size(); i++){
            std::vector<BYTE> bufCb;
            std::vector<BYTE> bufCr;
            if(i % 2 == 0){
                int numCb = 0;
                int numCr = 0;
                for(int j = 0; j < onlyCbB4[0].size(); j++) {
                    if (i == 0) {
                        bufCb.push_back(onlyCbB4[k][j]);
                        bufCb.push_back(onlyCbB4[k][j]);
                        bufCr.push_back(onlyCrB4[k][j]);
                        bufCr.push_back(onlyCrB4[k][j]);
                        continue;
                    }
                    else {
                        bufCb.push_back(onlyCbB4[k][j]);
                        numCb++;
                        bufCb.push_back(arifmCalc2(onlyCbB4[k][j], tmponlyCbB4[i - 1][numCb]));
                        numCb++;
                        bufCr.push_back(onlyCrB4[k][j]);
                        numCr++;
                        bufCr.push_back(arifmCalc2(onlyCrB4[k][j], tmponlyCrB4[i - 1][numCr]));
                        numCr++;
                    }
                }
                k++;
            }else{
                BYTE prev_numCb = 0;
                BYTE prev_numCr = 0;
                for(int j = 0; j < onlyCbB[0].size(); j++) {
                    if(j == 0){
                        bufCb.push_back(tmponlyCbB4[i - 1][j]);
                        bufCr.push_back(tmponlyCrB4[i - 1][j]);
                        prev_numCb = tmponlyCbB4[i - 1][j];
                        prev_numCr = tmponlyCrB4[i - 1][j];
                    }else{
                        prev_numCb = arifmCalc2(prev_numCb, tmponlyCbB4[i - 1][j]);
                        prev_numCr = arifmCalc2(prev_numCr, tmponlyCrB4[i - 1][j]);
                        bufCb.push_back(prev_numCb);
                        bufCr.push_back(prev_numCr);
                    }
                }
            }
            tmponlyCbB4.push_back(bufCb);
            tmponlyCrB4.push_back(bufCr);
        }


        k = 0;
        for(int i = 0; i < onlyCb.size(); i++){
            std::vector<BYTE> bufCb;
            std::vector<BYTE> bufCr;
            if(i % 2 == 0){
                int numCb = 0;
                int numCr = 0;
                for(int j = 0; j < tmponlyCbB4[0].size(); j++) {
                    if (i == 0) {
                        bufCb.push_back(tmponlyCbB4[k][j]);
                        bufCb.push_back(tmponlyCbB4[k][j]);
                        bufCr.push_back(tmponlyCrB4[k][j]);
                        bufCr.push_back(tmponlyCrB4[k][j]);
                        continue;
                    }
                    else {
                        bufCb.push_back(tmponlyCbB4[k][j]);
                        numCb++;
                        bufCb.push_back(arifmCalc2(tmponlyCbB4[k][j], onlyCbB4Restore[i - 1][numCb]));
                        numCb++;
                        bufCr.push_back(tmponlyCrB4[k][j]);
                        numCr++;
                        bufCr.push_back(arifmCalc2(tmponlyCrB4[k][j], onlyCrB4Restore[i - 1][numCr]));
                        numCr++;
                    }
                }
                k++;
            }else{
                BYTE prev_numCb = 0;
                BYTE prev_numCr = 0;
                for(int j = 0; j < onlyCb[0].size(); j++) {
                    if(j == 0){
                        bufCb.push_back(onlyCbB4Restore[i - 1][j]);
                        bufCr.push_back(onlyCrB4Restore[i - 1][j]);
                        prev_numCb = onlyCbB4Restore[i - 1][j];
                        prev_numCr = onlyCrB4Restore[i - 1][j];
                    }else{
                        prev_numCb = arifmCalc2(prev_numCb, onlyCbB4Restore[i - 1][j]);
                        prev_numCr = arifmCalc2(prev_numCr, onlyCrB4Restore[i - 1][j]);
                        bufCb.push_back(prev_numCb);
                        bufCr.push_back(prev_numCr);
                    }
                }
            }
            onlyCbB4Restore.push_back(bufCb);
            onlyCrB4Restore.push_back(bufCr);
        }
        for (int i = 0; i < fileYCbCr->info_header.biHeight; i++) {
            for (int j = 0; j < fileYCbCr->info_header.biWidth; j++) {
                RGBTRIPLE newTriple;
                newTriple.rgbBlue = onlyY[i][j];
                newTriple.rgbGreen = onlyCbB4Restore[i][j];
                newTriple.rgbRed = onlyCrB4Restore[i][j];
                fileYCbCr->rgb[i][j] = newTriple;
            }
        }

        std::cout << std::endl;
        std::cout << "PSNR for Cb, Cr (decimation 4x method B): " << std::endl << "Green == Cb, Red = Cr" << std::endl;
        origYCbCr->PSNR(fileYCbCr);

        fileYCbCr->YCbCrtoRGBCalc();

        std::cout << std::endl;
        std::cout << "PSNR for B, G, R (decimation 4x method B): " << std::endl;
        origRGB->PSNR(fileYCbCr);

        fileYCbCr->saveBMP("D:\\CLionProjects\\11\\barbaraAfterDecimationB4x.bmp");
    }
}

void BMP::calcFreq(std::vector<float>&freqB, std::vector<float>&freqG, std::vector<float>&freqR ,std::vector<float>&freqY, std::vector<float>&freqCb, std::vector<float>&freqCr) {
    for(int i = 0; i < 256; i++){
        freqB.push_back(0);
        freqG.push_back(0);
        freqR.push_back(0);
        freqY.push_back(0);
        freqCb.push_back(0);
        freqCr.push_back(0);
    }

    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            freqB[rgb[i][j].rgbBlue]++;
            freqG[rgb[i][j].rgbGreen]++;
            freqR[rgb[i][j].rgbRed]++;
        }
    }

    RGBtoYCbCrCalc();

    for (int i = 0; i < info_header.biHeight; i++) {
        for (int j = 0; j < info_header.biWidth; j++) {
            freqY[rgb[i][j].rgbBlue]++;
            freqCb[rgb[i][j].rgbGreen]++;
            freqCr[rgb[i][j].rgbRed]++;
        }
    }

    for(int i = 0; i < freqB.size(); i++){
        freqB[i] /= (info_header.biHeight * info_header.biWidth );
        freqG[i] /= (info_header.biHeight * info_header.biWidth );
        freqR[i] /= (info_header.biHeight * info_header.biWidth );
        freqY[i] /= (info_header.biHeight * info_header.biWidth );
        freqCb[i] /= (info_header.biHeight * info_header.biWidth );
        freqCr[i] /= (info_header.biHeight * info_header.biWidth );
    }

}

void BMP::frequency() {
    BMP * origFile  = new BMP();
    origFile->loadBMP("D:\\CLionProjects\\barbara.bmp");

    std::vector<float> freqB;
    std::vector<float> freqG;
    std::vector<float> freqR;
    std::vector<float> freqY;
    std::vector<float> freqCb;
    std::vector<float> freqCr;

    origFile->calcFreq(freqB,freqG, freqR, freqY, freqCb, freqCr);

    std::ofstream oFileFreq("D:\\CLionProjects\\12\\freq.txt", std::ios_base::out);


    if(!oFileFreq.is_open()){
        std::cout<< "Bad1111";
    }

    for(int i = 0; i < freqB.size(); i++){
        oFileFreq << std::fixed << std::setprecision(6) << freqB[i] << '\t';
        oFileFreq << std::fixed << std::setprecision(6) << freqG[i] << '\t';
        oFileFreq << std::fixed << std::setprecision(6) << freqR[i] << '\t';
        oFileFreq << std::fixed << std::setprecision(6) << freqY[i] << '\t';
        oFileFreq << std::fixed << std::setprecision(6) << freqCb[i] << '\t';
        oFileFreq << std::fixed << std::setprecision(6) << freqCr[i] << '\n';
    }

    oFileFreq.close();
}

void BMP::entropy() {
    BMP * origFile  = new BMP();
    origFile->loadBMP("D:\\CLionProjects\\barbara.bmp");

    std::vector<float> freqB;
    std::vector<float> freqG;
    std::vector<float> freqR;
    std::vector<float> freqY;
    std::vector<float> freqCb;
    std::vector<float> freqCr;

    origFile->calcFreq(freqB,freqG, freqR, freqY, freqCb, freqCr);


    float sumB = 0;
    float sumG = 0;
    float sumR = 0;
    float sumY = 0;
    float sumCb = 0;
    float sumCr = 0;

    for(int i = 0; i < freqB.size(); i++){
        if(freqB[i] != 0){
            sumB += (freqB[i] * std::log2(freqB[i]));
        }

        if(freqG[i] != 0){
            sumG += (freqG[i] * std::log2(freqG[i]));
        }

        if(freqR[i] != 0){
            sumR += (freqR[i] * std::log2(freqR[i]));
        }

        if(freqY[i] != 0){
            sumY += (freqY[i] * std::log2(freqY[i]));
        }

        if(freqCb[i] != 0){
            sumCb += (freqCb[i] * std::log2(freqCb[i]));
        }

        if(freqCr[i] != 0){
            sumCr += (freqCr[i] * std::log2(freqCr[i]));
        }
    }

    sumB *= -1;
    sumG *= -1;
    sumR *= -1;
    sumY *= -1;
    sumCb *= -1;
    sumCr *= -1;

    std::cout << "Entropy for blue = " << sumB << std::endl;
    std::cout << "Entropy for green = " << sumG << std::endl;
    std::cout << "Entropy for red = " << sumR << std::endl;
    std::cout << "Entropy for Y = " << sumY << std::endl;
    std::cout << "Entropy for Cb = " << sumCb << std::endl;
    std::cout << "Entropy for Cr = " << sumCr << std::endl;
}

void BMP::DPCM1(std::vector<std::vector<int>>&d, int color) {
    for (int i = 0; i < info_header.biHeight; i++) {
        std::vector<int> buf;
        if(i == 0){
            continue;
        }
        for (int j = 0; j < info_header.biWidth; j++) {
            if(j == 0){
                continue;
            }
            if(color == 1) {
               buf.push_back(rgb[i][j].rgbBlue - rgb[i][j - 1].rgbBlue);
            }
            else if(color == 2){
                buf.push_back(rgb[i][j].rgbGreen - rgb[i][j - 1].rgbGreen);
            }
            else if(color == 3){
                buf.push_back(rgb[i][j].rgbRed - rgb[i][j - 1].rgbRed);
            }
        }
        d.push_back(buf);
    }
}

void BMP::DPCM2(std::vector<std::vector<int>>&d, int color) {
    for (int i = 0; i < info_header.biHeight; i++) {
        std::vector<int> buf;
        if(i == 0){
            continue;
        }
        for (int j = 0; j < info_header.biWidth; j++) {
            if(j == 0){
                continue;
            }
            if(color == 1) {
                buf.push_back(rgb[i][j].rgbBlue - rgb[i - 1][j].rgbBlue);
            }
            else if(color == 2){
                buf.push_back(rgb[i][j].rgbGreen - rgb[i - 1][j].rgbGreen);
            }
            else if(color == 3){
                buf.push_back(rgb[i][j].rgbRed - rgb[i - 1][j].rgbRed);
            }
        }
        d.push_back(buf);
    }
}

void BMP::DPCM3(std::vector<std::vector<int>>&d, int color) {
    for (int i = 0; i < info_header.biHeight; i++) {
        std::vector<int> buf;
        if(i == 0){
            continue;
        }
        for (int j = 0; j < info_header.biWidth; j++) {
            if(j == 0){
                continue;
            }
            if(color == 1) {
                buf.push_back(rgb[i][j].rgbBlue - rgb[i - 1][j - 1].rgbBlue);
            }
            else if(color == 2){
                buf.push_back(rgb[i][j].rgbGreen - rgb[i - 1][j - 1].rgbGreen);
            }
            else if(color == 3){
                buf.push_back(rgb[i][j].rgbRed - rgb[i - 1][j - 1].rgbRed);
            }
        }
        d.push_back(buf);
    }
}

void BMP::DPCM4(std::vector<std::vector<int>>&d, int color) {
    for (int i = 0; i < info_header.biHeight; i++) {
        std::vector<int> buf;
        if(i == 0){
            continue;
        }
        for (int j = 0; j < info_header.biWidth; j++) {
            if(j == 0){
                continue;
            }
            if(color == 1) {
                buf.push_back(rgb[i][j].rgbBlue - arifmCalc3(rgb[i - 1][j - 1].rgbBlue, rgb[i - 1][j].rgbBlue, rgb[i][j - 1].rgbBlue));
            }
            else if(color == 2){
                buf.push_back(rgb[i][j].rgbGreen - arifmCalc3(rgb[i - 1][j - 1].rgbGreen, rgb[i - 1][j].rgbGreen, rgb[i][j - 1].rgbGreen));
            }
            else if(color == 3){
                buf.push_back(rgb[i][j].rgbRed - arifmCalc3(rgb[i - 1][j - 1].rgbRed, rgb[i - 1][j].rgbRed, rgb[i][j - 1].rgbRed));
            }
        }
        d.push_back(buf);
    }
}

void BMP::calcFreqDPCM(std::vector<std::vector<int>>&d, std::map<int,double> &map){

    for(int i = 0; i < d.size(); i++){
        for (int j = 0; j < d[0].size(); j++){
            std::map<int,double>::iterator it = map.begin();
            it = map.find(d[i][j]);
            if(it != map.end()){
                (*it).second++;
            }else{
                map.insert(std::pair<int,double>(d[i][j], 1));
            }
        }
    }

    double down = (info_header.biWidth * info_header.biHeight) - info_header.biWidth - info_header.biHeight;
    std::map<int,double>::iterator it = map.begin();
    while(it != map.end()){
        (*it).second /= down;
        it++;
    }
}

void BMP::writeDPCM(std::vector<std::vector<int>>&dB, std::vector<std::vector<int>>&dG, std::vector<std::vector<int>>&dR, std::vector<std::vector<int>>&dY, std::vector<std::vector<int>>&dCb, std::vector<std::vector<int>>&dCr, const char *fileName)
{
    std::map<int,double> mapB;
    std::map<int,double> mapG;
    std::map<int,double> mapR;
    std::map<int,double> mapY;
    std::map<int,double> mapCb;
    std::map<int,double> mapCr;

    calcFreqDPCM(dB,mapB);
    calcFreqDPCM(dG,mapG);
    calcFreqDPCM(dR,mapR);
    calcFreqDPCM(dY,mapY);
    calcFreqDPCM(dCb,mapCb);
    calcFreqDPCM(dCr,mapCr);


    std::ofstream oFile(fileName, std::ios_base::out);
    std::map<int,double>::iterator itB = mapB.begin();
    std::map<int,double>::iterator itG = mapG.begin();
    std::map<int,double>::iterator itR = mapR.begin();
    std::map<int,double>::iterator itY = mapY.begin();
    std::map<int,double>::iterator itCb = mapCb.begin();
    std::map<int,double>::iterator itCr = mapCr.begin();

    bool flag = true;

    while(flag){
        flag = false;
        if(itB != mapB.end()) {
            oFile << (*itB).first << '\t' << std::fixed << std::setprecision(9) << (*itB).second << '\t';
            itB++;
            flag = true;
        }
        if(itG != mapG.end()) {
            oFile << (*itG).first << '\t' << std::fixed << std::setprecision(9) << (*itG).second << '\t';
            itG++;
            flag = true;
        }
        if(itR != mapR.end()) {
            oFile << (*itR).first << '\t' << std::fixed << std::setprecision(9) << (*itR).second << '\t';
            itR++;
            flag = true;
        }
        if(itY != mapY.end()) {
            oFile << (*itY).first << '\t' << std::fixed << std::setprecision(9) << (*itY).second << '\t';
            itY++;
            flag = true;
        }
        if(itCb != mapCb.end()) {
            oFile << (*itCb).first << '\t' << std::fixed << std::setprecision(9) << (*itCb).second << '\t';
            itCb++;
            flag = true;
        }
        if(itCr != mapCr.end()) {
            oFile << (*itCr).first << '\t' << std::fixed << std::setprecision(9) << (*itCr).second << '\t';
            itCr++;
            flag = true;
        }
        oFile << '\n';
    }
    oFile.close();
}

void BMP::DPCM(bool flagForSave){
    BMP * origFile = new BMP();
    origFile->loadBMP("D:\\CLionProjects\\barbara.bmp");

    std::vector<std::vector<int>>dB1;
    std::vector<std::vector<int>>dG1;
    std::vector<std::vector<int>>dR1;

    origFile->DPCM1(dB1,1);
    origFile->DPCM1(dG1,2);
    origFile->DPCM1(dR1,3);

    std::vector<std::vector<int>>dB2;
    std::vector<std::vector<int>>dG2;
    std::vector<std::vector<int>>dR2;

    origFile->DPCM2(dB2,1);
    origFile->DPCM2(dG2,2);
    origFile->DPCM2(dR2,3);

    std::vector<std::vector<int>>dB3;
    std::vector<std::vector<int>>dG3;
    std::vector<std::vector<int>>dR3;

    origFile->DPCM3(dB3,1);
    origFile->DPCM3(dG3,2);
    origFile->DPCM3(dR3,3);

    std::vector<std::vector<int>>dB4;
    std::vector<std::vector<int>>dG4;
    std::vector<std::vector<int>>dR4;

    origFile->DPCM4(dB4,1);
    origFile->DPCM4(dG4,2);
    origFile->DPCM4(dR4,3);


    BMP * fileYCbCr = new BMP();
    fileYCbCr->loadBMP("D:\\CLionProjects\\barbara.bmp");
    fileYCbCr ->RGBtoYCbCrCalc();

    std::vector<std::vector<int>>dY1;
    std::vector<std::vector<int>>dCb1;
    std::vector<std::vector<int>>dCr1;

    fileYCbCr->DPCM1(dY1,1);
    fileYCbCr->DPCM1(dCb1,2);
    fileYCbCr->DPCM1(dCr1,3);

    std::vector<std::vector<int>>dY2;
    std::vector<std::vector<int>>dCb2;
    std::vector<std::vector<int>>dCr2;

    fileYCbCr->DPCM2(dY2,1);
    fileYCbCr->DPCM2(dCb2,2);
    fileYCbCr->DPCM2(dCr2,3);

    std::vector<std::vector<int>>dY3;
    std::vector<std::vector<int>>dCb3;
    std::vector<std::vector<int>>dCr3;

    fileYCbCr->DPCM3(dY3,1);
    fileYCbCr->DPCM3(dCb3,2);
    fileYCbCr->DPCM3(dCr3,3);

    std::vector<std::vector<int>>dY4;
    std::vector<std::vector<int>>dCb4;
    std::vector<std::vector<int>>dCr4;

    fileYCbCr->DPCM4(dY4,1);
    fileYCbCr->DPCM4(dCb4,2);
    fileYCbCr->DPCM4(dCr4,3);

    if(flagForSave){
        origFile -> writeDPCM(dB1, dG1, dR1, dY1, dCb1, dCr1, "D:\\CLionProjects\\15\\DPCM1\\DPCM1.txt");
        origFile -> writeDPCM(dB2, dG2, dR2, dY2, dCb2, dCr2, "D:\\CLionProjects\\15\\DPCM2\\DPCM2.txt");
        origFile -> writeDPCM(dB3, dG3, dR3, dY3, dCb3, dCr3, "D:\\CLionProjects\\15\\DPCM3\\DPCM3.txt");
        origFile -> writeDPCM(dB4, dG4, dR4, dY4, dCb4, dCr4, "D:\\CLionProjects\\15\\DPCM4\\DPCM4.txt");

    }else {
        origFile->entropyDPCM(dB1, "blue DPCM1");
        origFile->entropyDPCM(dG1, "green DPCM1");
        origFile->entropyDPCM(dR1, "red DPCM1");

        std::cout << std::endl;

        fileYCbCr->entropyDPCM(dY1, "Y DPCM1");
        fileYCbCr->entropyDPCM(dCb1, "Cb DPCM1");
        fileYCbCr->entropyDPCM(dCr1, "Cr DPCM1");

        std::cout << std::endl;

        origFile->entropyDPCM(dB2, "blue DPCM2");
        origFile->entropyDPCM(dG2, "green DPCM2");
        origFile->entropyDPCM(dR2, "red DPCM2");

        std::cout << std::endl;

        fileYCbCr->entropyDPCM(dY2, "Y DPCM2");
        fileYCbCr->entropyDPCM(dCb2, "Cb DPCM2");
        fileYCbCr->entropyDPCM(dCr2, "Cr DPCM2");

        std::cout << std::endl;

        origFile->entropyDPCM(dB3, "blue DPCM3");
        origFile->entropyDPCM(dG3, "green DPCM3");
        origFile->entropyDPCM(dR3, "red DPCM3");

        std::cout << std::endl;

        fileYCbCr->entropyDPCM(dY3, "Y DPCM3");
        fileYCbCr->entropyDPCM(dCb3, "Cb DPCM3");
        fileYCbCr->entropyDPCM(dCr3, "Cr DPCM3");

        std::cout << std::endl;

        origFile->entropyDPCM(dB4, "blue DPCM4");
        origFile->entropyDPCM(dG4, "green DPCM4");
        origFile->entropyDPCM(dR4, "red DPCM4");

        std::cout << std::endl;

        fileYCbCr->entropyDPCM(dY4, "Y DPCM4");
        fileYCbCr->entropyDPCM(dCb4, "Cb DPCM4");
        fileYCbCr->entropyDPCM(dCr4, "Cr DPCM4");

        std::cout << std::endl;

    }
}

void BMP::entropyDPCM(std::vector<std::vector<int>>&d, const char * name){
    std::map<int,double>map;

    calcFreqDPCM(d,map);

    double sum = 0;

    std::map<int,double>::iterator it = map.begin();
    for(it ; it != map.end(); it++){
        if((*it).second != 0){
            sum += ((*it).second * std::log2((*it).second));
        }
    }

    sum *= -1;

    std::cout << "Entropy for " << name << " = " << sum << std::endl;
}

void BMP::bitPlane() {
    BMP * fileYCbCr = new BMP();
    fileYCbCr->loadBMP("D:\\CLionProjects\\barbara.bmp");
    fileYCbCr ->RGBtoYCbCrCalc();

    std::vector<std::vector<BYTE>> onlyY;

    for (int i = 0; i < fileYCbCr->info_header.biHeight; i++) {
        std::vector<BYTE> bufY;
        for (int j = 0; j < fileYCbCr->info_header.biWidth; j++) {
            bufY.push_back(fileYCbCr->rgb[i][j].rgbBlue);
        }
        onlyY.push_back(bufY);
    }

    std::vector<std::vector<BYTE>> bitplane0;
    std::vector<std::vector<BYTE>> bitplane1;
    std::vector<std::vector<BYTE>> bitplane2;
    std::vector<std::vector<BYTE>> bitplane3;
    std::vector<std::vector<BYTE>> bitplane4;
    std::vector<std::vector<BYTE>> bitplane5;
    std::vector<std::vector<BYTE>> bitplane6;
    std::vector<std::vector<BYTE>> bitplane7;

    for(int i = 0;  i < onlyY.size(); i++){
        std::vector<BYTE> tmpbit0;
        std::vector<BYTE> tmpbit1;
        std::vector<BYTE> tmpbit2;
        std::vector<BYTE> tmpbit3;
        std::vector<BYTE> tmpbit4;
        std::vector<BYTE> tmpbit5;
        std::vector<BYTE> tmpbit6;
        std::vector<BYTE> tmpbit7;
        for(int j = 0; j < onlyY[i].size(); j++){
            int num = onlyY[i][j];
            std::vector<int>vec;
            while(true){
                vec.push_back(num%2);
                num /= 2;
                if (num < 1){
                    break;
                }
            }
            while(vec.size() != 8){
                vec.push_back(0);
            }

            for(int i = 0; i < vec.size(); i++){
                switch (i){
                    case 0:
                        tmpbit0.push_back(vec[0]);
                        break;
                    case 1:
                        tmpbit1.push_back(vec[1]);
                        break;
                    case 2:
                        tmpbit2.push_back(vec[2]);
                        break;
                    case 3:
                        tmpbit3.push_back(vec[3]);
                        break;
                    case 4:
                        tmpbit4.push_back(vec[4]);
                        break;
                    case 5:
                        tmpbit5.push_back(vec[5]);
                        break;
                    case 6:
                        tmpbit6.push_back(vec[6]);
                        break;
                    case 7:
                        tmpbit7.push_back(vec[7]);
                        break;
                    default:
                        break;
                }
            }
        }
        bitplane0.push_back(tmpbit0);
        bitplane1.push_back(tmpbit1);
        bitplane2.push_back(tmpbit2);
        bitplane3.push_back(tmpbit3);
        bitplane4.push_back(tmpbit4);
        bitplane5.push_back(tmpbit5);
        bitplane6.push_back(tmpbit6);
        bitplane7.push_back(tmpbit7);
    }


    std::vector<std::vector<double>> corrBit;

    for(int i = 0; i < 8; i++){
        std::vector<double> tmp(8, 0);
        corrBit.push_back(tmp);
    }

    double tmp = 0;
    for(int i = 0; i < 8; i ++){
        switch (i){
            case 0:
                corrBit[0][i] = 2;
                break;
            case 1:
                tmp = calcCorrelationBitPlane(bitplane0, bitplane1);
                corrBit[0][i] = tmp;
                corrBit[i][0] = tmp;
                break;
            case 2:
                tmp = calcCorrelationBitPlane(bitplane0, bitplane2);
                corrBit[0][i] = tmp;
                corrBit[i][0] = tmp;
                break;
            case 3:
                tmp = calcCorrelationBitPlane(bitplane0, bitplane3);
                corrBit[0][i] = tmp;
                corrBit[i][0] = tmp;
                break;
            case 4:
                tmp = calcCorrelationBitPlane(bitplane0, bitplane4);
                corrBit[0][i] = tmp;
                corrBit[i][0] = tmp;
                break;
            case 5:
                tmp = calcCorrelationBitPlane(bitplane0, bitplane5);
                corrBit[0][i] = tmp;
                corrBit[i][0] = tmp;
                break;
            case 6:
                tmp = calcCorrelationBitPlane(bitplane0, bitplane6);
                corrBit[0][i] = tmp;
                corrBit[i][0] = tmp;
                break;
            case 7:
                tmp = calcCorrelationBitPlane(bitplane0, bitplane7);
                corrBit[0][i] = tmp;
                corrBit[i][0] = tmp;
                break;
        }
    }

    for(int i = 1; i < 8; i ++){
        switch (i){
            case 1:
                corrBit[1][i] = 2;
                break;
            case 2:
                tmp = calcCorrelationBitPlane(bitplane1, bitplane2);
                corrBit[1][i] = tmp;
                corrBit[i][1] = tmp;
                break;
            case 3:
                tmp = calcCorrelationBitPlane(bitplane1, bitplane3);
                corrBit[1][i] = tmp;
                corrBit[i][1] = tmp;
                break;
            case 4:
                tmp = calcCorrelationBitPlane(bitplane1, bitplane4);
                corrBit[1][i] = tmp;
                corrBit[i][1] = tmp;
                break;
            case 5:
                tmp = calcCorrelationBitPlane(bitplane1, bitplane5);
                corrBit[1][i] = tmp;
                corrBit[i][1] = tmp;
                break;
            case 6:
                tmp = calcCorrelationBitPlane(bitplane1, bitplane6);
                corrBit[1][i] = tmp;
                corrBit[i][1] = tmp;
                break;
            case 7:
                tmp = calcCorrelationBitPlane(bitplane1, bitplane7);
                corrBit[1][i] = tmp;
                corrBit[i][1] = tmp;
                break;
        }
    }

    for(int i = 2; i < 8; i ++){
        switch (i){
            case 2:
                corrBit[2][i] = 2;
                break;
            case 3:
                tmp = calcCorrelationBitPlane(bitplane2, bitplane3);
                corrBit[2][i] = tmp;
                corrBit[i][2] = tmp;
                break;
            case 4:
                tmp = calcCorrelationBitPlane(bitplane2, bitplane4);
                corrBit[2][i] = tmp;
                corrBit[i][2] = tmp;
                break;
            case 5:
                tmp = calcCorrelationBitPlane(bitplane2, bitplane5);
                corrBit[2][i] = tmp;
                corrBit[i][2] = tmp;
                break;
            case 6:
                tmp = calcCorrelationBitPlane(bitplane2, bitplane6);
                corrBit[2][i] = tmp;
                corrBit[i][2] = tmp;
                break;
            case 7:
                tmp = calcCorrelationBitPlane(bitplane2, bitplane7);
                corrBit[2][i] = tmp;
                corrBit[i][2] = tmp;
                break;
        }
    }

    for(int i = 3; i < 8; i ++){
        switch (i){
            case 3:
                corrBit[3][i] = 3;
                break;
            case 4:
                tmp = calcCorrelationBitPlane(bitplane3, bitplane4);
                corrBit[3][i] = tmp;
                corrBit[i][3] = tmp;
                break;
            case 5:
                tmp = calcCorrelationBitPlane(bitplane3, bitplane5);
                corrBit[3][i] = tmp;
                corrBit[i][3] = tmp;
                break;
            case 6:
                tmp = calcCorrelationBitPlane(bitplane3, bitplane6);
                corrBit[3][i] = tmp;
                corrBit[i][3] = tmp;
                break;
            case 7:
                tmp = calcCorrelationBitPlane(bitplane3, bitplane7);
                corrBit[3][i] = tmp;
                corrBit[i][3] = tmp;
                break;
        }
    }

    for(int i = 4; i < 8; i ++){
        switch (i){
            case 4:
                corrBit[4][i] = 2;
                break;
            case 5:
                tmp = calcCorrelationBitPlane(bitplane4, bitplane5);
                corrBit[4][i] = tmp;
                corrBit[i][4] = tmp;
                break;
            case 6:
                tmp = calcCorrelationBitPlane(bitplane4, bitplane6);
                corrBit[4][i] = tmp;
                corrBit[i][4] = tmp;
                break;
            case 7:
                tmp = calcCorrelationBitPlane(bitplane4, bitplane7);
                corrBit[4][i] = tmp;
                corrBit[i][4] = tmp;
                break;
        }
    }

    for(int i = 5; i < 8; i ++){
        switch (i){
            case 5:
                corrBit[5][i] = 2;
                break;
            case 6:
                tmp = calcCorrelationBitPlane(bitplane5, bitplane6);
                corrBit[5][i] = tmp;
                corrBit[i][5] = tmp;
                break;
            case 7:
                tmp = calcCorrelationBitPlane(bitplane5, bitplane7);
                corrBit[5][i] = tmp;
                corrBit[i][5] = tmp;
                break;
        }
    }

    for(int i = 6; i < 8; i ++){
        switch (i){
            case 6:
                corrBit[6][i] = 2;
                break;
            case 7:
                tmp = calcCorrelationBitPlane(bitplane6, bitplane7);
                corrBit[6][i] = tmp;
                corrBit[i][6] = tmp;
                break;
        }
    }

    corrBit[7][7] = 2;

    std::ofstream oFile("D:\\CLionProjects\\dop\\corr.txt", std::ios_base::out);

    oFile << '\t'  << '\t';
    for(int i = 0; i < 8; i++){
        oFile << "bitplane" << i << '\t'  << '\t';
    }
    oFile << '\n';

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(j == 0){
                oFile << "bitplane" << i  << ' ' << '\t';
            }
            if(i == j){
                oFile << '\t' << '\t' << '\t';
                continue;
            }
            oFile << corrBit[i][j] << '\t' << '\t';
        }
        oFile << '\n';
    }

    oFile.close();

    std::vector<std::vector<RGBTRIPLE>> rgbBit0;
    std::vector<std::vector<RGBTRIPLE>> rgbBit1;
    std::vector<std::vector<RGBTRIPLE>> rgbBit2;
    std::vector<std::vector<RGBTRIPLE>> rgbBit3;
    std::vector<std::vector<RGBTRIPLE>> rgbBit4;
    std::vector<std::vector<RGBTRIPLE>> rgbBit5;
    std::vector<std::vector<RGBTRIPLE>> rgbBit6;
    std::vector<std::vector<RGBTRIPLE>> rgbBit7;

    rgbBit(bitplane0, rgbBit0);
    rgbBit(bitplane1, rgbBit1);
    rgbBit(bitplane2, rgbBit2);
    rgbBit(bitplane3, rgbBit3);
    rgbBit(bitplane4, rgbBit4);
    rgbBit(bitplane5, rgbBit5);
    rgbBit(bitplane6, rgbBit6);
    rgbBit(bitplane7, rgbBit7);


    BMP bmp;
    bmp.loadBMP("D:\\CLionProjects\\barbara.bmp");
    bmp.setRGB(rgbBit0);
    bmp.saveBMP("D:\\CLionProjects\\dop\\barbara0.bmp");

    bmp.setRGB(rgbBit1);
    bmp.saveBMP("D:\\CLionProjects\\dop\\barbara1.bmp");

    bmp.setRGB(rgbBit2);
    bmp.saveBMP("D:\\CLionProjects\\dop\\barbara2.bmp");

    bmp.setRGB(rgbBit3);
    bmp.saveBMP("D:\\CLionProjects\\dop\\barbara3.bmp");

    bmp.setRGB(rgbBit4);
    bmp.saveBMP("D:\\CLionProjects\\dop\\barbara4.bmp");

    bmp.setRGB(rgbBit5);
    bmp.saveBMP("D:\\CLionProjects\\dop\\barbara5.bmp");

    bmp.setRGB(rgbBit6);
    bmp.saveBMP("D:\\CLionProjects\\dop\\barbara6.bmp");

    bmp.setRGB(rgbBit7);
    bmp.saveBMP("D:\\CLionProjects\\dop\\barbara7.bmp");

}


double BMP::calcCorrelationBitPlane(std::vector<std::vector<BYTE>>plane1, std::vector<std::vector<BYTE>> plane2) {
    double MA = 0;
    double tmp_y = 0;
    int H = info_header.biHeight;
    int W = info_header.biWidth;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            plane1[i][j] == 0 ? tmp_y = 0 : tmp_y = 255;
            MA += tmp_y;
        }
    }
    MA /= W * H;

    double MB = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            plane2[i][j] == 0 ? tmp_y = 0 : tmp_y = 255;
            MB += tmp_y;
        }
    }
    MB /= W * H;

    double sigmaA = 0;
    double tmp = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            plane1[i][j] == 0 ? tmp_y = 0 : tmp_y = 255;
            tmp += pow(tmp_y - MA, 2);
        }
    }
    sigmaA = sqrt(tmp / ((H * W) - 1));


    double sigmaB = 0;
    tmp = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            plane2[i][j] == 0 ? tmp_y = 0 : tmp_y = 255;
            tmp += pow(tmp_y - MB, 2);
        }
    }
    sigmaB = sqrt(tmp / ((H * W) - 1));

    double M_AB = 0;
    double tmp_y2 = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            plane1[i][j] == 0 ? tmp_y = 0 : tmp_y = 255;
            plane2[i][j] == 0 ? tmp_y2 = 0 : tmp_y2 = 255;
            M_AB += (tmp_y - MA) * (tmp_y2 - MB);
        }
    }
    M_AB /= W * H;
    double sigmaAB = sigmaA * sigmaB;
    return M_AB / sigmaAB;
}


void BMP::rgbBit(std::vector<std::vector<BYTE>> bitPlane, std::vector<std::vector<RGBTRIPLE>>&rgbB){
    for (int i = 0; i < info_header.biHeight; i++) {
        std::vector<RGBTRIPLE> buf;
        for (int j = 0; j < info_header.biWidth; j++) {
            RGBTRIPLE rgbtriple;
            if(bitPlane[i][j] == 1){
                rgbtriple.rgbBlue = 0;
                rgbtriple.rgbGreen = 0;
                rgbtriple.rgbRed = 0;
            } else{
                rgbtriple.rgbBlue = 255;
                rgbtriple.rgbGreen = 255;
                rgbtriple.rgbRed = 255;
            }
            buf.push_back(rgbtriple);
        }
        rgbB.push_back(buf);
    }
}

void BMP::setRGB(std::vector<std::vector<RGBTRIPLE>> rgbB) {
    this->rgb = rgbB;
}


int BMP::mod(int a, int b){
    if(a < 0){
        a %= b;
        if(a < 0){
            a += b;
        }
    }else{
        a %= b;
    }
    return a;
}

double BMP::correlationCofRGB(std::vector<std::vector<RGBTRIPLE>> rgb, std::vector<std::vector<RGBTRIPLE>> rgbB) {
    int width = static_cast<int>(rgb[0].size());
    int height = static_cast<int>(rgb.size());

    double MA = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            MA += static_cast<double >(rgb[i][j].rgbBlue + rgb[i][j].rgbRed + rgb[i][j].rgbGreen);
        }
    }
    MA /= height * width;

    double MB = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            MB += static_cast<double >(rgbB[i][j].rgbBlue + rgbB[i][j].rgbRed + rgbB[i][j].rgbGreen);
        }
    }
    MB /= height * width;

    double M_all = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            M_all += (static_cast<double >(rgb[i][j].rgbBlue + rgb[i][j].rgbRed + rgb[i][j].rgbGreen) - MA) *
                     (static_cast<double >(rgbB[i][j].rgbBlue + rgbB[i][j].rgbRed + rgbB[i][j].rgbGreen) - MB);
        }
    }
    M_all /= height * width;

    double sigmaA = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            sigmaA += pow(static_cast<double >(rgb[i][j].rgbBlue + rgb[i][j].rgbRed + rgb[i][j].rgbGreen) - MA, 2);
        }
    }
    sigmaA = sqrt(sigmaA / ((height * width) - 1));

    double sigmaB = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            sigmaB += pow(static_cast<double >(rgbB[i][j].rgbBlue + rgbB[i][j].rgbRed + rgbB[i][j].rgbGreen) - MB, 2);
        }
    }
    sigmaB = sqrt(sigmaB / ((height * width) - 1));

    return (M_all / (sigmaA * sigmaB));
}

std::vector<std::vector<RGBTRIPLE>> BMP::onlyBlueRGB(std::vector<std::vector<RGBTRIPLE>> rgb) {
    for (int i = 0; i < rgb.size(); i++) {
        for (int j = 0; j < rgb[0].size(); j++) {
            rgb[i][j].rgbRed = 0;
            rgb[i][j].rgbGreen = 0;
        }
    }
    return rgb;
}


std::vector<std::vector<RGBTRIPLE>> BMP::onlyGreenRGB(std::vector<std::vector<RGBTRIPLE>> rgb) {
    for (int i = 0; i < rgb.size(); i++) {
        for (int j = 0; j < rgb[0].size(); j++) {
            rgb[i][j].rgbBlue = 0;
            rgb[i][j].rgbRed = 0;
        }
    }
    return rgb;
}

std::vector<std::vector<RGBTRIPLE>> BMP::onlyRedRGB(std::vector<std::vector<RGBTRIPLE>> rgb) {
    for (int i = 0; i < rgb.size(); i++) {
        for (int j = 0; j < rgb[0].size(); j++) {
            rgb[i][j].rgbBlue = 0;
            rgb[i][j].rgbGreen = 0;
        }
    }
    return rgb;
}

void BMP::autocor(){
        std::vector<double> rBB_10, rGG_10, rRR_10;
        std::vector<double> rBB_5, rGG_5, rRR_5;
        std::vector<double> rBB0, rGG0, rRR0;
        std::vector<double> rBB5, rGG5, rRR5;
        std::vector<double> rBB10, rGG10, rRR10;
        for (int y = -10; y <= 10; y += 5) {
            for (int x = 0; x < info_header.biWidth / 4; x +=3) {
                std::vector<std::vector<RGBTRIPLE>> tmp1, tmp2;
                for (int i1 = 0, i2 = y; i1 < info_header.biHeight - y, i2 < info_header.biHeight; i1++, i2++) {
                    std::vector<RGBTRIPLE> vec1, vec2;
                    for (int j1 = 0, j2 = x; j1 < info_header.biWidth - x, j2 < info_header.biWidth; j1++, j2++) {
                        vec1.push_back(rgb[mod(i1, info_header.biHeight)][j1]);
                        vec2.push_back(rgb[mod(i2, info_header.biHeight)][j2]);
                    }
                    tmp1.push_back(vec1);
                    tmp2.push_back(vec2);
                }
                switch (y) {
                case -10:
                    rBB_10.push_back(correlationCofRGB(onlyBlueRGB(tmp1), onlyBlueRGB(tmp2)));
                    rGG_10.push_back(correlationCofRGB(onlyGreenRGB(tmp1), onlyGreenRGB(tmp2)));
                    rRR_10.push_back(correlationCofRGB(onlyRedRGB(tmp1), onlyRedRGB(tmp2)));
                    break;
                case -5:
                    rBB_5.push_back(correlationCofRGB(onlyBlueRGB(tmp1), onlyBlueRGB(tmp2)));
                    rGG_5.push_back(correlationCofRGB(onlyGreenRGB(tmp1), onlyGreenRGB(tmp2)));
                    rRR_5.push_back(correlationCofRGB(onlyRedRGB(tmp1), onlyRedRGB(tmp2)));
                    break;
                case 0:
                    rBB0.push_back(correlationCofRGB(onlyBlueRGB(tmp1), onlyBlueRGB(tmp2)));
                    rGG0.push_back(correlationCofRGB(onlyGreenRGB(tmp1), onlyGreenRGB(tmp2)));
                    rRR0.push_back(correlationCofRGB(onlyRedRGB(tmp1), onlyRedRGB(tmp2)));
                    break;
                case 5:
                    rBB5.push_back(correlationCofRGB(onlyBlueRGB(tmp1), onlyBlueRGB(tmp2)));
                    rGG5.push_back(correlationCofRGB(onlyGreenRGB(tmp1), onlyGreenRGB(tmp2)));
                    rRR5.push_back(correlationCofRGB(onlyRedRGB(tmp1), onlyRedRGB(tmp2)));
                    break;
                case 10:
                    rBB10.push_back(correlationCofRGB(onlyBlueRGB(tmp1), onlyBlueRGB(tmp2)));
                    rGG10.push_back(correlationCofRGB(onlyGreenRGB(tmp1), onlyGreenRGB(tmp2)));
                    rRR10.push_back(correlationCofRGB(onlyRedRGB(tmp1), onlyRedRGB(tmp2)));
                    break;
            }
        }
        }

        std::ofstream oFilerBB("D:\\CLionProjects\\4\\blue.txt", std::ios_base::out);
        std::ofstream oFilerGG("D:\\CLionProjects\\4\\red.txt", std::ios_base::out);
        std::ofstream oFilerRR("D:\\CLionProjects\\4\\green.txt", std::ios_base::out);


        for (int y = -10; y <= 10; y += 5) {
            for (int j = 0; j < (info_header.biWidth / 4); j += 3) {
                switch (y) {
                    case -10:
                        oFilerBB << std::fixed << std::setprecision(9) << rBB_10[j/3] << '\t';
                        oFilerGG << std::fixed << std::setprecision(9) << rGG_10[j/3] << '\t';
                        oFilerRR << std::fixed << std::setprecision(9) << rRR_10[j/3] << '\t';
                        break;
                    case -5:
                        oFilerBB << std::fixed << std::setprecision(9) << rBB_5[j/3] << '\t';
                        oFilerGG << std::fixed << std::setprecision(9) << rGG_5[j/3] << '\t';
                        oFilerRR << std::fixed << std::setprecision(9) << rRR_5[j/3] << '\t';
                        break;
                    case 0:
                        oFilerBB << std::fixed << std::setprecision(9) << rBB0[j/3] << '\t';
                        oFilerGG << std::fixed << std::setprecision(9) << rGG0[j/3] << '\t';
                        oFilerRR << std::fixed << std::setprecision(9) << rRR0[j/3] << '\t';
                        break;
                    case 5:
                        oFilerBB << std::fixed << std::setprecision(9) << rBB5[j/3] << '\t';
                        oFilerGG << std::fixed << std::setprecision(9) << rGG5[j/3] << '\t';
                        oFilerRR << std::fixed << std::setprecision(9) << rRR5[j/3] << '\t';
                        break;
                    case 10:
                        oFilerBB << std::fixed << std::setprecision(9) << rBB10[j/3] << '\t';
                        oFilerGG << std::fixed << std::setprecision(9) << rGG10[j/3] << '\t';
                        oFilerRR << std::fixed << std::setprecision(9) << rRR10[j/3] << '\t';
                        break;
                }
            }
            oFilerBB << std::endl;
            oFilerGG << std::endl;
            oFilerRR << std::endl;

        }
        oFilerBB.close();
        oFilerGG.close();
        oFilerRR.close();
}


