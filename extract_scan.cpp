//
//  main.cpp
//  extract_scan
//
//  Created by yoshi on 25/07/2023.
//
#ifndef EXTRACT_SCAN_H
#define EXTRACT_SCAN_H
#define cimg_use_tiff
#define cimg_use_png
#include "CImg.h"
#include <iostream>
#include <fstream>
#include <png.h>
#define TIFFFF

#ifdef TIFFFF
#include <tiffio.h>
#endif

#include <iostream>
#include <fstream>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <H5Cpp.h>
#include <stdlib.h>
#include "check_mask.hpp"
#include "Savitzky_Golay_2d.h"
#include "extract_scan.h"
using namespace H5;
using namespace std;
using namespace cimg_library;

const int image_size_WOS = 600 * 1156;
const int image_size_D5 = 578 * 960;
static int dim0 = 578;
static int dim1 = 960;
static int dim0a = 289;//578 ;//
static int dim1a = 480;//300;//
static int *WOS_data;
static int *WOS_air;
static int *D5_data;
static float *D5_data_f;
static double *flat;
static char *mask;
static char name[255];
static char outname[255];
static char H5_file[] = "/Users/yoshi/temp/sample/sample_0001/sample_0001.h5";
static char flat_file[] = "/Users/yoshi/temp/flat5D.dat";
static char fmt_D5[] = "/%d.1/measurement/D5";
static char fmt_WOS[] = "/%d.1/measurement/WOS";
static char fmt_pm[] ="/%d.1/instrument/pm3/data";
static char fmt_out[] = "/Users/yoshi/temp/out%d.dat";
static char fmt_outD5[] = "/Users/yoshi/temp/D5out%d.dat";
static char air_file[] ="/Users/yoshi/temp/sample3/sample3_0001/sample3_0001.h5";
static char mask_file[] = "";
static int *D5_air;
static double pm3_air;
static double pm3;
static double max_intensity = 300;
static double *subtracted_data;
static double *filtered_data;
static Filter2d *filterWOS, *filterD5;
static int frame_changed = 1;
double step = 0.01;
double scale =1.0;
double contrast = 1.;
double bottom = 0.;
enum detector_type {WOS, D5};
//const char green[] = {0, static_cast<char>(255), 0};
//const char white[] = {static_cast<char>(255),static_cast<char>(255),static_cast<char>(255)};
int read(const char filename[], void *dat, size_t size)
{
    ifstream fi(filename);
    if(!fi) cerr << filename << "does not exist"<<endl;
    fi.read(reinterpret_cast<char *>(dat), size);
    return 0;
}

int allocate_data_space()
{
    WOS_data = new int[image_size_WOS];
    WOS_air = new int[image_size_WOS];

    D5_data = new int[image_size_D5];
    D5_air = new int[image_size_D5];
    D5_data_f = new float[image_size_D5];
    subtracted_data = new double[image_size_WOS];
    filtered_data = new double[image_size_WOS];
    flat = new double [image_size_WOS];
    mask = new char[image_size_WOS];
    return 0;
}

int read_mask_D5(){
    read("/Users/yoshi/temp/maskD5c.dat", mask, image_size_D5);
    return 0;
}


//    filter = new Filter2d (5, 5, 0, 3, 578, 960);
int read_D5_file(char D5name[], int i, int *data, double &pm){
    H5File file(D5name, H5F_ACC_RDONLY);
    char name[256];
    snprintf(name, 255, fmt_pm, i);
    DataSet dataset1 = file.openDataSet(name);
    DataSpace dataspace1 = dataset1.getSpace();
    dataset1.read(&pm, PredType::IEEE_F64LE, dataspace1, dataspace1);
    snprintf(name, 255, fmt_D5, i);
    DataSet dataset = file.openDataSet(name);
    DataSpace dataspace = dataset.getSpace();
    
    dataset.read(data, PredType::STD_I32LE, dataspace, dataspace);
    return 0;
}

