/*----------------------------------------------------------------------- 
 * 
 * File Name: ZVectorAbs.c
 * 
 * Author: Creighton, T. D.
 * 
 * Revision: $Id$
 * 
 *----------------------------------------------------------------------- 
 * 
 * NAME 
 * ZVectorAbs
 * 
 * SYNOPSIS 
 *
 *
 * DESCRIPTION 
 * Computes the magnitudes of a double-precision complex vector.
 * 
 * DIAGNOSTICS 
 * Null pointer, invalid input size, size mismatch.
 *
 * CALLS
 * 
 * NOTES
 * 
 *----------------------------------------------------------------------- */

#ifndef _MATH_H
#include <math.h>
#ifndef _MATH_H
#define _MATH_H
#endif
#endif

#ifndef _LALSTDLIB_H
#include "LALStdlib.h"
#ifndef _LALSTDLIB_H
#define _LALSTDLIB_H
#endif
#endif

#ifndef _VECTOROPS_H
#include "VectorOps.h"
#ifndef _VECTOROPS_H
#define _VECTOROPS_H
#endif
#endif

NRCSID(ZVECTORABSC,"$Id$");

void
ZVectorAbs (
    Status                *status,
    REAL8Vector           *out,
    const COMPLEX16Vector *in
    )
{
  COMPLEX16 *a;
  REAL8     *b;
  INT4       n;

  INITSTATUS (status, ZVECTORABSC);

  ASSERT (out, status, VECTOROPS_ENULL, VECTOROPS_MSGENULL);
  ASSERT (in, status, VECTOROPS_ENULL, VECTOROPS_MSGENULL);

  ASSERT (out->data, status, VECTOROPS_ENULL, VECTOROPS_MSGENULL);
  ASSERT (in->data, status, VECTOROPS_ENULL, VECTOROPS_MSGENULL);

  ASSERT (out->length > 0, status, VECTOROPS_ESIZE, VECTOROPS_MSGESIZE);
  ASSERT (in->length == out->length, status,
          VECTOROPS_ESZMM, VECTOROPS_MSGESZMM);

  a = in->data;
  b = out->data;
  n = out->length;

  while (n-- > 0)
  {
    REAL8 ar = a->re;
    REAL8 ai = a->im;

    if (fabs(ar) > fabs(ai))
    {
      REAL8 ratio = ai/ar;
      *b=fabs(ar)*sqrt(1.0 + ratio*ratio);
    }
    else
    {
      REAL8 ratio = ar/ai;
      *b=fabs(ai)*sqrt(1.0 + ratio*ratio);
    }

    ++a;
    ++b;
  }

  RETURN (status);
}

