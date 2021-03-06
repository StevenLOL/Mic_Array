#include <math.h>
#include "fft.h"
#include <stdio.h>



/*
 * FFT Algorithm
 * === Inputs ===
 * x : complex numbers
 * N : nodes of FFT. @N should be power of 2, that is 2^(*)
 * === Output ===
 * the @x contains the result of FFT algorithm, so the original data
 * in @x is destroyed, please store them before using FFT.
 */

int fft(Complex *x, int N)
{
    int i,j,l,k,ip;
    static int M = 0;
    static int le,le2;
    static float sR,sI,tR,tI,uR,uI;

    M = (int)(log(N) / log(2));
	//printf("loop: %d \n", M);
    /*
     * bit reversal sorting
     */
    l = N / 2;
    j = l;
    //BitReverse(x,x,N,M);
    for (i=1; i<=N-2; i++)
	{
        if (i < j)
		{
            tR = x[j].real;
            tI = x[j].imag;
            x[j].real = x[i].real;
            x[j].imag = x[i].imag;
            x[i].real = tR;
            x[i].imag = tI;
        }
        k = l;
        while (k <= j)
		{
            j = j - k;
            k = k / 2;
        }
        j = j + k;
    }

    /*
     * For Loops
     */
    for (l=1; l<=M; l++) {   /* loop for ceil{log2(N)} */
        le = (int)pow(2,l);
        le2 = (int)(le / 2);
        uR = 1;
        uI = 0;
        sR = cos(PI / le2);
        sI = -sin(PI / le2);
        for (j=1; j<=le2; j++) {   /* loop for each sub DFT */
            //jm1 = j - 1;
            for (i=j-1; i<=N-1; i+=le) {  /* loop for each butterfly */
                ip = i + le2;
                tR = x[ip].real * uR - x[ip].imag * uI;
                tI = x[ip].real * uI + x[ip].imag * uR;
                x[ip].real = x[i].real - tR;
                x[ip].imag = x[i].imag - tI;
                x[i].real += tR;
                x[i].imag += tI;
            }  /* Next i */
            tR = uR;
            uR = tR * sR - uI * sI;
            uI = tR * sI + uI *sR;
        } /* Next j */
    } /* Next l */

    return 0;
}

/*
 * Inverse FFT Algorithm
 * === Inputs ===
 * x : complex numbers
 * N : nodes of FFT. @N should be power of 2, that is 2^(*)
 * === Output ===
 * the @x contains the result of FFT algorithm, so the original data
 * in @x is destroyed, please store them before using FFT.
 */
int ifft(Complex *x, int N)
{
    int k = 0;

    for (k=0; k<=N-1; k++) {
        x[k].imag = -x[k].imag;
    }

    fft(x, N);    /* using FFT */

    for (k=0; k<=N-1; k++) {
        x[k].real = x[k].real / N;
        x[k].imag = -x[k].imag / N;
    }

    return 0;
}

#if FFT_RECURSIVE
/* 
   fft_R(v,N):
   [0] If N==1 then return.
   [1] For k = 0 to N/2-1, let ve[k] = v[2*k]
   [2] Compute fft(ve, N/2);
   [3] For k = 0 to N/2-1, let vo[k] = v[2*k+1]
   [4] Compute fft(vo, N/2);
   [5] For m = 0 to N/2-1, do [6] through [9]
   [6]   Let w.re = cos(2*PI*m/N)
   [7]   Let w.im = -sin(2*PI*m/N)
   [8]   Let v[m] = ve[m] + w*vo[m]
   [9]   Let v[m+N/2] = ve[m] - w*vo[m]
 */
