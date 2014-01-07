/* Comment to four1() and its invokation by G.Ivanitsky (9 Nov. 1999)

dataF - a type float array that contains Re and Im parts of input signal and
       whose index starts from 1, as it is usually done in all NRC routines.
       So, dataF[1] = Re(f0)
           dataF[2] = Im(f0)
           dataF[3] = Re(f1)
           dataF[4] = Im(f1)
           ................
Consider the case you have a complex array cbuf
      struct compl { float r; float i; } *cbuf;
that contains input signal and whose index starts from 0 according to normal
C language conventions. This is just the case met in all previous programs
that invoke CFFT. Consider you have a pointer to type float aray
      float *dataF;
Then you may invoke four1() in the following manner:
      dataF = ((float *)cbuf) - 1;
      four1( dataF, n, 1 );
Here we have: unsigned long n - length of input signal (power of 2);
              1 designates forward FFT (-1 is used for inverse transform).
If you previously had a struct complex *spect into which the CFFT placed
a result, you can use it also now by means of the following:
      spect = (struct compl *)(dataF+1);
*/

#include <math.h>
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

void four1(double *dataF, int nn, int isign)
{
        int n,mmax,m,j,istep,i;
        double wtemp,wr,wpr,wpi,wi,theta;
        double tempr,tempi;

        n=nn << 1;
        j=1;
        for (i=1;i<n;i+=2) {
                if (j > i) {
                        SWAP(dataF[j],dataF[i]);
                        SWAP(dataF[j+1],dataF[i+1]);
                }
                m=n >> 1;
                while (m >= 2 && j > m) {
                        j -= m;
                        m >>= 1;
                }
                j += m;
        }
        mmax=2;
        while (n > mmax) {
                istep=mmax << 1;
                theta=isign*(6.28318530717959/mmax);
                wtemp=sin(0.5*theta);
                wpr = -2.0*wtemp*wtemp;
                wpi=sin(theta);
                wr=1.0;
                wi=0.0;
                for (m=1;m<mmax;m+=2) {
                        for (i=m;i<=n;i+=istep) {
                                j=i+mmax;
                                tempr=wr*dataF[j]-wi*dataF[j+1];
                                tempi=wr*dataF[j+1]+wi*dataF[j];
                                dataF[j]=dataF[i]-tempr;
                                dataF[j+1]=dataF[i+1]-tempi;
                                dataF[i] += tempr;
                                dataF[i+1] += tempi;
                        }
                        wr=(wtemp=wr)*wpr-wi*wpi+wr;
                        wi=wi*wpr+wtemp*wpi+wi;
                }
                mmax=istep;
        }
}
#undef SWAP
/* (C) Copr. 1986-92 Numerical Recipes Software 95i)6=X3+9. */
