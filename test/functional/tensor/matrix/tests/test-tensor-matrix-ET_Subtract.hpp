//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-22, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#ifndef __TEST_TESNOR_MATRIX_ET_Subtract_HPP__
#define __TEST_TESNOR_MATRIX_ET_Subtract_HPP__

#include<RAJA/RAJA.hpp>

template <typename MATRIX_TYPE>
void ET_SubtractImpl()
{

  using matrix_t = MATRIX_TYPE;
  using policy_t = typename matrix_t::register_policy;
  using element_t = typename matrix_t::element_type;


  static constexpr camp::idx_t N = RAJA::max<camp::idx_t>(matrix_t::s_num_rows, matrix_t::s_num_columns)*2;

  //
  // Allocate Row-Major Data
  //

  // alloc data1

  std::vector<element_t> data1_vec(N*N);
  RAJA::View<element_t, RAJA::Layout<2>> data1_h(data1_vec.data(), N, N);

  element_t *data1_ptr = tensor_malloc<policy_t>(data1_vec);
  RAJA::View<element_t, RAJA::Layout<2>> data1_d(data1_ptr,  N, N);


  // alloc data2

  std::vector<element_t> data2_vec(N*N);
  RAJA::View<element_t, RAJA::Layout<2>> data2_h(data2_vec.data(),  N, N);

  element_t *data2_ptr = tensor_malloc<policy_t>(data2_vec);
  RAJA::View<element_t, RAJA::Layout<2>> data2_d(data2_ptr,  N, N);


  // alloc data3

  std::vector<element_t> data3_vec(N*N);
  RAJA::View<element_t, RAJA::Layout<2>> data3_h(data3_vec.data(),  N, N);

  element_t *data3_ptr = tensor_malloc<policy_t>(data3_vec);
  RAJA::View<element_t, RAJA::Layout<2>> data3_d(data3_ptr,  N, N);



  // Fill data1 and data2
  for(camp::idx_t i = 0;i < N; ++ i){
    for(camp::idx_t j = 0;j < N; ++ j){
      data1_h(i,j) = i*matrix_t::s_num_columns+j;
      data2_h(i,j) = 1+i+j;
    }
  }

  tensor_copy_to_device<policy_t>(data1_ptr, data1_vec);
  tensor_copy_to_device<policy_t>(data2_ptr, data2_vec);


  //
  // Do Operation: Full sum of data1 and data2
  //
  tensor_do<policy_t>([=] RAJA_HOST_DEVICE (){

    auto rows = RAJA::RowIndex<int, matrix_t>::all();
    auto cols = RAJA::ColIndex<int, matrix_t>::all();

    data3_d(cols, rows) = data1_d(rows, cols) - data2_d(cols, rows);

  });

  tensor_copy_to_host<policy_t>(data3_vec, data3_ptr);


  //
  // Check results
  //
  for(camp::idx_t i = 0;i < N; ++ i){
    for(camp::idx_t j = 0;j < N; ++ j){
      ASSERT_SCALAR_EQ(data3_h(j,i), data1_h(i,j)-data2_h(j,i));
//      printf("%d,%d:  %lf, %lf\n", (int)i, (int)j, data1(i,j), data2(i,j));
    }
  }



  //
  // Loop over all possible sub-matrix sizes using the load_*_nm routines
  //
  for(camp::idx_t n_size = 0;n_size <= N; ++ n_size){
    for(camp::idx_t m_size = 0;m_size <= N; ++ m_size){
//      printf("Running %d x %d\n", (int)n_size, (int)m_size);
      //
      // Clear data3
      //
      for(camp::idx_t i = 0;i < N; ++ i){
        for(camp::idx_t j = 0;j < N; ++ j){
          data3_h(j,i) = -1;
        }
      }
      tensor_copy_to_device<policy_t>(data3_ptr, data3_vec);


      //
      // Do Operation: Perform partial sum
      //
      tensor_do<policy_t>([=] RAJA_HOST_DEVICE (){
        // Load data using a View
        auto rows = RAJA::RowIndex<int, matrix_t>::range(0, n_size);
        auto cols = RAJA::ColIndex<int, matrix_t>::range(0, m_size);

        data3_d(cols, rows) = data1_d(rows, cols) - data2_d(cols, rows);
      });

      tensor_copy_to_host<policy_t>(data3_vec, data3_ptr);


      //
      // Check results
      //
      for(camp::idx_t i = 0;i < N; ++ i){
        for(camp::idx_t j = 0;j < N; ++ j){
//          printf("%d,%d:  %lf, %lf\n", (int)i, (int)j, data1(i,j), data2(i,j));
          if(i < n_size && j < m_size){
            ASSERT_SCALAR_EQ(data3_h(j,i), data1_h(i,j)-data2_h(j,i));
          }
          else{
            ASSERT_SCALAR_EQ(element_t(-1), data3_h(j,i));
          }
        }
      }


    }
  }


  //
  // Free data
  //
  tensor_free<policy_t>(data1_ptr);
  tensor_free<policy_t>(data2_ptr);
  tensor_free<policy_t>(data3_ptr);

}



TYPED_TEST_P(TestTensorMatrix, ET_Subtract)
{
  ET_SubtractImpl<TypeParam>();
}


#endif
