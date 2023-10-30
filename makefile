test1: test1.cc
	/usr/local/hdf5/bin/h5c++  -o test1 test1.cc -I/Users/yoshi/include -lX11 check_mask.cpp ~/lib/Savitzky_Golay_2d.o -lfftw3 -framework Accelerate -Wno-c++11-extensions

test: test.cc
	/usr/local/hdf5/bin/h5c++  -o test test.cc -I/Users/yoshi/include -lX11 check_mask.cpp ~/lib/Savitzky_Golay_2d.o -lfftw3 -framework Accelerate -Wno-c++11-extensions
test2: test2.cc apply_poni1.hpp
	/usr/local/hdf5/bin/h5c++  -o test2 test2.cc -I/Users/yoshi/include -lX11 check_mask.cpp ~/lib/Savitzky_Golay_2d.o -lfftw3 -framework Accelerate -Wno-c++11-extensions

test3: test3.cc
	/usr/local/hdf5/bin/h5c++  -o test3 test3.cc -I/Users/yoshi/include -lX11 check_mask.cpp ~/lib/Savitzky_Golay_2d.o -lfftw3 -framework Accelerate -Wno-c++11-extensions
test4: test4.cc
	/usr/local/hdf5/bin/h5c++  -o test4 test4.cc -I/Users/yoshi/include -lX11 check_mask.cpp ~/lib/Savitzky_Golay_2d.o -lfftw3 -framework Accelerate -Wno-c++11-extensions

test5: test5.cc apply_poni1.hpp
	/usr/local/hdf5/bin/h5c++  -o test5 test5.cc -I/Users/yoshi/include -lX11 check_mask.cpp ~/lib/Savitzky_Golay_2d.o -lfftw3 -framework Accelerate -Wno-c++11-extensions
