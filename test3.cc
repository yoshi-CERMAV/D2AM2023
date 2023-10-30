#include "extract_scan.h"
#include "apply_poni1.hpp"

int main(int argc, char *argv[])
{
   extract_scan scanWOS(1156, 600);
   scanWOS.read_fmts("WOSfmts");
   scanWOS.show_fmts();
   scanWOS.init();
   apply_poni theApplyPoniWOS = apply_poni("/Users/yoshi/analysis/D2AM/WOS-NewServ_5Nov22.h5", 
                "/Users/yoshi/analysis/D2AM/2023/0006b.poni");
   scanWOS.read_data(13);
   cout <<"read WOS"<<endl;
   theApplyPoniWOS.extract_intensity(2.75, 2.76, scanWOS.get_data(), scanWOS.get_flat(),"/Users/yoshi/temp/carbon_azim1.dat");
}
