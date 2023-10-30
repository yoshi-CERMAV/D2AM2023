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
cout << "initialized"<<endl;
//   apply_poni theApplyPoniD5 = apply_poni("/Users/yoshi/analysis/D2AM/D5-NewServ_5Nov22.h5", 
//                "/Users/yoshi/analysis/D2AM/2023/D5_0007.poni");
   apply_poni theApplyPoniD5 = apply_poni("D5poni");
   theApplyPoniD5.set_weight(scan.get_flat());
cout << "D5poni"<<endl;
//   apply_poni theApplyPoniWOS = apply_poni("/Users/yoshi/analysis/D2AM/WOS-NewServ_5Nov22.h5", 
//                "/Users/yoshi/analysis/D2AM/2023/0006b.poni");
   apply_poni theApplyPoniWOS = apply_poni("WOSponi");
   theApplyPoniWOS.set_weight(scanWOS.get_flat());
cout <<"WOSponi"<<endl;
    for(int i = 480; i !=600; i++) theApplyPoniWOS.mask_line(i);
   theApplyPoniWOS.set_weight(scanWOS.get_flat()); 
   double scale;
   int bgi = 38;
   int sample[] = {13};
//   int sample[] = {889};
//   int sample[] = {789, 793, 797, 801, 809, 
//                   813, 821, 829, 833, 837, 
//                   841, 849, 857, 865, 897};
   scan.read_bg(bgi);
   scan.prepare_bg();
   scanWOS.read_bg(bgi);
   scanWOS.prepare_bg();
   for(int i = 0; i!=1; i++){
   scan.read_data(sample[i]);
   scan.prepare();

   scale = theApplyPoniD5.fit(0.34, 0.35, scan.get_clean_data(), 
   scan.get_clean_bg(), scan.get_flat(), scan.get_flat_delta());
   scale = 0.9;
   cout << "scale ="<<scale<<endl;
   scan.rescale(scale);
   cout <<" rescaled"<<endl;
   scanWOS.read_data(sample[i]);
   cout <<"read WOS"<<endl;
   scanWOS.set_scale(scale);
   scanWOS.prepare();
   cout <<"prepare"<<endl;
   char fileD5[255], fileWOS[255];
   snprintf(fileD5, 255, "/Users/yoshi/temp/%04dD5_integrate.dat", sample[i]);
   snprintf(fileWOS, 255, "/Users/yoshi/temp/%04dWOS_integrate.dat", sample[i]);
   cout << fileD5<<endl;
   cout << fileWOS<<endl;
   theApplyPoniD5.integrate(0.005, 0.625, 0.005, scan.get_bg(), scan.get_data(), 
		scale, fileD5);
   theApplyPoniWOS.integrate(0.45, 5., 0.01, scanWOS.get_bg(), scanWOS.get_data(), 
		scale, fileWOS);
   }
}
