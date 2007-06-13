/*
*  Copyright (C) 2007 Jolien Creighton, Kipp Cannon, Patrick Brady, Saikat Ray-Majumder, Xavier Siemens
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with with program; see the file COPYING. If not, write to the
*  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
*  MA  02111-1307  USA
*/

/*----------------------------------------------------------------------- 
 * 
 * File Name: SnglBurstUtils.c
 *
 * Author: Brown, D. A.  Brady, P. R. Ray Majumder, S. K. and Cannon, K. C.
 * 
 * Revision: $Id$
 * 
 *-----------------------------------------------------------------------
 */

#if 0
<lalVerbatim file="SnglBurstUtilsCV">
Author: Brown, D. A. Brady, P. R. Ray Majumder, S. K and Cannon, K. C.
$Id$
</lalVerbatim> 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <lal/LALStdlib.h>
#include <lal/LALStdio.h>
#include <lal/LIGOMetadataTables.h>
#include <lal/LIGOMetadataUtils.h>
#include <lal/Date.h>

NRCSID( SNGLBURSTUTILSC, "$Id$" );

#if 0
<lalLaTeX>
\subsection{Module \texttt{SnglBurstUtils.c}}

\noindent Blah.

\subsubsection*{Prototypes}
\vspace{0.1in}
\input{SnglBurstUtilsCP}
\idx{LALSortSnglBurst()}
\idx{XLALSortSnglBurst()}
\idx{XLALCompareSnglBurstByStartTime()}
\idx{XLALCompareSnglBurstByLowFreq()}
\idx{XLALCompareSnglBurstByStartTimeAndLowFreq()}
\idx{XLALCompareSimBurstAndSnglBurst()}
\idx{XLALClusterSnglBurstTable()}

\subsubsection*{Description}

\noindent Blah.

\subsubsection*{Algorithm}

\noindent None.

\subsubsection*{Uses}

\noindent None.

\subsubsection*{Notes}
%% Any relevant notes.
 
\vfill{\footnotesize\input{SnglBurstUtilsCV}}

</lalLaTeX>
#endif


/*
 * A few quickies for convenience.
 */


static INT8 start_time(const SnglBurstTable *x)
{
	return(XLALGPStoINT8(&x->start_time));
}


static INT8 peak_time(const SnglBurstTable *x)
{
	return(XLALGPStoINT8(&x->peak_time));
}


static REAL4 lo_freq(const SnglBurstTable *x)
{
	return(x->central_freq - x->bandwidth / 2);
}


/*
 * Free a sngl_burst 
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
void XLALFreeSnglBurst(SnglBurstTable *event)
/* </lalVerbatim> */
{
	XLALFree(event);
}


/*
 * Assign event_id values to the entries in a sngl_burst linked list.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
void XLALSnglBurstAssignIDs(SnglBurstTable *head)
/* </lalVerbatim> */
{
	unsigned int id;

	for(id = 0; head; head = head->next)
		head->event_id = id++;
}


/*
 * Sort a list of SnglBurstTable events into increasing order according to the
 * supplied comparison function.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
int XLALSnglBurstTableLength(SnglBurstTable *head)
/* </lalVerbatim> */
{
	int length;

	/* count the number of events in the list */
	for(length = 0; head; head = head->next)
		length++;

	return(length);
}


/* <lalVerbatim file="SnglBurstUtilsCP"> */
void XLALSortSnglBurst(
	SnglBurstTable **head,
	int (*comparefunc)(const SnglBurstTable * const *, const SnglBurstTable * const *)
)
/* </lalVerbatim> */
{
	int i;
	int length;
	SnglBurstTable *event;
	SnglBurstTable **array;

	/* empty list --> no-op */
	if(!head || !*head)
		return;

	/* construct an array of pointers into the list */
	length = XLALSnglBurstTableLength(*head);
	array = XLALCalloc(length, sizeof(*array));
	for(i = 0, event = *head; event; event = event->next)
		array[i++] = event;

	/* sort the array using the specified function */
	qsort(array, length, sizeof(*array), (int(*)(const void *, const void *)) comparefunc);

	/* re-link the list according to the sorted array */
	for(i = 0; i < length; i++, head = &(*head)->next)
		*head = array[i];
	*head = NULL;

	/* free the array */
	XLALFree(array);
}


/*
 * Compare the start times of two SnglBurstTable events.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
int XLALCompareSnglBurstByStartTime(
	const SnglBurstTable * const *a,
	const SnglBurstTable * const *b
)
/* </lalVerbatim> */
{
	INT8 ta, tb;

	ta = start_time(*a);
	tb = start_time(*b);

	if(ta > tb)
		return(1);
	if(ta < tb)
		return(-1);
	return(0);
}


/*
 * Compare two sngl_burst events by their peak times, with no slack.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
int XLALCompareSnglBurstByExactPeakTime(
	const SnglBurstTable * const *a,
	const SnglBurstTable * const *b
)
/* </lalVerbatim> */
{
	INT8 ta, tb;

	ta = peak_time(*a);
	tb = peak_time(*b);

	if(ta > tb)
		return(1);
	if(ta < tb)
		return(-1);
	return(0);
}