void fft_R( Complex *v, int n, Complex *tmp )
{
    if(n>1)  /* otherwise, do nothing and return */
    {			
          int k,m;    
          Complex z, w;
          Complex *vo, *ve;
        
          ve = tmp; 
          vo = tmp+n/2;
          for(k=0; k<n/2; k++) 
          {
            ve[k] = v[2*k];   //even sample
            vo[k] = v[2*k+1]; //odd sample
          }
          
          fft_R( ve, n/2, v );		/* FFT on even-indexed elements of v[] */
          fft_R( vo, n/2, v );		/* FFT on odd-indexed elements of v[] */
          for(m=0; m<n/2; m++)
          {
              w.real = cos(2*PI*m/(float)n);
              w.imag = -sin(2*PI*m/(float)n);
              z.real = w.real*vo[m].real - w.imag*vo[m].imag;	/* Re(w*vo[m]) */
              z.imag = w.real*vo[m].imag + w.imag*vo[m].real;	/* Im(w*vo[m]) */
              v[  m  ].real = ve[m].real + z.real;
              v[  m  ].imag = ve[m].imag + z.imag;
              v[m+n/2].real = ve[m].real - z.real;
              v[m+n/2].imag = ve[m].imag - z.imag;
         }
    }
    return;
}


/* 
   ifft_R(v,N):
   [0] If N==1 then return.
   [1] For k = 0 to N/2-1, let ve[k] = v[2*k]
   [2] Compute ifft(ve, N/2);
   [3] For k = 0 to N/2-1, let vo[k] = v[2*k+1]
   [4] Compute ifft(vo, N/2);
   [5] For m = 0 to N/2-1, do [6] through [9]
   [6]   Let w.re = cos(2*PI*m/N)
   [7]   Let w.im = sin(2*PI*m/N)
   [8]   Let v[m] = ve[m] + w*vo[m]
   [9]   Let v[m+N/2] = ve[m] - w*vo[m]
 */
void ifft_R( Complex *v, int n, Complex *tmp )
{
  if(n>1) /* otherwise, do nothing and return */
  {			
    int k,m;    
	Complex z, w, *vo, *ve;
    ve = tmp; vo = tmp+n/2;
    for(k=0; k<n/2; k++) 
	{
      ve[k] = v[2*k];
      vo[k] = v[2*k+1];
    }
    ifft_R( ve, n/2, v );		/* FFT on even-indexed elements of v[] */
    ifft_R( vo, n/2, v );		/* FFT on odd-indexed elements of v[] */
    for(m=0; m<n/2; m++) {
      w.real = cos(2*PI*m/(float)n);
      w.imag = sin(2*PI*m/(float)n);
      z.real = w.real*vo[m].real - w.imag*vo[m].imag;	 /* Re(w*vo[m]) */
      z.imag = w.real*vo[m].imag + w.imag*vo[m].real;	/* Im(w*vo[m]) */
      v[  m  ].real = ve[m].real + z.real;
      v[  m  ].imag = ve[m].imag + z.imag;
      v[m+n/2].real = ve[m].real - z.real;
      v[m+n/2].imag = ve[m].imag - z.imag;
    }
  }
  return;
}


#endif



int BitReverse(int src, int size)
{
	int tmp = src;
	int des = 0;
	int i;

	for (i=size-1; i>=0; i--)
	{
		des = ((tmp & 0x3) << (i*2)) | des;
		tmp = tmp >> 2;
	}
	return des;
}

void reverse_idx(Complex *in,int log4_N)
{
	int i;
	int N = 1 << (log4_N*2);

	Complex * temp;
	temp = (Complex *)malloc(N * sizeof(Complex));
	if(!temp)
	{
		printf("malloc failed!\n");
		exit(0);
	}

	for(i = 0; i < N;i++)
	{
		int idx;
		idx = BitReverse(i,log4_N);
		temp[idx].real = in[i].real;
		temp[idx].imag = in[i].imag;
	}

	for(i = 0; i < N; i++)
	{
		in[i].real = temp[i].real;
		in[i].imag = temp[i].imag;
	}

	free(temp);
}

