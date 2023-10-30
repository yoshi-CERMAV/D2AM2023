//
//  check_mask.hpp
//  D2AM2023
//
//  Created by yoshi on 31/07/2023.
//

#ifndef check_mask_hpp
#define check_mask_hpp

#include <stdio.h>
#include <list>
using namespace std;
class check_mask{
public:
    check_mask(char *mask, size_t dim0_, size_t dim1_);
    template <class T>
    int get_from_neighbors(T *data);
protected:
    size_t dim0, dim1;
    size_t dim01, dim11;
    list<int> lis[16];
};
template<class T>
int
check_mask::get_from_neighbors(T *data)
{
   for(int x:lis[1]){
      data[x] = data[x-1];
   }
   for(int x:lis[2]){
      data[x] = data[x+1];
   }
   for(int x:lis[4]){
      data[x] = data[x-578];
   }
   for(int x:lis[8]){
      data[x] = data[x+578];
   }
   for(int x:lis[3]){
      data[x] = 0.5*(data[x - 1] + data[x + 1]);
   }
   for(int x:lis[5]){
      data[x] = 0.5*(data[x - 1] + data[x-578]);
   }
   for(int x:lis[6]){
      data[x] = 0.5*(data[x + 1] + data[x-578]);
   }
   for(int x:lis[9]){
      data[x] = 0.5*(data[x - 1] + data[x+578]);
   }
   for(int x:lis[10]){
      data[x] = 0.5*(data[x + 1] + data[x+578]);
   }
   for(int x:lis[12]){
      data[x] = 0.5*(data[x - 578] + data[x+578]);
   }
   for(int x:lis[7]){
      data[x] = (data[x+1] + data[x-1] +data[x-578])*0.333333333333333333;
   }
   for(int x:lis[11]){
      data[x] = (data[x+1] + data[x-1] +data[x+578])*0.333333333333333333;
   }
   for(int x:lis[13]){
      data[x] = (data[x-1] + data[x-578] +data[x+578])*0.333333333333333333;
   }
   for(int x:lis[14]){
      data[x] = (data[x+1] + data[x-578] +data[x+578])*0.333333333333333333;
   }
   for(int x:lis[15]){
      data[x] = (data[x-1] + data[x+1] + data[x-578] +data[x+578])*0.25;
   }
    return 0;
}

#endif /* check_mask_hpp */
