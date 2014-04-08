#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#include <CL/opencl.h>

using namespace std;

#include "graph.hpp"
#include "error.hpp"

#define ORDER 1000

int main(int argc, char* argv[])
{
  cl_int errNum;
  size_t param_value_ret_size;
  
  cl_platform_id    platformId;
  cl_device_id      device_id;
  cl_context        context;
  cl_command_queue  queue;
  cl_program        program;
  cl_kernel         kernel;
  
  const char options[] = "-cl-std=CL1.1 -cl-mad-enable";   // -Werror

  cl_mem a_in;
  cl_mem b_in;
  cl_mem c_out;
  
  float* A;
  float* B;
  float* C;
  unsigned int Ndim = ORDER, Mdim = ORDER, Pdim = ORDER;
  size_t sizeA, sizeB, sizeC;
  
  clock_t start, end;
  
  Graph net;
  cl_mem graph;
  
    
//---------------------------------------------------------------------------------------  
// Ser up OpenCL Platform  
//---------------------------------------------------------------------------------------  
  errNum = clGetPlatformIDs(1, &platformId, NULL);
  checkErr(errNum, "clGetPlatformIDs");
  
//---------------------------------------------------------------------------------------  
// Get device Ids  
//---------------------------------------------------------------------------------------  
  errNum = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
  checkErr(errNum, "clGetDeviceIDs");
  
//---------------------------------------------------------------------------------------  
// Create context  
//---------------------------------------------------------------------------------------  
  cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platformId, 0};
  context = clCreateContext(properties, 1, &device_id, NULL, NULL, &errNum);
  checkErr(errNum, "clCreateContext");
 
//---------------------------------------------------------------------------------------  
// Create command queue  
//---------------------------------------------------------------------------------------  
  queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &errNum);
  checkErr(errNum, "clCreateCommandQueue");
  
//---------------------------------------------------------------------------------------  
// Set up Matrices  
//---------------------------------------------------------------------------------------  
  sizeA = Ndim*Pdim; sizeB = Mdim*Pdim; sizeC = Ndim*Mdim;
  A = new float[sizeA];
  B = new float[sizeB];
  C = new float[sizeC];
  //A = (float *)malloc(sizeof(float)*sizeA);
  //B = (float *)malloc(sizeof(float)*sizeB);
  //C = (float *)malloc(sizeof(float)*sizeC);
  
  for(int i=0; i<(ORDER*ORDER); i++)
  {
    A[i] = 4.535;
    B[i] = 6.335;
   // C[i] = 0;
  }
  
//---------------------------------------------------------------------------------------  
// Set up buffers   
//---------------------------------------------------------------------------------------  
  a_in = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*sizeA, NULL, &errNum);
  checkErr(errNum, "clCreateBuffer-(1)");
  
  b_in = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*sizeB, NULL, &errNum);
  checkErr(errNum, "clCreateBuffer-(2)");
  
  c_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*sizeC, NULL, &errNum);
  checkErr(errNum, "clCreateBuffer-(3)");
  
  graph = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(net), NULL, &errNum);
  checkErr(errNum, "clCreateBuffer-(4)");

//---------------------------------------------------------------------------------------  
// Create program   
//---------------------------------------------------------------------------------------  
  ifstream srcFile("matrix_multi.cl");
  checkErr(srcFile.is_open() ? CL_SUCCESS : -1, "Reading matrix_multi.cl");

  string scrProg(istreambuf_iterator<char>(srcFile), (istreambuf_iterator<char>()));
  
  const char* scr = scrProg.c_str();
  size_t length = scrProg.length();
  
  program = clCreateProgramWithSource(context, 1, &scr, &length, &errNum);
  checkErr(errNum, "clCreateProgramwWithSource");
  
//---------------------------------------------------------------------------------------  
// Build from program   
//---------------------------------------------------------------------------------------  
  errNum = clBuildProgram(program, 1, &device_id, options, NULL, NULL);
  if(errNum < 0)
  {
    errNum = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &param_value_ret_size);
    checkErr(errNum, "clGetProgramBuildInfo-(1)");
    
    char *buildLog = new char[param_value_ret_size];
    
    errNum = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, param_value_ret_size, buildLog, NULL);
    checkErr(errNum, "clGetProgramBuildInfo-(2)");
    
    cout<<endl<<"Error: In OpenCl C code"<<endl<<buildLog<<endl;
    exit(1);
  }
  
