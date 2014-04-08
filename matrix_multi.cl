/*__kernel void matrixMultiply(__global float* A, __global float* B, __global float* C, const unsigned int Ndim, const unsigned int Mdim, const unsigned int Pdim)
{
  const size_t idx = get_global_id(0);
  //const size_t idy = get_global_id(1);
    
  __private float a[10];
  //__private float b[100];
  __private float c[10];
  __private float partial_sum;
  
  int i,k;
  
  if(idx < Ndim)
  {
    //printf("%d - %d\n", idx, idy);
  
    for(int i = 0; i < Pdim; i++)
    {
      a[i] = A[i + Pdim*idx];
      //b[i] = B[Mdim*i + idx];
    }
  
    partial_sum = 0;
    for(i=0; i < Mdim; i++)
    {
      for(k=0; k<Pdim; k++)
      {
       partial_sum += a[k]*B[i*Mdim + k];
      }
      c[i] = partial_sum;
      printf("%d\n", c[i]);
    }
  
    for(i=0; i<Mdim; i++)
    {
      C[Mdim*idx + i] = partial_sum;
    }
  }
  
}*/

/*
__kernel void matrixMultiply(__global float* A, __global float* B, __global float* C, const unsigned int Ndim, const unsigned int Mdim, const unsigned int Pdim)
{
  int k,j;
  int i = get_global_id(0);
  float tmp;
  
  if(i < Ndim)
  {
    for(j=0; j<Mdim; j++)
    {
      tmp = 0.0;
      for(k=0; k<Pdim; k++)
	tmp += A[i*Pdim+k]*B[k*Pdim+j];
      C[i*Ndim+j] = tmp;
    }
  }
  
}*/

//-----------------------------------------------------------------------------------------------
// Threads equal to number of rows in C i.e each thread calculates single C row
//-----------------------------------------------------------------------------------------------
__kernel void matrixMultiply(__global float* A,\
			     __global float* B,\
			     __global float* C,\
			     const uint Ndim,\
			     const uint Mdim,\
		             const uint Pdim)
{
  int idx = get_global_id(0);
  float A_private[1000];
  float tmp;
   
  //printf("%d\n",idx); 
  if(idx < Ndim)
  {
    for(int k=0; k<Pdim; k++)
      A_private[k] = A[idx*Pdim + k];
    
    for(int j=0; j<Mdim ;j++)
    {
      tmp = 0.0; 
      for(int k=0; k<Pdim; k++)
      {
	tmp += A_private[k]*B[k*Mdim + j];
      }
      C[idx*Mdim + j] = tmp;
    }
  }  
}

/*
//------------------------------------------------------------------------------------------
// serial kernel code using only single thread  
//------------------------------------------------------------------------------------------
__kernel void matrixMultiply(__global float* A, __global float* B, __global float* C, const unsigned int Ndim, const unsigned int Mdim, const unsigned int Pdim)
{
  int idx = get_global_id(0);
  float partial_sum;
  if(idx == 0)
  {
    for(int i = 0; i < Ndim; i++)
    {
      for(int j = 0; j < Mdim; j++)
      {
	partial_sum = 0;
        for(int k = 0; k < Pdim; k++)
        {
          partial_sum += A[i*Pdim + k]*B[j + Mdim*k];          // dot-product
        }    
        C[j + i*Mdim] = partial_sum;
      } 
    }
  }  
}
*/

/*
//-----------------------------------------------------------------------------------------------
// Used Local Memory
//-----------------------------------------------------------------------------------------------
__kernel void matrixMultiply(__global float* A, __global float* B, __global float* C, const unsigned int Ndim, const unsigned int Mdim, const unsigned int Pdim, __local float* B_temp)
{
  int k,j;
  
  int idx = get_global_id(0);
  int idy = get_local_id(0);
  size_t loc_size = get_local_size(0);
  
  float Awrk[1000];
  float tmp;

  if(idx < Ndim)
  {
    for(k=0; k<Pdim; k++)
      Awrk[k] = A[idx*Pdim + k];
    
    for(j=0; j<Mdim ;j++)
    {
      for(k=0; k<Pdim; k++)
	B_temp[k] = B[k*Mdim + j];
      tmp = 0.0; 
      for(k=0; k<Pdim; k++)
      {
	tmp += Awrk[k]*B_temp[k];
      }
      C[idx*Mdim + j] = tmp;
    }
  } 
  
}*/
