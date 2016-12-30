/*
 *   MIRACL prime number routines - test for and generate prime numbers
 *   mrprime.c
 *
 *   Copyright (c) 1988-1997 Shamus Software Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <miracl.h>

void gprime(_MIPD_ int maxp)
{ /* generate all primes less than maxp into global PRIMES */
    char *sv;
    int pix,i,k,prime;
#ifndef MR_GENERIC_MT
    miracl *mr_mip=get_mip();
#endif
    if (mr_mip->ERNUM) return;
    if (maxp<=0)
    {
        if (mr_mip->PRIMES!=NULL) mr_free(_MIPP_ mr_mip->PRIMES);
        mr_mip->PRIMES=NULL;
        return;
    }

    MR_IN(70)

    if (maxp>=MR_TOOBIG)
    {
         mr_berror(_MIPP_ MR_ERR_OUT_OF_MEMORY);
         MR_OUT
         return;
    }
    if (maxp<1000) maxp=1000;
    maxp=(maxp+1)/2;
    sv=(char *)mr_alloc(_MIPP_ maxp,1);
    if (sv==NULL)
    {
        mr_berror(_MIPP_ MR_ERR_OUT_OF_MEMORY);
        MR_OUT
        return;
    }
    pix=1;
    for (i=0;i<maxp;i++)
        sv[i]=TRUE;
    for (i=0;i<maxp;i++)
    if (sv[i])
    { /* using sieve of Eratosthenes */
        prime=i+i+3;
        for (k=i+prime;k<maxp;k+=prime)
            sv[k]=FALSE;
        pix++;
    }
    if (mr_mip->PRIMES!=NULL) mr_free(_MIPP_ mr_mip->PRIMES);
    mr_mip->PRIMES=(int *)mr_alloc(_MIPP_ pix+2,sizeof(int));
    if (mr_mip->PRIMES==NULL)
    {
        mr_free(_MIPP_ sv);
        mr_berror(_MIPP_ MR_ERR_OUT_OF_MEMORY);
        MR_OUT
        return;
    }
    mr_mip->PRIMES[0]=2;
    pix=1;
    for (i=0;i<maxp;i++)
        if (sv[i]) mr_mip->PRIMES[pix++]=i+i+3;
    mr_mip->PRIMES[pix]=0;
    mr_free(_MIPP_ sv);
    MR_OUT
    return;
}

int trial_division(_MIPD_ big x,big y)
{ 
  /* general purpose trial-division function, works in two modes */
  /* if (x==y) quick test for candidate prime, using trial       *
   * division by the small primes in the instance array PRIMES[] */
  /* returns 0 if definitely not a prime *
   * returns 1 if definitely is  a prime *
   * returns 2 if possibly a prime       */
  /* if x!=y it continues to extract small factors, and returns  *
   * the unfactored part of x in y, or 1 if  x is "smooth",      *
   * i.e. completely factors over the small primes in PRIMES     *
   * In this case the returned value refers to the status of y   */
  /* returns 1 if x was "smooth" (y=1)  *
   * returns 2 if y is possibly a prime */
    int i;
#ifndef MR_GENERIC_MT
    miracl *mr_mip=get_mip();
#endif
    if (mr_mip->ERNUM) return 0;
    if (size(x)<=1) return 0;

    MR_IN(105)

    copy(x,y);

    if (mr_mip->PRIMES==NULL) gprime(_MIPP_ 1000);
    for (i=0;mr_mip->PRIMES[i]!=0;i++)
    { /* test for divisible by first few primes */
        while (subdiv(_MIPP_ y,mr_mip->PRIMES[i],mr_mip->w1)==0)
        { 
            if (x==y)
            {
                MR_OUT
                if (size(mr_mip->w1)==1) return 1;
                else return 0;
            }
            else 
            {
                copy(mr_mip->w1,y);
                if (size(y)==1) 
                {
                    MR_OUT
                    return 1;
                } 
                continue;
            }
        }
        if (size(mr_mip->w1)<=mr_mip->PRIMES[i])
        { /* must be prime */
            if (x!=y) convert(_MIPP_ 1,y);
            MR_OUT
            return 1;
        }
    }
    MR_OUT
    return 2;
}

