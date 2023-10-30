//
//  extract_scan.h
//  D2AM2023
//
//  Created by yoshi on 25/07/2023.
//

#ifndef extract_scan_h
#define extract_scan_h
#include "Savitzky_Golay_2d.h"
#include "CImg.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <H5Cpp.h>
#include "check_mask.hpp"
#include <algorithm>
using namespace std;
using namespace cimg_library;
using namespace H5;
const char green[] = {0, static_cast<char>(255), 0};
const char red[] = {static_cast<char>(255),0,  0};
const char white[] = {static_cast<char>(255),static_cast<char>(255),static_cast<char>(255)};


class show_data{
public:
    show_data(size_t dim0_, size_t dim1_, double *data_, const char title[]){
        dim0 = dim0_;
        dim1 = dim1_;
        image_size = dim0*dim1;
        dim0a = dim0/2;
        dim1a = dim1/2;
        data= data_;
        temp = new double[image_size];
        disp = new CImgDisplay((int)dim0a, (int)dim1a ,title, 0);
        img = new CImg<unsigned char>((int)dim0a,(int)dim1a,1,3);
    }
    void get_contrast(){
        size_t n0 = image_size * 0.1;
        size_t n1 = image_size * 0.9;
        memcpy(temp, data, sizeof(double)*image_size);
        nth_element(temp, temp+n0, temp+image_size);
        nth_element(temp+n0, temp+n1, temp+image_size);
        bottom = temp[n0];
        top = temp[n1];
        contrast = 255/(top-bottom);
    }
    void show(){
        get_contrast();
        fill();
        char message[255];
        snprintf(message,255, "max = %5.3f min=%5.3f", top, bottom);
        img->draw_text(10, 5, message, red, green, .8, 22).display(*disp);
    }
    void show(double scale){
        get_contrast();
        fill();
        char message[255];
        snprintf(message,255, "scale = %5.3f max = %5.3f min=%5.3f", scale, top, bottom);
        img->draw_text(10, 5, message, red, green, .8, 22).display(*disp);
    }
    CImgDisplay *get_disp(){
        return disp;
    }
    void fill()
    {
        unsigned char *cptr = *img;
        double *ptr = data;
        int step = 2;
        size_t image_size_a = dim1a*dim0a;
        unsigned char *cptr1 = cptr  + image_size_a;
        unsigned char *cptr2 = cptr1 + image_size_a;
        for(int j = 0; j < dim1a; j++, ptr += dim0*step){
            double *ptr1 = ptr;
            for(int i = 0; i < dim0a; i++, ptr1+=step){
                float x = (*ptr1 -
                           bottom) * contrast;
                if (x<0) x= 0;
                if (x>255) x= 255;
                unsigned char c = x;
                *(cptr++) = c;
                *(cptr1++) = c;
                *(cptr2++) = c;
            }
        }
    }
protected:
    double *data;
    double *temp;
    CImgDisplay *disp;
    CImg<unsigned char> *img;
    double contrast;
    double bottom;
    double top;
    size_t dim0a, dim1a;
    size_t dim0, dim1;
    size_t image_size;
};

class extract_scan{
public:
    extract_scan(size_t dim0_, size_t dim1_)
    {
        dim0 = dim0_;
        dim1 = dim1_;
        image_size = dim0*dim1;
        data = new int [image_size];
        bg = new int [image_size];
        
        subtracted = new double[image_size];
        flat = new double[image_size*2];
        flat_delta = flat+image_size;
        filtered = new double[image_size];
        temp = new double[image_size];
        clean_data = new double[image_size];
        clean_bg = new double[image_size];
        clean_subtracted = new double[image_size];
        mask = new char[image_size];
        filter = new Filter2d (5, 5, 0, 3, dim0, dim1);
    }
    ~extract_scan(){}
    
