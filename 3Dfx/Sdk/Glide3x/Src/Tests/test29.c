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

static const char name[]    = "test29";
static const char purpose[] = "Simple palette texture test";
static const char usage[]   = "-n <frames> -r <res> -d <filename>";

void main(int argc, char **argv) {
  char match; 
  char **remArgs;
  int  rv;

  GrScreenResolution_t resolution = GR_RESOLUTION_640x480;
  float                scrWidth   = 640.0f;
  float                scrHeight  = 480.0f;
  int frames                      = -1;
  FxBool               scrgrab = FXFALSE;
  char                 filename[256];
  FxU32                wrange[2];

  /* Process Command Line Arguments */
  while(rv = tlGetOpt(argc, argv, "nrd", &match, &remArgs)) {
    if (rv == -1) {
      printf("Unrecognized command line argument\n");
      printf("%s %s\n", name, usage);
      printf("Available resolutions:\n%s\n",
              tlGetResolutionList());
      return;
    }
    switch(match) {
    case 'n':
      frames = atoi(remArgs[0]);
      break;
    case 'r':
      resolution = tlGetResolutionConstant(remArgs[0], 
                                            &scrWidth, 
                                            &scrHeight);
      break;
    case 'd':
      scrgrab = FXTRUE;
      frames = 1;
      strcpy(filename, remArgs[0]);
      break;
    }
  }

  tlSetScreen(scrWidth, scrHeight);

  version = grGetString( GR_VERSION );

  printf("%s:\n%s\n", name, purpose);
  printf("%s\n", version);
  printf("Resolution: %s\n", tlGetResolutionString(resolution));
  if (frames == -1) {
    printf("Press A Key To Begin Test.\n");
    tlGetCH();
  }
    
  /* Initialize Glide */
  grGlideInit();
  assert( hwconfig = tlVoodooType() );

  grSstSelect(0);
  assert(grSstWinOpen(0,
                      resolution,
                      GR_REFRESH_60Hz,
                      GR_COLORFORMAT_ABGR,
                      GR_ORIGIN_UPPER_LEFT,
                      2, 1));
    
  tlConSet(0.0f, 0.0f, 1.0f, 1.0f, 
            60, 30, 0xffffff);

  {
    GuTexPalette texPal;
    GrTexInfo texInfo;
    unsigned char texData[256 * 256];
    int i, j;

    /* Create simple greyscale ramp palette w/o any alpha */
    for(i = 0; i < sizeof(texPal.data) / sizeof(texPal.data[0]); i++) {
      texPal.data[i] = ((i << 16) |
                        (i << 8) |
                        (i << 0));
    }

    /* Download this in a sequential way to test the palette download */
    i = 0;
    while(i < 256) {
      const int end = MIN((i + (rand() % 10)), 255);

      grTexDownloadTablePartial(GR_TEXTABLE_PALETTE, &texPal, i, end);
      i = end + 1;
    }

    /* Download dumb palettized texture */
    texInfo.smallLodLog2 =
    texInfo.largeLodLog2 = GR_LOD_LOG2_256;
    texInfo.aspectRatioLog2 = GR_ASPECT_LOG2_1x1;
    texInfo.format = GR_TEXFMT_P_8;
    texInfo.data = (void*)texData;
    
    for(i = 0; i < 256; i++) {
      for(j = 0; j < 256; j++) {
        texData[i * 256 + j] = (unsigned char)i;
      }
    }
    grTexDownloadMipMap(GR_TMU0, 0, GR_MIPMAPLEVELMASK_BOTH, &texInfo);
    grTexSource(GR_TMU0, 0, GR_MIPMAPLEVELMASK_BOTH, &texInfo);
  }
  
  /* Set up Render State */
  grGet(GR_WDEPTH_MIN_MAX, 8, wrange);  
  grVertexLayout(GR_PARAM_XY,  0, GR_PARAM_ENABLE);
  grVertexLayout(GR_PARAM_Q,   GR_VERTEX_OOW_OFFSET << 2, GR_PARAM_ENABLE);
  grVertexLayout(GR_PARAM_ST0, GR_VERTEX_SOW_TMU0_OFFSET << 2, GR_PARAM_ENABLE);

  grColorCombine(GR_COMBINE_FUNCTION_SCALE_OTHER,
                 GR_COMBINE_FACTOR_ONE,
                 GR_COMBINE_LOCAL_CONSTANT,
                 GR_COMBINE_OTHER_TEXTURE,
                 FXFALSE);
  grTexCombine(GR_TMU0,
                 GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_ONE,
                 GR_COMBINE_FUNCTION_LOCAL, GR_COMBINE_FACTOR_ONE,
                 FXFALSE, FXFALSE);
  
  tlConOutput("Press a key to quit\n");
  while(frames-- && tlOkToRender()) {
    GrVertex vtxA, vtxB, vtxC;

    if (hwconfig == TL_VOODOORUSH) {
      tlGetDimsByConst(resolution,
                       &scrWidth, 
                       &scrHeight);
        
      grClipWindow(0, 0, (FxU32) scrWidth, (FxU32) scrHeight);
    }

    grBufferClear(0x00FF0000, 0, wrange[1]);

    /* Triangle w/ texture ramp going across and down. */
    vtxA.oow = 
    vtxB.oow =
    vtxC.oow = 1.0f;

    vtxA.x = tlScaleX(0.2f), vtxA.y = tlScaleY(0.2f);
    vtxA.tmuvtx[0].sow = 
    vtxA.tmuvtx[0].tow = 0.0f;

    vtxB.x = tlScaleX(0.8f), vtxB.y = tlScaleY(0.2f);
    vtxB.tmuvtx[0].sow = 255.0f; 
    vtxB.tmuvtx[0].tow = 0.0f;

    vtxC.x = tlScaleX(0.5f), vtxC.y = tlScaleY(0.8f);
    vtxC.tmuvtx[0].sow = 128.0f;
    vtxC.tmuvtx[0].tow = 255.0f;

    grDrawTriangle(&vtxA, &vtxB, &vtxC);

    tlConRender();
    grBufferSwap(1);

    /* grab the frame buffer */
    if (scrgrab) {
      if (!tlScreenDump(filename, (FxU16)scrWidth, (FxU16)scrHeight))
        printf("Cannot open %s\n", filename);
      scrgrab = FXFALSE;
    }

    if (tlKbHit()) frames = 0;
  }
    
  grGlideShutdown();
  return;
}