BOOL isprime(_MIPD_ big x)
{  /*  test for primality (probably); TRUE if x is prime. test done NTRY *
    *  times; chance of wrong identification << (1/4)^NTRY. Note however *
    *  that this is an extreme upper bound. For example for a 100 digit  *
    *  "prime" the chances of false witness are actually < (.00000003)^NTRY *
    *  See Kim & Pomerance "The probability that a random probable prime *
    *  is Composite", Math. Comp. October 1989 pp.721-741                *
    *  The value of NTRY is now adjusted internally to account for this. */
    int j,k,n,times,d;

#ifndef MR_GENERIC_MT
    miracl *mr_mip=get_mip();
#endif
    if (mr_mip->ERNUM) return TRUE;
    if (size(x)<=1) return FALSE;

    MR_IN(22)

    k=trial_division(_MIPP_ x,x);
    if (k==0) 
    {
        MR_OUT
        return FALSE;
    }
    if (k==1)
    {
        MR_OUT
        return TRUE;
    }

/* Miller-Rabin */

    decr(_MIPP_ x,1,mr_mip->w1); /* calculate k and mr_w8 ...    */
    k=0;
    while (subdiv(_MIPP_ mr_mip->w1,2,mr_mip->w1)==0)
    {
        k++;
        copy(mr_mip->w1,mr_mip->w8);
    }              /* ... such that x=1+mr_w8*2**k */
    times=mr_mip->NTRY;
    if (times>100) times=100;
    d=logb2(_MIPP_ x);    /* for larger primes, reduce NTRYs */
    if (d>220) times=2+((10*times)/(d-210));


    for (n=1;n<=times;n++)
    { /* perform test times times */
        j=0;
        powltr(_MIPP_ mr_mip->PRIMES[n],mr_mip->w8,x,mr_mip->w9);
/* uses 3,5,7 etc as basis. Does NOT use 2, as this falsely passes
   all fermat numbers of the form 2^2^n+1 as probable primes */
        decr(_MIPP_ x,1,mr_mip->w2);

        while ((j>0 || size(mr_mip->w9)!=1) 
              && compare(mr_mip->w9,mr_mip->w2)!=0)
        {
            j++;
            if ((j>1 && size(mr_mip->w9)==1) || j==k)
            { /* definitely not prime */
                MR_OUT
                return FALSE;
            }
            mad(_MIPP_ mr_mip->w9,mr_mip->w9,mr_mip->w9,x,x,mr_mip->w9);
        }

        if (mr_mip->user!=NULL) if (!(*mr_mip->user)())
        {
            MR_OUT
            return FALSE;
        }
    }
    MR_OUT
    return TRUE;  /* probably prime */
}

BOOL nxprime(_MIPD_ big w,big x)
{  /*  find next highest prime from w using     * 
    *  probabilistic primality test             */
#ifndef MR_GENERIC_MT
    miracl *mr_mip=get_mip();
#endif
    if (mr_mip->ERNUM) return FALSE;

    MR_IN(21)

    copy(w,x);
    if (size(x)<2) 
    {
        convert(_MIPP_ 2,x);
        MR_OUT
        return TRUE;
    }
    if (subdiv(_MIPP_ x,2,mr_mip->w1)==0) incr(_MIPP_ x,1,x);
    else                           incr(_MIPP_ x,2,x);
    while (!isprime(_MIPP_ x)) 
    {
        incr(_MIPP_ x,2,x);
        if (mr_mip->user!=NULL) if (!(*mr_mip->user)())
        {
            MR_OUT
            return FALSE;
        }
    }
    MR_OUT
    return TRUE;
}


BOOL nxsafeprime(_MIPD_ int type,int subset,big w,big p)
{ /* If type=0 finds next highest "safe" prime p >= w *
   * A safe prime is one for which q=(p-1)/2 is also  *
   * prime, and will be congruent to 3 mod 4          *
   * However if type=1 finds a prime p for which      *
   * q=(p+1)/2 is prime, congruent to 1 mod 4         *
   * Set subset=1 for q=1 mod 4, subset=3 for         *
   * q=3 mod 4, subset=0 if you don't care which      */

    int rem,increment;
#ifndef MR_GENERIC_MT
    miracl *mr_mip=get_mip();
#endif
    if (mr_mip->ERNUM) return FALSE;

    MR_IN(106)

    copy(w,p);

    rem=remain(_MIPP_ p,8);
    if (subset==0)
    {
        rem=rem%4;
        if (type==0) incr(_MIPP_ p,3-rem,p);
        else
        {
            if (rem>1) incr(_MIPP_ p,5-rem,p);
            else       incr(_MIPP_ p,1-rem,p);
        }
        increment=4;
    }
    else
    {
        if (subset==1) 
        {
            if (type==0)
            {
                if (rem>3) incr(_MIPP_ p,11-rem,p);
                else       incr(_MIPP_ p,3-rem,p);
            }
            else
            {
                if (rem>1) incr(_MIPP_ p,9-rem,p);
                else       incr(_MIPP_ p,1-rem,p);
            }
        }
        else 
        { 
            if (type==0) incr(_MIPP_ p,7-rem,p);
            else
            {
                if (rem>5) incr(_MIPP_ p,13-rem,p);
                else       incr(_MIPP_ p,5-rem,p);
            }
        }
        increment=8;
    }
    if (type==0) decr(_MIPP_ p,1,mr_mip->w10);
    else         incr(_MIPP_ p,1,mr_mip->w10);
    subdiv(_MIPP_ mr_mip->w10,2,mr_mip->w10);

    forever
    {
        do {
            if (mr_mip->user!=NULL) if (!(*mr_mip->user)())
            {
                MR_OUT
                return FALSE;
            }
            incr(_MIPP_ p,increment,p);
            incr(_MIPP_ mr_mip->w10,increment/2,mr_mip->w10);
        } while (!trial_division(_MIPP_ p,p) || !trial_division(_MIPP_ mr_mip->w10,mr_mip->w10));
        if (!isprime(_MIPP_ mr_mip->w10)) continue;
        if (isprime(_MIPP_ p)) break;
    }

    MR_OUT
    return TRUE;
}