//---------------------------------------------------------------------------------------  
// Create kernel   
//---------------------------------------------------------------------------------------  
  kernel = clCreateKernel(program, "matrixMultiply", &errNum);
  checkErr(errNum, "clCreateKernel(matrixMultiply)");
  
//---------------------------------------------------------------------------------------  
// Set kernel argument  
//---------------------------------------------------------------------------------------  
  errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_in);
  checkErr(errNum, "clSetKernelArg(matrixMultiply - arg(0))");
  
  errNum = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_in);
  checkErr(errNum, "clSetKernelArg(matrixMultiply - arg(1))");
  
  errNum = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_out);
  checkErr(errNum, "clSetKernelArg(matrixMultiply - arg(2))");
  
  errNum = clSetKernelArg(kernel, 3, sizeof(unsigned int), (void *)&Ndim);
  checkErr(errNum, "clSetKernelArg(matrixMultiply - arg(3))");
  
  errNum = clSetKernelArg(kernel, 4, sizeof(unsigned int), (void *)&Mdim);
  checkErr(errNum, "clSetKernelArg(matrixMultiply - arg(4))");
  
  errNum = clSetKernelArg(kernel, 5, sizeof(unsigned int), (void *)&Pdim);
  checkErr(errNum, "clSetKernelArg(matrixMultiply - arg(5))");
  
  //errNum = clSetKernelArg(kernel, 6, sizeof(cl_mem), NULL);
  //checkErr(errNum, "clSetKernelArg(matrixMultiply - arg(6))");
  
//---------------------------------------------------------------------------------------  
// Write input data - Warning Don't write to buffer without initializing matrices
//---------------------------------------------------------------------------------------  
  errNum = clEnqueueWriteBuffer(queue, a_in, CL_TRUE, 0, sizeof(float)*sizeA, A, 0, NULL, NULL);
  checkErr(errNum, "clEnqueueWiteBuffer-(1)");
  
  errNum = clEnqueueWriteBuffer(queue, b_in, CL_TRUE, 0, sizeof(float)*sizeB, B, 0, NULL, NULL);
  checkErr(errNum, "clEnqueueWiteBuffer-(2)");
  
//---------------------------------------------------------------------------------------  
// Enqueue kernel for execution
//---------------------------------------------------------------------------------------  
  cl_event event;
  
  size_t globalSize = Ndim;
  size_t localSize  = 1; 
  errNum = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, 0, &event);
  checkErr(errNum, "clEnqueueNDRangeKernel");  
  
  clFinish(queue);

//---------------------------------------------------------------------------------------  
// Profiling data
//---------------------------------------------------------------------------------------  
  cl_ulong ev_start_time = (cl_ulong)0;
  cl_ulong ev_end_time   = (cl_ulong)0;
  size_t ret_size;
  
  errNum = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
  checkErr(errNum, "clGetEventProfilingInfo-(1)");
  
  errNum = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);
  checkErr(errNum, "clGetEventProfilingInfo-(2)");
  
  
  
  
  
/*  
//---------------------------------------------------------------------------------------  
// Get Work group info
//--------------------------------------------------------------------------------------- 
  cl_ulong private_usage;
  errNum = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(private_usage), &private_usage, NULL);
  checkErr(errNum, "clGetKernelWorkGroupInfo");
  
  cout<<endl<<"Private memory usage : "<<private_usage<<" bytes"<<endl;  
*/  
  

//---------------------------------------------------------------------------------------  
// Read back computed data
//---------------------------------------------------------------------------------------  
  errNum = clEnqueueReadBuffer(queue, c_out, CL_TRUE, 0, sizeof(float)*sizeC, C, 1, &event, NULL);
  checkErr(errNum, "clEnqueueReadBuffer");
  /*
  for(int k=0; k<(Ndim*Mdim); k++)
  {
    cout<<C[k]<<" ";
  }*/
  /*
  for(int k=0; k<Ndim; k++)
  {
    for(int j=0; j<Mdim; j++)
    {
      cout<<C[k*Mdim + j]<<" ";
    }
    cout<<endl;    
  }*/
  
  cl_ulong run_time = ev_end_time - ev_start_time;
  
  cout<<"Run time : "<<run_time<<endl;
  
  
  
clReleaseProgram(program);  
clReleaseKernel(kernel);  
clReleaseMemObject(a_in);  
clReleaseMemObject(b_in);  
clReleaseMemObject(c_out);
clReleaseCommandQueue(queue);
clReleaseContext(context);
  
  
  
  return 0;
}