int read_D5_air(){
    read_D5_file(air_file, 38, D5_air, pm3_air);
    cout <<" pm3_air " <<pm3_air<<endl;
    return 0;
}

int write_D5_air()
{
    ofstream fo("/Users/yoshi/temp/air.dat");
    fo.write(reinterpret_cast<char *>(D5_air), sizeof(int) * image_size_D5);
    return 0;
}
int read_WOS_air(){
    read_data(air_file, 38, WOS_air, fmt_WOS, pm3_air);

    cout <<" pm3_air " <<pm3_air<<endl;
    return 0;
}

int write_WOS_air()
{
   ofstream fo("/Users/yoshi/temp/WOS_air.dat");
   fo.write(reinterpret_cast<char *>(WOS_air), sizeof(int) * image_size_WOS);
   return 0;
}

int read_data(const char file_name[], int i, float *data, char fmt[], detector_type t)
{
    int *idata;
    int *airdata;
    switch(t){
        case WOS:
            idata = WOS_data;
            airdata = WOS_air;
            break;
        case D5:
            idata = D5_data;
            airdata = D5_air;
            break;
            
    }
    H5File file(file_name, H5F_ACC_RDONLY);
    snprintf(name, 255, fmt, i);
    DataSet dataset = file.openDataSet(name);
    DataSpace dataspace = dataset.getSpace();
    dataset.read(idata, PredType::STD_I32LE, dataspace, dataspace);
    snprintf(name, 255, fmt_pm, i);
    DataSet dataset1 = file.openDataSet(name);
    DataSpace dataspace1 = dataset1.getSpace();
    dataset1.read(&pm3, PredType::IEEE_F64LE, dataspace1, dataspace1);
//    double scale = pm3/pm3_air;
    
    int rank = dataspace.getSimpleExtentNdims() ;
    hsize_t dims_out[rank];
    const int ndims = dataspace.getSimpleExtentDims( dims_out, NULL);
    size_t len = 1;
    cout << "rank is "<< rank <<" "<<ndims<<endl;
    for(int i = 0; i < rank; i++) {
        cout << dims_out[i]<<endl;
        len *= dims_out[i];
    }

    double scale =  0.75;
    scale = .9;
    cout << " scale " <<scale <<endl;
    for(int i = 0; i != len; i++){data[i] = idata[i] - airdata[i]*scale;}
    return 0;
}

int read_data(const char file_name[], int i, int *data, char fmt[], double &pm3)
{
    H5File file(file_name, H5F_ACC_RDONLY);
    snprintf(name, 255, fmt, i);
    DataSet dataset = file.openDataSet(name);
    DataSpace dataspace = dataset.getSpace();
    dataset.read(data, PredType::STD_I32LE, dataspace, dataspace);
    snprintf(name, 255, fmt_pm, i);
    DataSet dataset1 = file.openDataSet(name);
    DataSpace dataspace1 = dataset1.getSpace();
    dataset1.read(&pm3, PredType::IEEE_F64LE, dataspace1, dataspace1);
    return 0;
}

int read_data(const char filename[], void *data, size_t len)
{
    ifstream fi(filename);
    fi.read(reinterpret_cast<char *>(data), len );
    return 0;
}

int write_data(const char fmt_out[], int i, void *data, int size)
{
    snprintf(outname, 255, fmt_out, i);
    ofstream fo(outname);
    fo.write(reinterpret_cast<char *>(data), size*4);
    return 0;
}

int read_air_WOS(int i, int *idata, double &pm3)
{
    return read_data(air_file, i, idata,fmt_WOS, pm3);
}

int read_WOS(int i, int *idata, double &pm3)
{
    return read_data(H5_file, i, idata,fmt_WOS, pm3);
}

int write_WOS(int i, int *idata)
{
    return write_data(fmt_out, i, idata, image_size_WOS);
}

int read_D5(int i, int *idata, double &pm3)
{
    return read_data(H5_file, i, idata,fmt_D5, pm3);
}

int read_D5(int i, float *fdata)
{
    return read_data(H5_file, i, fdata,fmt_D5, D5);
}

