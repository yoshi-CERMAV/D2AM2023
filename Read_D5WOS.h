//
//  Read_D5WOS.h
//  D2AM2023
//
//  Created by yoshi on 13/09/2023.
//

#ifndef Read_D5WOS_h
#define Read_D5WOS_h
#include <hdf5.h>
#include <hdf5_hl.h>
#include <H5Cpp.h>
#include <cstring>
#include <stdlib.h>

using namespace std;
using namespace H5;

class read_h5
{
public:
    read_h5(char filename[]);
    read_h5(){
        strncpy(path_,"/Users/yoshi/temp/putaux/",255);
        strncpy(filename_, "sample3/sample3_0001/sample3_0001.h5",255);
        strncpy(D5fmt_,"/%d.1/measurement/D5", 255);
        strncpy(WOSfmt_,"/%d.1/measurement/WOS", 255);
        char full_path[255];
        strncpy(full_path, path_, 255);
        strncat(full_path, filename_, 255);
        file_ = new H5File(full_path, H5F_ACC_RDONLY);
    }

    void change_path(const char path[])
    {
        strncpy(path_, path, 255);
    }
    void change_file(const char filename[])
    {
        char full_path[255];
        strncpy(filename_,filename, 256);
        strncpy(full_path, path_, 255);
        strncat(full_path, filename_, 255);
        struct stat buf;
        if(!stat(full_path, &buf)){
            file_ = new H5File(full_path, H5F_ACC_RDONLY);
       }
    }
    
    void read_D5(int i, int *data){
        read(i, data, D5fmt_);
    }
    void read_WOS(int i, int *data){
        read(i, data, WOSfmt_);
    }
    int read(int i, int *data, char fmt[]){
        char name [255];
        snprintf(name, 255, fmt, i);
        DataSet dataset = file_->openDataSet(name);
        if( !dataset.getId() )
            {
    //            std::runtime_error( "ReportReaderHDF5: "
    //                                  "Dataset " + name + " not found "
    //                                  "in file: " + file_.getFileName( )));
                return -1;
            }
        DataSpace dataspace = dataset.getSpace();
        dataset.read(data, PredType::STD_I32LE, dataspace, dataspace);
        return 0;
    }
    ~read_h5(){file_->close();}
private:
    char path_[255];
    char filename_[255];
    char D5fmt_[255];
    char WOSfmt_[255];
    H5File *file_;
};

#endif /* Read_D5WOS_h */
