//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

///
/// Source file containing tests for RAJA util merge_sort for cuda gpus
///

#include "test-util-sort.hpp"

#if defined(RAJA_ENABLE_CUDA)
/*
// Reserved for future development.
using CudaMergeSortTypes = Test< camp::cartesian_product<
                                                             CudaMergeSortSorters,
                                                             CudaResourceList,
                                                             SortKeyTypeList,
                                                             SortMaxNListSmall >
                         >::Types;

INSTANTIATE_TYPED_TEST_SUITE_P( CudaTest,
                                SortUnitTest,
                                CudaMergeSortTypes );
*/
#endif