int write_D5(int i, float *fdata)
{
    return write_data(fmt_outD5, i, fdata, image_size_D5);
}

                
int read_WOS(int i, float *fdata)
{
    return read_data(H5_file, i, fdata, fmt_WOS, WOS);
}

int write_D5(int i, int *idata)
{
    return write_data(fmt_outD5, i, idata, image_size_D5);
}
int *get_data(){return WOS_data;}

int copy(float *sptr, unsigned char *cptr, unsigned short max)
{
    size_t s_image_size = image_size_D5/4;
    
    float ratio = 255./max;
    float temp;
    unsigned char *cptr1 = cptr+ s_image_size;
    unsigned char *cptr2 = cptr1+ s_image_size;
    for(int i = 0; i < s_image_size; i++, sptr++){
        *(cptr++) = (temp = *(sptr) *ratio) > 255? 255: temp ;
        *(cptr1++) = (temp > 255) ? 0: temp;
        *(cptr2++) = (temp > 255) ? 0: temp;
    }
    return 0;
}

int max_contrast()
{
    float min = 1e10;
    float max = -1e10;
    int x0 = 250;
    int x1 = 400;
    int y0 = 0;
    int y1 = 400;
    for(int j = y0; j !=y1; j++){
        for(int i = x0; i !=x1; i++){
            int pos = j*dim0+i;
            float tmp = filtered_data[pos];
            if(tmp < min) min = tmp;
            if(tmp > max) max = tmp;
        }
    }
    contrast = 255/(max-min);
    bottom = min;

    return 0;
}

int flat_data(double *data, double *flat, int len)
{
    for(int i = 0; i !=len; i++){
        data[i] *=flat[i];
    }
    return 0;
}

int cook_data(int len, double *data, double scale, int *data0, int *data1)
{
    for(int i = 0; i != len; i++){
        data[i] = data0[i]- scale * data1[i];
    }
    return 0;
}


int cook_data()
{
    for(int i = 0; i != image_size_D5; i++){
        subtracted_data[i] = D5_data[i]- scale * D5_air[i];
        subtracted_data[i] *= flat[i];
    }
    filterD5->apply(subtracted_data, filtered_data, image_size_D5);
    return 0;
}
int cook_data(check_mask &mask)
{
    for(int i = 0; i != image_size_D5; i++){
        subtracted_data[i] = D5_data[i]- scale * D5_air[i];
        subtracted_data[i] *= flat[i];
    }
    mask.get_from_neighbors(subtracted_data);
    filterD5->apply(subtracted_data, filtered_data, image_size_D5);
    return 0;
}

int cook_data(int len, double *data, double scale, int *data0, int *data1,
              check_mask &mask)
{
    for(int i = 0; i != len; i++){
        data[i] = data0[i]- scale * data1[i];
    }
    flat_data(data, flat, len);
    mask.get_from_neighbors(data);
    filterD5->apply(data, filtered_data, len);
    return 0;
}

int fill_img2(unsigned char *cptr, int dim0a, int dim1a, double *data, int dim0, int step)
{
    double *ptr = data;
    size_t image_size = dim1a*dim0a;
    unsigned char *cptr1 = cptr  + image_size;
    unsigned char *cptr2 = cptr1 + image_size;
    for(int j = 0; j < dim1a; j++, ptr += dim0*step){
        double *ptr1 = ptr;
        for(int i = 0; i < dim0a; i++, ptr1+=step){
            float x = (*ptr1 -
                       bottom) * contrast;
            if (x<0) x= x;
            if (x>255) x= 255;
            unsigned char c = x;
            *(cptr++) = c;
            *(cptr1++) = c;
            *(cptr2++) = c;
        }
    }
    return 0;
}
int fill_img2(unsigned char *cptr)
{
    return fill_img2(cptr, dim0a, dim1a, filtered_data, dim0, 2);
}
#ifdef AAA
int fill_img2(unsigned char *cptr)
{
    float ratio = 255./max_intensity;
    float temp;
    size_t image_size = dim1a*dim0a;
    unsigned char *cptr1 = cptr+ image_size;
    unsigned char *cptr2 = cptr1+ image_size;
    for(int j = 0; j < dim1a; j++){
        size_t pos = j * 2 * dim0;
        int *iptr = D5_data+ pos;
        int *bptr = D5_air + pos;
        double *fptr = flat + pos;
        for(int i = 0; i < dim0a; i++, iptr+=2, bptr+=2, fptr+=2){
            float x = (*iptr - scale * *(bptr)) ;
            x *= *fptr;
            x *= contrast;
            x += bottom;
            if(x<0) x= 0;
            if (x>255) x = 255;
            unsigned char temp = x;
            *(cptr++) = x;
            *(cptr1++) = x;
            *(cptr2++) = x;
        }
        
    }
    return 0;
}
#endif
//#define MAIN_
#ifdef MAIN_

