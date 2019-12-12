sudo apt-get -yV update
sudo apt-get -yV install build-essential cmake cmake-curses-gui git libtbb2 libtbb-dev
cd
git clone --recurse-submodules --branch=master https://github.com/tfhe/tfhe.git
cd tfhe
mkdir build
cd build
sed -i '79c\    inline void add4(double * res, const double * a, const double * b) {' ../src/libtfhe/fft_processors/spqlios/spqlios-fft-impl.cpp
sed -i '83c\    inline void sub4(double * res, const double * a, const double * b) {' ../src/libtfhe/fft_processors/spqlios/spqlios-fft-impl.cpp
cmake ../src -DENABLE_FFTW=off -DENABLE_NAYUKI_PORTABLE=on -DENABLE_NAYUKI_AVX=on -DENABLE_SPQLIOS_AVX=on -DENABLE_SPQLIOS_FMA=on -DCMAKE_BUILD_TYPE=optim
sudo make install
