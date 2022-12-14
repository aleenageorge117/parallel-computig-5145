#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <string.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float getSum(int fId, float x, int intensity )
{
  switch(fId){
      case 1:
      {
        return f1(x, intensity);
      }
      case 2:
      {
        return f2(x, intensity);
      }
      case 3:
      {
        return f3(x, intensity);
      }
      case 4:
      {
        return f4(x, intensity);
      }
  }
  return 0; 
}

double getNumIntegration(int functionid, float a, float b, int n, int intensity, int nbthreads )
{
    float sum=0;
    float commonFact = (b-a)/(float)n;
    float summation;
    #pragma omp parallel for reduction(+: sum) schedule(runtime) num_threads(nbthreads)
    for (int i=0;i<n;i++) 
    {
      summation = a + (((float)i + 0.5) * commonFact);
      sum += getSum( functionid, summation, intensity);
    }
    return (sum*commonFact);
}

int main (int argc, char* argv[]) {
  //forces openmp to create the threads beforehand
  #pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  //insert code here

  float a, b,result;
   
  int function_id, intensity, nbthreads,n;
  function_id = atoi(argv[1]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  string kind = argv[7];  

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  if (kind.compare("static")==0)
  {
    omp_set_schedule(omp_sched_static,-1);
  }
  else if (kind.compare("dynamic")==0){
    omp_set_schedule(omp_sched_dynamic,-1);
  }
  else if (kind.compare("guided")==0){
    omp_set_schedule(omp_sched_guided,-1);
  }

  result = getNumIntegration(function_id, a, b, n, intensity, nbthreads);
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapased_seconds = end-start;

  cout<<result; 

  std::cerr<<elapased_seconds.count()<<std::endl;

  return 0;
}