    void init()
    {
        read_flat();
        read_mask();
    }
    void read_string(istream &fi, char *name){
        string s;
        getline(fi, s);
        strncpy(name, s.c_str(), s.length());
        name[s.length()]='\0';
    }
    void show_fmts(){
        cout << "path to file = \""<<file_path<<"\"" <<endl;
        cout << "path to bg file = \""<<bg_file_path<<"\""<<endl;
        cout << "format: \""<<fmt<<"\""<<endl;
        cout << "format for pm\""<<fmt_pm<<"\""<<endl;
        cout << "path to mask = \""<<mask_file<<"\""<<endl;
        cout << "path to flat = \""<<flat_file<<"\""<<endl;
        
    }
    void read_fmts(const char filename[]){
        ifstream fi(filename);
        read_string(fi, file_path);
        read_string(fi, bg_file_path);
        read_string(fi, fmt);
        read_string(fi, fmt_pm);
        read_string(fi, mask_file);
        read_string(fi, flat_file);
    }
    void read_data(int i){
        read_data(file_path, fmt, i, data, fmt_pm, pm);
    }
    void read_bg(int i){
        read_data(bg_file_path, fmt, i, bg, fmt_pm, pm_bg);
    }
    void set_scale(double s){scale = s;}
    void read_flat(){
        read(flat_file, flat, 16*image_size);
    }
    void read_mask(){
        read(mask_file, mask, image_size);
        check = new check_mask(mask, dim0, dim1);
    }
    int read_data(const char file_name[], char fmt[], int i, int *data, char fmt_pm[], double &pm3)
    {
        current_image_number = i;
        char name [255];
        cout <<"reading "<<file_name <<endl;
        H5File file(file_name, H5F_ACC_RDONLY);
        cout <<"opened"<<endl;
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
    
    int read(const char filename[], void *dat, size_t size)
    {
        ifstream fi(filename);
        if(!fi) cerr << filename << "does not exist"<<endl;
        fi.read(reinterpret_cast<char *>(dat), size);
        return 0;
    }
    
    void copy_data(double *dat, int *idat){
        for(int i = 0; i != image_size; i++){
            dat[i] = idat[i];//*flat[i];
        }
    }
    
    void flat_data(double *dat){
        for(int i = 0; i != image_size; i++){
            dat[i] *= flat[i];
        }
    }
    int cook(){
        for(int i = 0; i != image_size; i++){
            subtracted[i] = data[i]- scale * bg[i];
        }
        cook(subtracted, clean_subtracted);
        return 0;
    }
    int cook(double *dat, double *clean_dat){
        flat_data(dat);
        check->get_from_neighbors(dat);
        filter->apply(dat, clean_dat, image_size);
        return 0;
    }
    void rescale(double s){
        scale = s;
        cook();
        if (must_show) show_subtracted->show(s);
    }
    void show_sub(){
        show_subtracted->show(scale);
    }
    void prepare_bg(){
        copy_data(temp, bg);
        cook(temp, clean_bg);
    }
    void prepare(){
        copy_data(temp, data);
        cook(temp, clean_data);
        cook();
    }
    void show(){
        prepare_bg();
        prepare();
        char data_name[255];
        snprintf(data_name,255, "data%d", current_image_number);
        show_dat = new show_data(dim0, dim1, clean_data, data_name);
        show_bg = new show_data(dim0, dim1, clean_bg, "bg");
        must_show = true;
        show_subtracted = new show_data(dim0, dim1, clean_subtracted, "subtracted");
        show_dat->show();
        show_bg->show();
        show_subtracted->show();
        displays[0] = show_dat->get_disp();
        displays[1] = show_bg->get_disp();
        displays[2] = show_subtracted->get_disp();
    }
    void write_idata(char filename[]){
        ofstream fo(filename);
        fo.write(reinterpret_cast<char *>(data), sizeof(int)*image_size);
    }
    void write_ibg(char filename[]){
        ofstream fo(filename);
        fo.write(reinterpret_cast<char *>(bg), sizeof(int)*image_size);
    }
    CImgDisplay **get_display(){return displays;}
    int fill_img(unsigned char *cptr);
    int *get_data(){return data;}
    int *get_bg(){return bg;}
    char *get_mask(){return mask;}
    double get_pm(){return pm;}
    double get_pm_bg(){return pm_bg;}
    double *get_flat(){return flat;}
    double *get_flat_delta(){return flat_delta;}
    double *get_clean_data(){return clean_data;}
    double *get_clean_bg(){return clean_bg;}
protected:
    Filter2d *filter;
    size_t dim0, dim1;
    size_t image_size;
    int *data;
    int *bg;
    int current_image_number;
    double pm;
    double pm_bg;
    double scale;
    bool bg_exists = false;
    bool must_show = false;
    double *subtracted;
    double *flat;
    double *flat_delta;
    double *filtered;
    double *temp;
    double *clean_data;
    double *clean_bg;
    double *clean_subtracted;
    char *mask;
    check_mask *check;
    char file_path[255];
    char bg_file_path[255];
    char fmt[255];
    char fmt_pm[255];
    char mask_file[255];
    char mask_file[255];
    char flat_file[255];
    show_data *show_dat, *show_bg, *show_subtracted;
    CImgDisplay *displays[3];
};

int allocate_data_space();
int read_air_WOS(int i, int *idata, double &pm);
int read_WOS(int i, int *idata, double &pm);
int write_WOS( int i, int *idata);
int read_D5(int i, int *idata, double &pm);
int write_D5(int i, int *idata);
int read_D5(int i, float *fdata);
int read_WOS(int i, float *fdata);
int write_D5(int i, float *data);
int read_WOS_air();
int read_D5_air();
int write_D5_air();
int write_WOS_air();

int read_data(const char file_name[], int i, int *data, char fmt[], double &pm3);
int read_data(const char file_name[], int i, int *data, char fmt[]);
int read_data(const char file_name[], void *data, size_t len);

#endif /* extract_scan_h */
