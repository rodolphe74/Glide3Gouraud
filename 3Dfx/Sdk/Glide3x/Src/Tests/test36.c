/*
** Copyright (c) 1997, 3Dfx Interactive, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of 3Dfx Interactive, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <assert.h>
#include <string.h>

#include <glide.h>
#include "tlib.h"

int hwconfig;
static const char *version;
static const char *extension;

static const char name[]    = "test36";
static const char purpose[] = "screen resolutions";
static const char usage[]   = "-p show passed, -n no progress";

static const char status[]  = "-\\|/";

void main( int argc, char **argv) {
	FxBool bShowPassed = FXFALSE;
	FxBool bNoProgress = FXFALSE;
	GrResolution resTemplate;
	GrResolution *presSupported;
	FxI32 size, i, 
		iFailures = 0, iStatusAt = 0,
		viewport[4];
	GrContext_t context;
	extern unsigned long hWndMain;

	printf ( "Test36 - Glide resolution checker.  All output goes to the console.\n\n" );
	tlGetCH ();
	for ( i=1; i < argc; i++ ) {
		if ( !strcmp ( argv[i], "-p" ) )
			bShowPassed = FXTRUE;
		if ( !strcmp ( argv[i], "-n" ) )
			bNoProgress = FXTRUE;
	}

	grGlideInit ();
	resTemplate.resolution = GR_QUERY_ANY;
	resTemplate.refresh = GR_QUERY_ANY;
	resTemplate.numColorBuffers = GR_QUERY_ANY;
	resTemplate.numAuxBuffers = GR_QUERY_ANY;

	size = grQueryResolutions ( &resTemplate, NULL );
	presSupported = ( GrResolution * ) malloc ( size );
	
	size = size / sizeof ( GrResolution );
	grQueryResolutions ( &resTemplate, presSupported );

	grGlideShutdown ();

	// Now, start the assault...  Glide Init, mode set, Glide Shutdown...

	for ( i = 0; i < size; i++ ) {
		grGlideInit ();

		context = grSstWinOpen ( hWndMain, 
			presSupported[i].resolution, 
			presSupported[i].refresh, 
			GR_COLORFORMAT_ABGR, 
			GR_ORIGIN_UPPER_LEFT, 
			presSupported[i].numColorBuffers, 
			presSupported[i].numAuxBuffers );
		
		if ( context ) {
			grGet ( GR_VIEWPORT, sizeof ( FxI32 ) * 4, viewport );
			if ( bShowPassed ) {
				printf ( "Res #%d ( %d, %d, %d, %d ) - Refresh %d, nCol %d, nAux %d - PASSED!\n", 
				presSupported[i].resolution, viewport[0], viewport[1], viewport[2], viewport[3], 
				presSupported[i].refresh, presSupported[i].numColorBuffers, presSupported[i].numAuxBuffers );
			} else if ( !bNoProgress ) {
				printf ( "Working...  %c\r", status[iStatusAt] );
				iStatusAt = ( iStatusAt + 1 ) & 3;
			}
			grSstWinClose ( context ) ;
		} else {
			printf ( "Res #%d - Refresh %d, nCol %d, nAux %d - FAILED!\n", 
				presSupported[i].resolution, presSupported[i].refresh, 
				presSupported[i].numColorBuffers, presSupported[i].numAuxBuffers );
			iFailures++;
		}			

		grGlideShutdown ();
	}

	free ( presSupported );

	if ( iFailures ) 
		printf ( "This test failed with %d failures.  ", iFailures );
	else
	    printf ( "This test passed.  " );

	printf ( "Press any key.\n" );
	tlGetCH ();
	return;
}





