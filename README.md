# AlphaZeroCpp
A re-implementation of AlphaZeroSimple in C++

## Dependencies

- Ubuntu 18.04
- [Cuda 10]()
- [cuDNN v8.0.3 (August 26th, 2020), for CUDA 10.1](https://developer.nvidia.com/compute/machine-learning/cudnn/secure/8.0.3.33/10.1_20200825/cudnn-10.1-linux-x64-v8.0.3.33.tgz)
  - Download and unzip
  - cd `cuda`
  - `sudo cp include/cudnn.h /usr/local/cuda/include`
  - `sudo cp include/cudnn_version.h /usr/local/cuda/include`
  - `sudo cp lib64/libcudnn* /usr/local/cuda/lib64`
  - `sudo chmod a+r /usr/local/cuda/lib64/libcudnn*`

- [LibTorch 1.6.0](https://download.pytorch.org/libtorch/cu102/libtorch-cxx11-abi-shared-with-deps-1.6.0.zip) must be unzipped to this directory.



## Building

`cd build`
`cmake -DCMAKE_PREFIX_PATH=/home/josh/git/AlphaZeroCpp/libtorch -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda-10.1/ ..`

## Running



