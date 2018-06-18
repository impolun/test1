#include <conio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>

using namespace std;

// filter class
class ButterworthFilter
{
  public:
    ButterworthFilter(int a[], int b[], int q_factor)
    {
      for (int i = 0; i <= 12; i++)
      {
        coff_a[i] = a[i];
        coff_b[i] = b[i];
      }
      qf = q_factor;
      Reset();
    }

    void Reset()
    {
      for (int i = 0; i <= 12; i++)
      { 
        x[i] = 0; 
        y[i] = 0;
      }
    }

    int Filter(int in)
    {
      // compute output
      int upscaled_out = 0;
      upscaled_out += (int)coff_b[0] * (int)in;
      for (int i = 1; i <= 12; i++)
      {
        upscaled_out += (int)coff_b[i] * (int)x[i];
        upscaled_out -= (int)coff_a[i] * (int)y[i];
      }

      // scale down
      int out = upscaled_out >> qf;
      // update delay wires
      for (int i = 12; i > 1; i--) { x[i] = x[i-1]; }
      for (int i = 12; i > 1; i--) { y[i] = y[i-1]; }
      x[1] = in; 
      y[1] = out;     
      return out;
    }

  private:
    // delay wires
    int x[13]; // here x[i] represents input[n - i]
    int y[13]; // here y[i] represents output[n - i]

    // filter coefficients
    int coff_a[13];
    int coff_b[13];

    // Q scaling factor
    int qf;
};

int main()
{
  // float filter coefficients, generated by MATLAB program
  // sampling frequency = 48kHz
  // Stopping edges = 5kHz, 7kHz
  float a_floats[] = {1.000000, -7.837476, 30.620141, -77.454899, 140.263174, -190.725578, 
                      199.258637, -161.032630, 99.987174, -46.615556, 15.558375, -3.362259, 0.362331};
  float b_floats[] = {0.601939, -5.151698, 21.982807, -60.698420, 119.892951, -177.664081, 202.094438,
                      -177.664081, 119.892951, -60.698420, 21.982807, -5.151698, 0.601939};
  int a[13], b[13], input, output;
  int i, q_factor = 20; 
  float q = 1 << q_factor; // scaling factor = 2^20
  float x;

  // convert float filter coefficients to integers
  for (int i = 0; i <= 12; i++)
  {
    a[i] = a_floats[i] * q;
    b[i] = b_floats[i] * q;
  }

  // filter object with integer coefficients
  ButterworthFilter filter(a, b, q_factor);

  // load data and output response of filter on console
  // data_2k.txt - 2kHz sine wave, should be passed by filter
  // data_6k.txt - 6kHz, should be suppressed by filter
  ifstream source;  
  source.open("data_2k.txt", ios_base::in);
  unsigned int timeStart = clock();
  for(std::string line; std::getline(source, line); )
  {
      std::istringstream in(line);
      in >> x;
      input = x * q;
      output = filter.Filter(input);
      // print input and output in two columns
      // to match the output with MATLAB, just convert the integral output back to float
      // by dividing with Q-factor (2^20 here) 
      cout << input << " " << output << endl;
  }
  	unsigned int timeEnd = clock();
	unsigned int timeRezult = timeEnd - timeStart;
cout << "Time Work Program = " << timeRezult << "\n";
  getch();
  return 0;
}