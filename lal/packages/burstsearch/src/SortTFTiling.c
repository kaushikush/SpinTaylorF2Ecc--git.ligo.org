/******** <lalVerbatim file="SortTFTilingCV"> ********
Author: Eanna Flanagan
$Id$
********* </lalVerbatim> ********/
 

#include <lal/LALRCSID.h>


NRCSID (SORTTFTILINGC, "$Id$");


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <lal/LALStdlib.h>
#include <lal/LALConstants.h>
#include <lal/SeqFactories.h>
#include <lal/RealFFT.h>
#include <lal/Thresholds.h>
#include <lal/ExcessPower.h>
#include <lal/Random.h>


#define TRUE 1
#define FALSE 0


extern INT4 lalDebugLevel;

static int TileCompare( const void *t1, const void *t2 )
{
  TFTile * const *tiles1 = t1;
  TFTile * const *tiles2 = t2;
  if ( (*tiles1)->alpha > (*tiles2)->alpha )
    return 1;
  if ( (*tiles1)->alpha < (*tiles2)->alpha )
    return -1;
  return 0;
}

/******** <lalVerbatim file="SortTFTilingCP"> ********/
void
LALSortTFTiling (
	      LALStatus         *status,
	      TFTiling          *tfTiling
	      )
/******** </lalVerbatim> ********************************/
{
  INT4               tileCount;
  INT4               numTiles;
  TFTile             *thisTile;
  TFTile             **tiles;

  INITSTATUS (status, "LALSortTFTiling", SORTTFTILINGC);
  ATTATCHSTATUSPTR (status);


    /* make sure that arguments are not NULL */
  ASSERT (tfTiling, status, EXCESSPOWERH_ENULLP, EXCESSPOWERH_MSGENULLP);
  ASSERT (tfTiling->tfp, status, EXCESSPOWERH_ENULLP, EXCESSPOWERH_MSGENULLP);
  ASSERT (tfTiling->dftParams, status, EXCESSPOWERH_ENULLP, 
          EXCESSPOWERH_MSGENULLP);
  ASSERT (tfTiling->firstTile, status, EXCESSPOWERH_ENULLP, 
          EXCESSPOWERH_MSGENULLP);
  ASSERT (tfTiling->numPlanes>0, status, EXCESSPOWERH_EPOSARG,
          EXCESSPOWERH_MSGEPOSARG);


  /* make sure excess power has already been computed */
  ASSERT (tfTiling->excessPowerComputed, status, EXCESSPOWERH_EORDER,
          EXCESSPOWERH_MSGEORDER);

  /* compute number of tiles */
  thisTile = tfTiling->firstTile;
  tileCount=0;
  while (thisTile != NULL)
    {
      tileCount++;
      thisTile = thisTile->nextTile;
    }
  numTiles = tileCount;

  /* 
   *
   *  Make an array of pointers to be used to sort the tiles.
   *
   */

  /* allocate memory for array of pointers to tiles */
  tiles = (TFTile **) LALMalloc (numTiles * sizeof(TFTile *));
  
  /*  Make sure that the allocation was succesful */
  ASSERT (tiles, status, EXCESSPOWERH_ENULLP, EXCESSPOWERH_MSGENULLP);


  /* copy out pointers into array */
  tileCount=0;
  thisTile = tfTiling->firstTile;
  while (thisTile != NULL)
    {
      tileCount++;
      *(tiles + tileCount-1) = thisTile;
      thisTile = thisTile->nextTile;
    }
  
  qsort( tiles, numTiles, sizeof( TFTile * ), TileCompare );

  /* copy sorted array back into linked list */
  { 
    TFTile **currentTile = &(tfTiling->firstTile);

    tileCount=0;
    while (tileCount < numTiles)
      {
	*currentTile = *(tiles + tileCount);
	tileCount++;
	currentTile = &((*currentTile)->nextTile);
      }

    /* correctly terminate the linked list */
    *currentTile = NULL;
  }

  LALFree (tiles);
  
  /* set flag saying tiles have been sorted */
  tfTiling->tilesSorted=TRUE;

  /* normal exit */
  DETATCHSTATUSPTR (status);
  RETURN (status);
}



