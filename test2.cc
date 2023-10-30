#include "extract_scan.h"
#include "apply_poni1.hpp"

int main(int argc, char *argv[])
{
   extract_scan scan(578, 960);
   scan.read_fmts("D5fmts");
   scan.show_fmts();
   scan.init();
//   apply_poni theApplyPoniD5 = apply_poni("/Users/yoshi/analysis/D2AM/D5-NewServ_5Nov22.h5", 
//                "/Users/yoshi/analysis/D2AM/2023/D5_0007.poni");
   apply_poni theApplyPoniD5 = apply_poni("D5poni");

   extract_scan scanWOS(1156, 600);
   scanWOS.read_fmts("WOSfmts");
   scanWOS.show_fmts();
   scanWOS.init();
   apply_poni theApplyPoniWOS = apply_poni("WOSponi");
   theApplyPoniWOS.set_weight(scanWOS.get_flat());
   theApplyPoniWOS.add_mask(scanWOS.get_mask());

   int ibg = 23;
   int isample0 = atoi(argv[1]);
   int isample1 = atoi(argv[2]);
   for (int isample = isample0; isample< isample1; isample+=2){
      double scale;
      scan.read_data(isample);
      scan.read_bg(ibg);
      cout <<"pm  "<< scan.get_pm()<<" "<<scan.get_pm_bg()<<endl;
      cout <<scan.get_pm()/scan.get_pm_bg()<<endl;
//   scan.write_idata("23.dat");
//   scan.write_ibg("25.dat");
      scan.show();
      scan.set_scale(scale);
      scanWOS.read_data(isample);
      scanWOS.read_bg(ibg);
//   scale = theApplyPoniD5.fit(0.32, 0.38, scan.get_clean_data(), 
      theApplyPoniD5.add_mask(scan.get_mask());
//   scale = theApplyPoniD5.fit1(0.34, 0.35, scan.get_data(), 
//	scan.get_bg(), scan.get_flat(), scan.get_flat_delta());
      scale = theApplyPoniD5.fit(0.33, 0.37, scan.get_clean_data(), 
         scan.get_clean_bg(), scan.get_flat(), scan.get_flat_delta());
      theApplyPoniD5.set_weight(scan.get_flat());
      scanWOS.set_scale(scale);

      cout << "scale ="<<scale<<endl;
      scan.rescale(scale);
      char filenameD5[255]; 
      char filenameWOS[255]; 
      snprintf(filenameD5, 255, "/Users/yoshi/temp/%04dD5_integrate.dat", isample);
      snprintf(filenameWOS, 255, "/Users/yoshi/temp/%04dWOS_integrate.dat", isample);

      theApplyPoniD5.integrate(0.01, 0.625, 0.005, scan.get_bg(), scan.get_data(),  
		scale, filenameD5);
//0.45, 5., 0.01
      theApplyPoniWOS.integrate(0.45, 4, 0.01, scanWOS.get_bg(), scanWOS.get_data(),  
		scale, filenameWOS);

   }   
#define AAAA
#ifdef AAAA
   CImgDisplay **displays = scan.get_display();
   while(1){
       for(int i = 0; i < 3;i ++){
           if (displays[i]->is_keyARROWLEFT()){
               cout << i <<" arrow left"<<endl; displays[i]->wait(200);}
           if(displays[i]-> button() && displays[i]->mouse_x() >=0 && displays[i]->mouse_y()>=0 ){
               int xm =displays[i]->mouse_x();
               int ym = displays[i]->mouse_y();
               cout << i <<" "<<xm <<" "<<ym<<endl;
               cout <<"q = "<<theApplyPoniD5.get_q(xm*2, ym*2)<<endl;
               displays[i]-> wait(400);
           }
       }
   }
#endif

}