void fft_ifft_4_common(Complex *in,Complex * win,int log4_N,int reverse)
{
	int N = (1 << log4_N * 2);
	int i,j,k;
	int span = 1;
	int n = N >> 2;
	float wr,wi;
	int widx;
	Complex temp1,temp2,temp3,temp4;
	int idx1,idx2,idx3,idx4;
	int **cof;

	for(i = 0;i < log4_N; i++)
	{
		for(j = 0; j < n; j++)
		{
			widx = 0;

			idx1 = j * span * 4;
			idx2 = idx1 + span;
			idx3 = idx2 + span;
			idx4 = idx3 + span;
			for(k = 0; k < span; k++)
			{

				temp1.real = in[idx1 + k].real;
				temp1.imag = in[idx1 + k].imag;
				temp2.real = win[widx].real * in[idx2 + k].real - win[widx].imag * in[idx2 + k].imag;
				temp2.imag = win[widx].imag * in[idx2 + k].real + win[widx].real * in[idx2 + k].imag;
				temp3.real = win[widx * 2].real * in[idx3 + k].real -win[widx * 2].imag * in[idx3 + k].imag;
				temp3.imag = win[widx * 2].imag * in[idx3 + k].real + win[widx * 2].real * in[idx3 + k].imag;
				temp4.real = win[widx * 3].real * in[idx4 + k].real -win[widx * 3].imag * in[idx4 + k].imag;
				temp4.imag = win[widx * 3].imag * in[idx4 + k].real + win[widx * 3].real * in[idx4 + k].imag;

				in[idx1 + k].real = temp1.real + temp3.real;
				in[idx1 + k].imag = temp1.imag + temp3.imag;
				in[idx2 + k].real = temp1.real - temp3.real;
				in[idx2 + k].imag = temp1.imag - temp3.imag;
				in[idx3 + k].real = temp2.real + temp4.real;
				in[idx3 + k].imag = temp2.imag + temp4.imag;
				in[idx4 + k].real = temp2.real - temp4.real;
				in[idx4 + k].imag = temp2.imag - temp4.imag;

				temp1.real = in[idx1 + k].real + in[idx3 + k].real;
				temp1.imag = in[idx1 + k].imag + in[idx3 + k].imag;
				if(reverse == 0)
				{
					temp2.real = in[idx2 + k].real + in[idx4 + k].imag;
					temp2.imag = in[idx2 + k].imag - in[idx4 + k].real;
				}
				else
				{
					temp2.real = in[idx2 + k].real - in[idx4 + k].imag;
					temp2.imag = in[idx2 + k].imag + in[idx4 + k].real;
				}
				temp3.real = in[idx1  +k].real - in[idx3 + k].real;
				temp3.imag = in[idx1 + k].imag - in[idx3 + k].imag;

				if(reverse == 0)
				{
					temp4.real = in[idx2 + k].real - in[idx4 + k].imag;
					temp4.imag = in[idx2 + k].imag + in[idx4 + k].real;
				}
				else
				{
					temp4.real = in[idx2 + k].real + in[idx4 + k].imag;
					temp4.imag = in[idx2 + k].imag - in[idx4 + k].real;
				}

				in[idx1 + k].real = temp1.real;
				in[idx1 + k].imag = temp1.imag;
				in[idx2 + k].real = temp2.real;
				in[idx2 + k].imag = temp2.imag;
				in[idx3 + k].real = temp3.real;
				in[idx3 + k].imag = temp3.imag;
				in[idx4 + k].real = temp4.real;
				in[idx4 + k].imag = temp4.imag;

				widx += n;
			}
		}
		n >>= 2;
		span <<= 2;
	}
}

void fft4(Complex *in,int log4_N)
{
	Complex *win;
	int N = 1 << (log4_N*2);
	int i;

	win = (Complex *)malloc((3*N/4 - 2) * sizeof(Complex));
	if(!win)
	{
		printf("malloc failed!\n");
		exit(0);
	}

	reverse_idx(in,log4_N);

	for(i = 0; i < (3*N/4-2); i++)
	{
		win[i].real = cos(2*pi*i/(float)N);
		win[i].imag = -sin(2*pi*i/(float)N);
	}

	fft_ifft_4_common(in,win,log4_N,0);

	free(win);
}

void ifft4(Complex *in,int log4_N)
{
	int N = 1 << (log4_N * 2);
	Complex *win;
	int i;

	win = (Complex *)malloc((3*N/4 - 2) * sizeof(Complex));
	if(!win)
	{
		printf("malloc failed!\n");
		exit(0);
	}

	reverse_idx(in,log4_N);

	for(i = 0; i < (3*N/4-2); i++)
	{
		win[i].real = cos(2*pi*i/(float)N);
		win[i].imag = sin(2*pi*i/(float)N);
	}

	fft_ifft_4_common(in,win,log4_N,1);

	for(i = 0; i < N; i++)
	{
		in[i].real /= (float)N;
		in[i].imag /= (float)N;
	}

	free(win);
}
