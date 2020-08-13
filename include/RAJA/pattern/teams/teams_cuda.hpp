/*!
 ******************************************************************************
 *
 * \file
 *
 * \brief   RAJA header file containing user interface for RAJA::Teams::cuda
 *
 ******************************************************************************
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#ifndef RAJA_pattern_teams_cuda_HPP
#define RAJA_pattern_teams_cuda_HPP

#include "RAJA/pattern/teams/teams_core.hpp"
#include "RAJA/policy/cuda/policy.hpp"

namespace RAJA
{

namespace expt
{

template <bool async, int num_threads = 0>
struct cuda_launch_t {
};

template <typename BODY>
__global__ void launch_global_fcn(LaunchContext ctx, BODY body)
{
  body(ctx);
}


template <bool async>
struct LaunchExecute<RAJA::expt::cuda_launch_t<async, 0>> {
  template <typename BODY>
  static void exec(LaunchContext const &ctx, BODY const &body)
  {
    dim3 blocks;
    dim3 threads;

    blocks.x = ctx.teams.value[0];
    blocks.y = ctx.teams.value[1];
    blocks.z = ctx.teams.value[2];

    threads.x = ctx.threads.value[0];
    threads.y = ctx.threads.value[1];
    threads.z = ctx.threads.value[2];
    launch_global_fcn<<<blocks, threads>>>(ctx, body);

    if (!async) {
      cudaDeviceSynchronize();
    }
  }
};


template <typename BODY, int num_threads>
__launch_bounds__(num_threads, 1) __global__
    void launch_global_fcn_fixed(LaunchContext ctx, BODY body)
{
  body(ctx);
}


template <bool async, int nthreads>
struct LaunchExecute<RAJA::expt::cuda_launch_t<async, nthreads>> {
  template <typename BODY>
  static void exec(LaunchContext const &ctx, BODY const &body)
  {
    dim3 blocks;
    dim3 threads;

    blocks.x = ctx.teams.value[0];
    blocks.y = ctx.teams.value[1];
    blocks.z = ctx.teams.value[2];

    threads.x = ctx.threads.value[0];
    threads.y = ctx.threads.value[1];
    threads.z = ctx.threads.value[2];
    launch_global_fcn_fixed<nthreads><<<blocks, threads>>>(ctx, body);

    if (!async) {
      cudaDeviceSynchronize();
    }
  }
};

/*
  CUDA thread loops with block strides
*/

template <typename SEGMENT>
struct LoopExecute<cuda_thread_x_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();

    for (int tx = threadIdx.x; tx < len; tx += blockDim.x) {
      body(*(segment.begin() + tx));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_thread_y_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();

    for (int ty = threadIdx.y; ty < len; ty += blockDim.y) {
      body(*(segment.begin() + ty));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_thread_z_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();

    for (int i = threadIdx.z; i < len; i += blockDim.z) {
      body(*(segment.begin() + i));
    }
  }
};

/*
  CUDA thread direct mappings
*/

template <typename SEGMENT>
struct LoopExecute<cuda_thread_x_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();
    {
      const int tx = threadIdx.x;
      if (tx < len) body(*(segment.begin() + tx));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_thread_y_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();
    {
      const int ty = threadIdx.y;
      if (ty < len) body(*(segment.begin() + ty));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_thread_z_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();
    {
      const int tz = threadIdx.z;
      if (tz < len) body(*(segment.begin() + tz));
    }
  }
};

/*
  CUDA block loops with grid strides
*/
template <typename SEGMENT>
struct LoopExecute<cuda_block_x_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();

    for (int bx = blockIdx.x; bx < len; bx += gridDim.x) {
      body(*(segment.begin() + bx));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_block_y_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();

    for (int by = blockIdx.y; by < len; by += gridDim.y) {
      body(*(segment.begin() + by));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_block_z_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();

    for (int bz = blockIdx.z; bz < len; bz += gridDim.z) {
      body(*(segment.begin() + bz));
    }
  }
};

/*
  CUDA block direct mappings
*/

template <typename SEGMENT>
struct LoopExecute<cuda_block_x_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();
    {
      const int bx = blockIdx.x;
      if (bx < len) body(*(segment.begin() + bx));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_block_y_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();
    {
      const int by = blockIdx.y;
      if (by < len) body(*(segment.begin() + by));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_block_z_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment,
      BODY const &body)
  {

    const int len = segment.end() - segment.begin();
    {
      const int bz = blockIdx.z;
      if (bz < len) body(*(segment.begin() + bz));
    }
  }
};


// perfectly nested cuda direct policies
struct cuda_block_xy_nested_direct;
struct cuda_block_xyz_nested_direct;

struct cuda_thread_xy_nested_direct;
struct cuda_thread_xyz_nested_direct;


