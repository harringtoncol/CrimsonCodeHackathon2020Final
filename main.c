#include <stdio.h>
#include <stdint.h>
#include <fftw3.h>
#include <math.h>
#define N 1024

void main()
{
  // Launch two instances of FFmpeg, one to read the original WAV
  // file and another to write the modified WAV file. In each case,
  // data passes between this program and FFmpeg through a pipe.
  FILE *pipein;
  FILE *pipeout;
  pipein  = popen("ffmpeg -i /tmp/mozilla_ryan0/12345678.wav -f s16le -ac 1 -", "r");
  pipeout = popen("ffmpeg -y -f s16le -ar 44100 -ac 1 -i - out.wav", "w");
     
  // Read, modify and write one sample at a time
  int16_t sample;
  int count, n = 0;
    
  double in1[N];
  double in2[N];
  fftw_complex  out[N / 2 + 1];
  fftw_plan     p1, p2;
int c = 0;

  p1 = fftw_plan_dft_r2c_1d(N, in1, out, FFTW_ESTIMATE);
  p2 = fftw_plan_dft_c2r_1d(N, out, in2, FFTW_ESTIMATE);
  while(1)
  {
    for (int i = 0; i < N; ++i)
    {
      count = fread(&sample, 2, 1, pipein); // read one 2-byte sample
      if (count != 1) break;
      in1[i] = (double) sample;
    }
    if (count != 1) break;
   
    fftw_execute(p1);
   




/* 
    double p = 1 / 4.0;
    //p = (p > 3 ? 3 : p < 0.4 ? 0.4 : p);
    if (p > 1)
      for (int i = N / 2; i > 0; --i)
      {
        out[i][0] = out[(int)(i / p)][0];
        out[i][1] = out[(int)(i / p)][1];
      }
    else if (p < 1)
      for (int i = 1; i < 2 * N * p; ++i)
      {
        if (i < N / 2 + 1)
        {
          out[(int)(i * p)][0] = out[i][0];
          out[(int)(i * p)][1] = out[i][1];
        }
        else
        {
          out[(int)(i * p)][0] = 0;
          out[(int)(i * p)][1] = 0;
        }
      }
*/





    
    fftw_execute(p2);
    double s = .9999;
    for (int i = 0; i < N - 1; ++i)
    {
      sample = (int16_t) (in2[i] / N);
      ++n;
      if (s > 1)
      {
        if (n - s * (int)(n / s) < 1)
          fwrite(&sample, 2, 1, pipeout);
      }
      else if (s < 1)
      {
        for (int i = 0; c < n/s; ++i, ++c)
          fwrite(&sample, 2, 1, pipeout);
      }
      else
        fwrite(&sample, 2, 1, pipeout);
    }
  }
  fftw_destroy_plan(p1);
  fftw_destroy_plan(p2);

  // Close input and output pipes
  pclose(pipein);    
  pclose(pipeout);

}