/*
 * Compare the SNRs of two SnglBurstTable events.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
int XLALCompareSnglBurstBySNR(
	const SnglBurstTable * const *a,
	const SnglBurstTable * const *b
)
/* </lalVerbatim> */
{
	REAL4 snra = (*a)->snr;
	REAL4 snrb = (*b)->snr;

	if(snra < snrb)
		return 1;
	if(snra > snrb)
		return -1;
	return 0;
}


/*
 * Compare the peak times and SNRs of two SnglBurstTable events.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
int XLALCompareSnglBurstByPeakTimeAndSNR(
	const SnglBurstTable * const *a,
	const SnglBurstTable * const *b
)
/* </lalVerbatim> */
{
	int result;

	result = XLALCompareSnglBurstByExactPeakTime(a, b);
	if(!result)
		result = XLALCompareSnglBurstBySNR(a, b);

	return(result);
}


/*
 * Check if two string events overlap in time. The peak times are uncertain
 * to whatever the high frequency cutoff is
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
int XLALCompareStringBurstByTime(
	const SnglBurstTable * const *a,
	const SnglBurstTable * const *b
)
/* </lalVerbatim> */
{
	INT8 ta, tb;
	INT8 epsilon;

	epsilon = (*a)->string_cluster_t * 1e9;
	if(epsilon < (*b)->string_cluster_t * 1e9)
		epsilon = (*b)->string_cluster_t * 1e9;

	ta = peak_time(*a);
	tb = peak_time(*b);
	if(ta > tb + epsilon)
		return(1);
	if(ta < tb - epsilon)
		return(-1);
	return(0);
}


/*
 * Compare the low frequency limits of two SnglBurstTable events.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
int XLALCompareSnglBurstByLowFreq(
	const SnglBurstTable * const *a,
	const SnglBurstTable * const *b
)
/* </lalVerbatim> */
{
	REAL4 flowa, flowb;

	flowa = lo_freq(*a);
	flowb = lo_freq(*b);

	if(flowa > flowb)
		return(1);
	if(flowa < flowb)
		return(-1);
	return(0);
}


/*
 * Compare two events first by start time, then by lowest frequency to break
 * ties.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
int XLALCompareSnglBurstByStartTimeAndLowFreq(
	const SnglBurstTable * const *a,
	const SnglBurstTable * const *b
)
/* </lalVerbatim> */
{
	int result;

	result = XLALCompareSnglBurstByStartTime(a, b);
	if(!result)
		result = XLALCompareSnglBurstByLowFreq(a, b);
	return(result);
}


/*
 * cluster events a and b, storing result in a; takes one with largest snr
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
void XLALStringBurstCluster(
	SnglBurstTable *a,
	const SnglBurstTable *b
)
/* </lalVerbatim> */
{
	REAL4 snra=a->snr, snrb=b->snr;

	if(snrb > snra) {
		a->central_freq =  b->central_freq;
		a->bandwidth = b->bandwidth; 
		a->start_time = b->start_time;
		a->duration = b->duration;
		a->amplitude = b->amplitude;
		a->snr = b->snr;
		a->confidence = b->confidence;
		a->peak_time = b->peak_time;
	}
}


/*
 * Recursively cluster a linked list of SnglBurstTable events until the list
 * stops changing.  testfunc() should return 0 if the two given events are to
 * be clustered.  If bailoutfunc() is provided (not NULL), then testfunc() will
 * be used to sort the trigger list before each clustering pass and
 * bailoutfunc() will be called to check for the option of terminating the
 * inner loop early.  In the ideal case, use of bailoutfunc() converts this
 * algorithm from O(n^3) to order O(n log n).  The clusterfunc() should replace
 * the SnglBurstTable event pointed to by its first argument with the cluster
 * of that event and the event pointed to by the second argument.
 */


/* <lalVerbatim file="SnglBurstUtilsCP"> */
void XLALClusterSnglBurstTable (
	SnglBurstTable **list,
	int (*bailoutfunc)(const SnglBurstTable * const *, const SnglBurstTable * const *),
	int (*testfunc)(const SnglBurstTable * const *, const SnglBurstTable * const *),
	void (*clusterfunc)(SnglBurstTable *, const SnglBurstTable *)
)
/* </lalVerbatim> */
{
	int did_cluster;
	SnglBurstTable *a, *b, *prev;

	do {
		did_cluster = 0;

		if(bailoutfunc)
			XLALSortSnglBurst(list, testfunc);

		for(a = *list; a; a = a->next)
			for(prev = a, b = a->next; b; b = prev->next) {
				if(!testfunc((const SnglBurstTable * const *) &a, (const SnglBurstTable * const *) &b)) {
					clusterfunc(a, b);
					prev->next = b->next;
					XLALFreeSnglBurst(b);
					did_cluster = 1;
				} else {
					if(bailoutfunc && bailoutfunc((const SnglBurstTable * const *) &a, (const SnglBurstTable * const *) &b))
						break;
					prev = b;
				}
			}
	} while(did_cluster);
}
