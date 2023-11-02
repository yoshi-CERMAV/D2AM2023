//
//  display_D2AM.cpp
//  D2AM2023
//
//  Created by yoshi on 13/09/2023.
//

#include "display_D2AM.hpp"
#include <hdf5.h>
#include <hdf5_hl.h>
#include <H5Cpp.h>
#include "CImg.h"
#include "Read_D5WOS.h"
#include "apply_poni1.hpp"
#include <string.h>
#include <map>
using namespace H5;
using namespace std;
using namespace cimg_library;
enum mode {D5scale, WOSscale, scan_num, file_name, stepd5, stepWO, stepscan};
map<unsigned int, enum> key_map{
    {cimg::key1, D5scale},
    {cimg::key2, WOSscale},
    {cimg::key3, WOSscale},
    {cimg::key4, WOSscale}
    {cimg::key5, stepd5}
    {cimg::key6, stepWO}
    {cimg::key7, stepscan}
};
int canvas_x = 600;
int canvas_y = 400;
const int image_size_WOS = 600 * 1156;
const int image_size_D5 = 578 * 960;
const unsigned char green[] = {0, 255, 0};
const unsigned char black[] = {0, 0, 0};
const unsigned char white[] = {255,255,255};
const unsigned char red[]   = {255,0,0};
//const unsigned char *col[3];
const unsigned char *the_col;
double D5_scale = 1;
double WOS_scale = 1;
double D5_step = 2;
double WOS_step = 2;
int scan_num_ = 23;
int scan_step = 1;
int file_select = 0;
char message[3][50];
int current_menu = 0;
char fmt[3][50]={"1. D5 scale  %5.3f", "2. WOS scale %5.3f", "3. frame num %3d"};
const char file_path[] = {"/Users/yoshi/temp/"};
const char filenames[][50]= {"sample3/sample3_0001/sample3_0001.h5","sample4/sample4_0001/sample4_0001.h5", "sample/sample_0001/sample_0001.h5"};

class menu_element{
public:
    menu_element(){};
    menu_element(const char name[], int x, int y, CImg<unsigned char > *img_ptr){
        init(name, x, y, img_ptr);
    }
    void init(const char name[], int x, int y, CImg<unsigned char > *img_ptr){
        img = img_ptr;
        strncpy(message, name, 50);
        pos_x = x;
        pos_y = y;
        img->draw_text(pos_x, pos_y, message, white, black, 1, 14);
    }
    void select(){
        selected = true;
        img->draw_text(pos_x, pos_y, message, red, black, 1, 14);
    }
    void unselect(){
        selected = false;
        img->draw_text(pos_x, pos_y, message, white, black, 1, 14);
    }
    void refresh(){
        if(selected)
            img->draw_text(pos_x, pos_y, message, red, black, 1, 14);
        else  img->draw_text(pos_x, pos_y, message,　white, black, 1, 14);
    }
private:
    CImg<unsigned char > *img;
    char message[50];
    int pos_x;
    int pos_y;
    bool selected = false;
};

class value_element{
public:
    value_element(){}
    ~value_element(){}
    void init(int x, int y, const char fmt_[], CImg<unsigned char > * img_ptr)
    {
        pos_x = x;
        pos_y = y;
        img  = img_ptr;
        strncpy(fmt, fmt_, 50);
    }
    template<class T>
    void set(T val){
 //       value = val;
        snprintf(message, 50, fmt, val);
        img->draw_text(pos_x, pos_y, message, white, black, 1, 14);
    }
    void select(int i){
        strncpy(message, file_path,50);
        strncat(message, filenames[i], 50);
    }
private:
    int pos_x;
    int pos_y;
    CImg<unsigned char > *img;
    char message[50];
    char fmt[50];
};

menu_element *menus;
value_element *values;

void init_menu(CImg<unsigned char > * img_ptr)
{
    menus = new menu_element[8];
    menus[0].init("1. D5 scale", 220, 220, img_ptr);
    menus[1].init("2. WOS scale", 220, 240, img_ptr);
    menus[2].init("3. scan_number", 220, 260, img_ptr);
    menus[3].init("4. filename", 220, 280, img_ptr);
    menus[4].init("5. step", 420, 220, img_ptr);
    menus[5].init("6. step", 420, 240, img_ptr);
    menus[6].init("7. step", 420, 260, img_ptr);
    values = new value_element[8];
    values[0].init(300, 220, "%3.1f", img_ptr);
    values[1].init(300, 240, "%3.1f",img_ptr);
    values[2].init(300, 260, "%3d",img_ptr);
    values[3].init(300, 280, "%3d",img_ptr);
    values[4].init(500, 220, "%3.1f",img_ptr);
    values[5].init(500, 240, "%3.1f",img_ptr);
    values[6].init(500, 260, "%3d",img_ptr);
    values[0].set(D5_scale);
    values[1].set(WOS_scale);
    values[2].set(scan_num_);
    values[3].select(file_select);
    values[4].set(D5_step);
    values[5].set(WOS_step);
    values[6].set(scan_step);

}
void select(int i)
{
    if (i != current_menu ){
        menus[current_menu].unselect();
        menus[i].select();
        current_menu= i;
    }
}

template<class T>
int change_value(int i, T val, CImg<unsigned char> &img)
{
    snprintf(message[i], 50, fmt[i], val);
    img.draw_text(220, 220 + i * 20, message[i], red, black, 1, 14);
}
template<class T>
int set_value(int i, T val){
    snprintf(message[i], 50, fmt[i], val);
}
int menu(int select, CImg<unsigned char> &img)
{
    for(int i = 0; i < 3; i++)
    {
        the_col = (i==select) ? red: white;
        img.draw_text(220, 220+i*20, message[i], the_col, black, 1, 14);
    }
}

