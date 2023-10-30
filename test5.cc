#include "extract_scan.h"
#include "apply_poni1.hpp"

int main(int argc, char *argv[])
{
   extract_scan scan(578, 960);
   scan.read_fmts("D5fmts");
   scan.show_fmts();
   scan.init();
   apply_poni theApplyPoniD5 = apply_poni("D5poni");

   extract_scan scanWOS(1156, 600);
   scanWOS.read_fmts("WOSfmts");
   scanWOS.show_fmts();
   scanWOS.init();
   apply_poni theApplyPoniWOS = apply_poni("WOSponi");
   theApplyPoniWOS.set_weight(scanWOS.get_flat());
   theApplyPoniWOS.add_mask(scanWOS.get_mask());

}