template <typename SEGMENT>
struct LoopExecute<cuda_block_xy_nested_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment0,
      SEGMENT const &segment1,
      BODY const &body)
  {
    const int len1 = segment1.end() - segment1.begin();
    const int len0 = segment0.end() - segment0.begin();
    {
      const int i = blockIdx.x;
      const int j = blockIdx.y;
      if (i < len0 && i < len1)
        body(*(segment0.begin() + i), *(segment1.begin() + j));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_thread_xy_nested_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment0,
      SEGMENT const &segment1,
      BODY const &body)
  {
    const int len1 = segment1.end() - segment1.begin();
    const int len0 = segment0.end() - segment0.begin();
    {
      const int i = threadIdx.x;
      const int j = threadIdx.y;
      if (i < len0 && j < len1)
        body(*(segment0.begin() + i), *(segment1.begin() + j));
    }
  }
};


template <typename SEGMENT>
struct LoopExecute<cuda_block_xyz_nested_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment0,
      SEGMENT const &segment1,
      SEGMENT const &segment2,
      BODY const &body)
  {
    const int len2 = segment2.end() - segment2.begin();
    const int len1 = segment1.end() - segment1.begin();
    const int len0 = segment0.end() - segment0.begin();
    {
      const int i = blockIdx.x;
      const int j = blockIdx.y;
      const int k = blockIdx.z;
      if (i < len0 && j < len1 && k < len2)
        body(*(segment0.begin() + i),
             *(segment1.begin() + j),
             *(segment2.begin() + k));
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_thread_xyz_nested_direct, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment0,
      SEGMENT const &segment1,
      SEGMENT const &segment2,
      BODY const &body)
  {
    const int len2 = segment2.end() - segment2.begin();
    const int len1 = segment1.end() - segment1.begin();
    const int len0 = segment0.end() - segment0.begin();
    {
      const int i = threadIdx.x;
      const int j = threadIdx.y;
      const int k = threadIdx.z;
      if (i < len0 && j < len1 && k < len2)
        body(*(segment0.begin() + i),
             *(segment1.begin() + j),
             *(segment2.begin() + k));
    }
  }
};


// perfectly nested cuda loop policies
struct cuda_block_xy_nested_loop;
struct cuda_block_xyz_nested_loop;

struct cuda_thread_xy_nested_loop;
struct cuda_thread_xyz_nested_loop;

template <typename SEGMENT>
struct LoopExecute<cuda_block_xy_nested_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment0,
      SEGMENT const &segment1,
      BODY const &body)
  {
    const int len1 = segment1.end() - segment1.begin();
    const int len0 = segment0.end() - segment0.begin();
    {
      for (int by = blockIdx.y; by < len1; by += gridDim.y) {
        for (int bx = blockIdx.x; bx < len0; bx += gridDim.x) {
          body(*(segment0.begin() + bx), *(segment1.begin() + by));
        }
      }
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_thread_xy_nested_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment0,
      SEGMENT const &segment1,
      BODY const &body)
  {
    const int len1 = segment1.end() - segment1.begin();
    const int len0 = segment0.end() - segment0.begin();
    {
      for (int ty = threadIdx.y; ty < len1; ty += blockDim.y) {
        for (int tx = threadIdx.x; tx < len0; tx += blockDim.x) {
          body(*(segment0.begin() + tx), *(segment1.begin() + ty));
        }
      }
    }
  }
};


template <typename SEGMENT>
struct LoopExecute<cuda_block_xyz_nested_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment0,
      SEGMENT const &segment1,
      SEGMENT const &segment2,
      BODY const &body)
  {
    const int len2 = segment2.end() - segment2.begin();
    const int len1 = segment1.end() - segment1.begin();
    const int len0 = segment0.end() - segment0.begin();

    for (int bz = blockIdx.z; bz < len2; bz += gridDim.z) {
      for (int by = blockIdx.y; by < len1; by += gridDim.y) {
        for (int bx = blockIdx.x; bx < len0; bx += gridDim.x) {
          body(*(segment0.begin() + bx),
               *(segment1.begin() + by),
               *(segment2.begin() + bz));
        }
      }
    }
  }
};

template <typename SEGMENT>
struct LoopExecute<cuda_thread_xyz_nested_loop, SEGMENT> {

  template <typename BODY>
  static RAJA_INLINE RAJA_DEVICE void exec(
      LaunchContext const RAJA_UNUSED_ARG(&ctx),
      SEGMENT const &segment0,
      SEGMENT const &segment1,
      SEGMENT const &segment2,
      BODY const &body)
  {
    const int len2 = segment2.end() - segment2.begin();
    const int len1 = segment1.end() - segment1.begin();
    const int len0 = segment0.end() - segment0.begin();

    for (int bz = threadIdx.z; bz < len2; bz += blockDim.z) {
      for (int by = threadIdx.y; by < len1; by += blockDim.y) {
        for (int bx = threadIdx.x; bx < len0; bx += blockDim.x) {
          body(*(segment0.begin() + bx),
               *(segment1.begin() + by),
               *(segment2.begin() + bz));
        }
      }
    }
  }
};

}  // namespace expt

}  // namespace RAJA
#endif
