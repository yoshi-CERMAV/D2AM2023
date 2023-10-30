#include "extract_scan.h"
#include "apply_poni1.hpp"

int main(int argc, char *argv[])
{
   extract_scan scan(578, 960);
   scan.read_fmts("D5fmts");
   scan.show_fmts();
   scan.init();
   extract_scan scanWOS(1156, 600);
   scanWOS.read_fmts("WOSfmts");
   scanWOS.show_fmts();
   scanWOS.init();
//   apply_poni theApplyPoniD5 = apply_poni("/Users/yoshi/analysis/D2AM/D5-NewServ_5Nov22.h5", 
//                "/Users/yoshi/analysis/D2AM/2023/D5_0007.poni");
//   apply_poni theApplyPoniWOS = apply_poni("/Users/yoshi/analysis/D2AM/WOS-NewServ_5Nov22.h5", 
//                "/Users/yoshi/analysis/D2AM/2023/0006b.poni");


   int i, j;
   double scale;
   cin >> i >> j ;
   scan.read_data(i);
   scan.read_bg(j);
   scan.show();
   scan.set_scale(scale);
   scale = theApplyPoniD5.fit(0.34, 0.35, scan.get_clean_data(), 
	scan.get_clean_bg(), scan.get_flat(), scan.get_flat_delta());
    cout << "scale ="<<scale<<endl;
   scan.rescale(scale);
   scanWOS.read_data(i);
   scanWOS.read_bg(j);
   scanWOS.set_scale(scale);
   scanWOS.show();
   CImgDisplay **displays = scan.get_display(); 
   CImgDisplay **displays1 = scanWOS.get_display();
   theApplyPoniD5.integrate(0.005, 0.625, 0.005, scan.get_bg(), scan.get_data(), scan.get_flat(), 
		scale, "/Users/yoshi/temp/test_integrate.dat");
   theApplyPoniWOS.integrate(0.45, 5., 0.01, scanWOS.get_bg(), scanWOS.get_data(), scanWOS.get_flat(), 
		scale, "/Users/yoshi/temp/test_integrate_WOS.dat");
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
}