int main(int argc, const char * argv[])
{
    allocate_data_space();
    read(flat_file, flat, 8*image_size_D5);
//    read("/Users/yoshi/temp/mask_WOS.raw", mask, image_size_WOS);
    read_mask_D5();
    check_mask check(mask, 578, 960);
//    check_mask check(mask, 1156, 600);
//    read_air();
//    dim0 = 1156;
//    dim1 = 600;
//    dim0a = 578;
//    dim1a = 300;
    int num0 = atoi(argv[1]);
    int num1 = atoi(argv[2]);
    filterD5 = new Filter2d (15, 15, 0, 3, 578, 960);//1156, 600);//
    filterWOS = new Filter2d (15, 15, 0, 3, 1156, 600);//1156, 600);//
    read_data(H5_file, num0, D5_air, fmt_D5, pm3_air);
    read_data(H5_file, num1, D5_data, fmt_D5, pm3);
    scale = pm3 / pm3_air;
    cout << "scale = "<<scale;
    cout << "pm3_air, pm3 = "<<pm3_air <<" "<<pm3<<endl;
    cook_data(image_size_D5, subtracted_data, scale, D5_data, D5_air, check);
//    ofstream fo("/Users/yoshi/temp/tmp.dat");
//    fo.write(reinterpret_cast<char *>(D5_data), sizeof(int)*image_size_D5);
//    fo.write(reinterpret_cast<char *>(subtracted_data), sizeof(double)*image_size_D5);
//    fo.write(reinterpret_cast<char *>(filtered_data), sizeof(double)*image_size_D5);

    cout << pm3<<endl;
//    write_data(fmt_out, 21, D5_data, image_size_D5);
    // insert code here...
    CImgDisplay
    main_disp(dim0a, dim1a ,"Click a point", 0);
    main_disp.move(0,0);
   // 578 * 960
    CImg<unsigned char> img2(dim0a, dim1a,1,3);
    
    while (!main_disp.is_closed() ){
        if (main_disp.is_keyARROWLEFT())  {
            scale += step;//cout << "arrow left"<<endl;
            cook_data(image_size_D5, subtracted_data, scale, D5_data, D5_air, check);
            max_contrast();
            frame_changed = true;
        }
        if (main_disp.is_keyARROWRIGHT())  {
            scale -= step;//cout << "arrow left"<<endl;
            cook_data(image_size_D5, subtracted_data, scale, D5_data, D5_air, check);
            max_contrast();
            frame_changed = true;
        }
        if (main_disp.is_keyARROWUP())  {
            contrast *= 1.2;//cout << "arrow left"<<endl;
            frame_changed = true;
        }
        if (main_disp.is_keyARROWDOWN())  {
            contrast *= 0.8;//cout << "arrow left"<<endl;
            frame_changed = true;
        }
        char message[255];
        snprintf(message,255, "scale = %5.3f", scale);
        fill_img2(img2);
        img2.draw_text(10, 5, message, green, white, .5, 35).display(main_disp);
        cimg::wait(20);

    }
    std::cout << "Hello, World!\n";
    return 0;
}
#endif
#endif
