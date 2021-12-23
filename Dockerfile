###############################################################################
# Copyright (c) 2016-21, Lawrence Livermore National Security, LLC
# and RAJA project contributors. See the RAJA/LICENSE file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
###############################################################################

FROM ghcr.io/rse-ops/gcc-ubuntu-20.04:gcc-7.3.0 AS gcc7
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN cmake -DCMAKE_CXX_COMPILER=g++ -DRAJA_ENABLE_WARNINGS=On -DRAJA_ENABLE_TBB=On -DRAJA_DEPRECATED_TESTS=On .. && \
    make -j 6 &&\
    ctest -T test --output-on-failure

FROM ghcr.io/rse-ops/gcc-ubuntu-20.04:gcc-8.1.0 AS gcc8
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN cmake -DCMAKE_CXX_COMPILER=g++ -DRAJA_ENABLE_WARNINGS=On -DRAJA_ENABLE_WARNINGS_AS_ERRORS=On -DENABLE_COVERAGE=On -DRAJA_ENABLE_TBB=On .. && \
    make -j 6 &&\
    ctest -T test --output-on-failure

FROM ghcr.io/rse-ops/gcc-ubuntu-20.04:gcc-9.4.0 AS gcc9
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN cmake -DCMAKE_CXX_COMPILER=g++ -DRAJA_ENABLE_WARNINGS=On -DRAJA_ENABLE_TBB=On -DRAJA_ENABLE_RUNTIME_PLUGINS=On .. && \
    make -j 6 &&\
    ctest -T test --output-on-failure

FROM ghcr.io/rse-ops/gcc-ubuntu-20.04:gcc-11.2.0 AS gcc11
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN cmake -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_COMPILER=g++ -DRAJA_ENABLE_WARNINGS=On -DRAJA_ENABLE_TBB=On -DRAJA_ENABLE_BOUNDS_CHECK=ON .. && \
    make -j 6 &&\
    ctest -T test --output-on-failure

FROM ghcr.io/rse-ops/clang-ubuntu-20.04:llvm-11.0.0 AS clang11
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN cmake -DCMAKE_CXX_COMPILER=clang++ -DRAJA_ENABLE_TBB=On .. && \
    make -j 6 &&\
    ctest -T test --output-on-failure

FROM ghcr.io/rse-ops/clang-ubuntu-20.04:llvm-11.0.0 AS clang11-debug
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug .. && \
    make -j 6 &&\
    ctest -T test --output-on-failure

FROM ghcr.io/rse-ops/clang-ubuntu-22.04:llvm-13.0.0 AS clang13
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release .. && \
    make -j 6 &&\
    ctest -T test --output-on-failure

# Broken container is built w/ gcc9, which is incompatible with nvcc10 
FROM ghcr.io/rse-ops/cuda-ubuntu-20.04:cuda-10.1.243 AS nvcc10
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN . /opt/spack/share/spack/setup-env.sh && spack load cuda && \
    cmake -DCMAKE_CXX_COMPILER=g++ -DENABLE_CUDA=On -DCMAKE_CUDA_STANDARD=14 -DCMAKE_CUDA_ARCHITECTURES=70 .. && \
    make -j 6

FROM ghcr.io/rse-ops/cuda-ubuntu-20.04:cuda-11.1.1 AS nvcc11
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN . /opt/spack/share/spack/setup-env.sh && spack load cuda && \
    cmake -DCMAKE_CXX_COMPILER=g++ -DENABLE_CUDA=On -DCMAKE_CUDA_STANDARD=14 -DCMAKE_CUDA_ARCHITECTURES=70 .. && \
    make -j 6

FROM ghcr.io/rse-ops/cuda-ubuntu-20.04:cuda-11.1.1 AS nvcc11-debug
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN . /opt/spack/share/spack/setup-env.sh && spack load cuda && \
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++ -DENABLE_CUDA=On -DCMAKE_CUDA_STANDARD=14 -DCMAKE_CUDA_ARCHITECTURES=70 .. && \
    make -j 6

FROM axom/compilers:rocm AS hip
ENV GTEST_COLOR=1
COPY --chown=axom:axom . /home/axom/workspace
WORKDIR /home/axom/workspace
ENV HCC_AMDGPU_TARGET=gfx900
RUN mkdir build && cd build && cmake -DBLT_CXX_STD=c++14 -DROCM_ROOT_DIR=/opt/rocm/include -DHIP_RUNTIME_INCLUDE_DIRS="/opt/rocm/include;/opt/rocm/hip/include" -DENABLE_HIP=On -DENABLE_OPENMP=Off -DENABLE_CUDA=Off -DRAJA_ENABLE_WARNINGS_AS_ERRORS=Off -DHIP_HIPCC_FLAGS=-fPIC ..
RUN cd build && make -j 6

FROM ghcr.io/rse-ops/intel-ubuntu-20.04:intel-2021.2.0 AS sycl
ENV GTEST_COLOR=1
COPY . /home/raja/workspace
WORKDIR /home/raja/workspace/build
RUN /bin/bash -c "source /opt/view/setvars.sh && \
    cmake -DCMAKE_CXX_COMPILER=dpcpp -DENABLE_SYCL=On -DENABLE_OPENMP=Off .. && \
    make -j 6 &&\
    ctest -T test --output-on-failure"