int fill(unsigned char *c, int x, int y){
    for(int j = 0; j < y; j++){
        for(int i = 0; i < x; i++, c++){
            *c = (i<255)? i:255;
        }
    }
}
int fill1(unsigned char *c, int x, int y){
    for(int j = 0; j < y; j++){
        for(int i = 0; i < x; i++, c++){
            *c = (j<255)?j:255;
        }
    }
}

void run_select(unsigned int key)
{
case cimg::key1:
    if(select != D5scale){
        menus[select].unselect();
        select = D5scale;
        menus[select].select();

}

int main(int argc, char *argv[])
{
    CImg<unsigned char> imgD5(200, 400,1,1);
    CImg<unsigned char> imgWOS(400, 200,1,1);
    fill(imgD5, 200, 400);
    fill1(imgWOS, 400, 200);
    CImgDisplay main_disp(canvas_x, canvas_y ,"D2AM", 0);
    CImg<unsigned char> the_img(600, 400,1,3);
    main_disp.move(0, 0);
    read_h5 reader;
    detector WOS("/Users/yoshi/analysis/D2AM/WOS-NewServ_5Nov22.h5");
    detector D5("/Users/yoshi/analysis/D2AM/D5-NewServ_5Nov22.h5");
    cout <<"initializing"<<endl;
    WOS.init_quick_view(400, 200);
    cout<<"WOS finished "<<endl;
    D5.init_quick_view(200, 400);
    cout << "initialized"<<endl;
    int *dataWOS = new int[image_size_WOS];
    int *dataD5 = new int[image_size_D5];
    reader.read_WOS(23, dataWOS);
    reader.read_D5(23, dataD5);
    cout << "read "<< endl;
    mode select = D5scale;
    mode previous_select = D5scale ;

    bool refresh = true;
    bool select_changed = false;
    set_value(1, WOS_scale);
    set_value(2, scan_num_);
    set_value(0, D5_scale);
    init_menu(& the_img);
    while (!main_disp.is_closed()){
        switch (main_disp.key()) {
            case 0: break;
            case cimg::key1:
                if(select != D5scale){
                    menus[select].unselect();
                    select = D5scale;
                    menus[select].select();
//                    select_changed = true;
                }
                 break;
            case cimg::key2:
                if(select != WOSscale){
                    menus[select].unselect();
                    select = WOSscale;
                    menus[select].select();
//                    select_changed = true;
                }
                break;
            case cimg::key3:
                if(select != scan_num){
                    menus[select].select();
                    select = scan_num;
                    menus[select].select();
                    select_changed = true;
                }
            case cimg::key4:
                if(select != file_name){
                    menus[select].select();
                    select = scan_name;
                    menus[select].select();
                    select_changed = true;
                }
            case cimg::key4:
                if(select != file_name){
                    menus[select].select();
                    select = scan_name;
                    menus[select].select();
                    select_changed = true;
                }
            case cimg::key4:
                if(select != file_name){
                    menus[select].select();
                    select = scan_name;
                    menus[select].select();
                    select_changed = true;
                }

            default: break;
        }

        if (main_disp.is_keyARROWUP()) {
            switch(select){
                case D5scale:
                    D5_scale *= 2;
                    break;
                case WOSscale:
                    WOS_scale *= 2;
                    break;
                case scan_num:
                    scan_num_++;
                    reader.read_WOS(scan_num_, dataWOS);
                    reader.read_D5(scan_num_, dataD5);
                    change_value(scan_num, scan_num_, the_img);
                    break;
                    
            }
            refresh = true;
        }
        if (main_disp.is_keyARROWDOWN()) {
            switch(select){
                case D5scale:  D5_scale /=2; break;
                case WOSscale: WOS_scale /=2; break;
                case scan_num:
                    if(scan_num_){
                        scan_num_--;
                        reader.read_WOS(scan_num_, dataWOS);
                        reader.read_D5(scan_num_, dataD5);
                        change_value(scan_num, scan_num_, the_img);
                    }
                    break;
                case file_name:
                    if(file_select){
                        file_select--;
                        
                    }

             }
            refresh = true;
        }
        if(refresh){
 //           snprintf(message1, 50, "1. D5 scale  %5.3f", D5_scale);
 //           snprintf(message2, 50, "2. WOS scale %5.3f", WOS_scale);
            switch(select){
                case D5scale:
                    change_value(D5scale, D5_scale, the_img); break;
                case WOSscale:
                    change_value(WOSscale, WOS_scale, the_img); break;
                default:
                    break;
            }
            WOS.make_image(imgWOS, dataWOS, 0, WOS_scale);
            D5.make_image(imgD5, dataD5, 0, D5_scale);
            the_img.draw_image(0,0,0,0,imgD5).draw_image(0,0,0,1,imgD5).draw_image(0,0,0,2,imgD5)
                .draw_image(200,0,0,0,imgWOS).draw_image(200,0,0,1,imgWOS).draw_image(200,0,0,2,imgWOS)
 //               .draw_text(220, 220, message1, col, black, 1, 14)
 //               .draw_text(220, 240, message2, col1, black, 1, 14)
                .display(main_disp);
            refresh = false;
        }
        if(select_changed){
//            menu(select, the_img);
            the_img
  //              .draw_text(220, 220, message1, col, black, 1, 14)
  //              .draw_text(220, 240, message2, col1, black, 1, 14)
                .display(main_disp);
            select_changed= false;
        }
        cimg::wait(200);
    }
    
}
