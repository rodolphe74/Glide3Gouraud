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

static const char name[]    = "test35";
static const char purpose[] = "texture chromarange";
static const char usage[]   = "-n <frames> -r <res> -d <filename>";

void main( int argc, char **argv) {
    char match; 
    char **remArgs;
    int  rv;

    GrScreenResolution_t resolution = GR_RESOLUTION_640x480;
    float                scrWidth   = 640.0f;
    float                scrHeight  = 480.0f;
    int frames                      = -1;
    FxBool               scrgrab = FXFALSE;
    char                 filename[256];
    int                  ftsize = 0;

    TlTexture  baseTexture;
    unsigned long baseTextureAddr;
    GrVertex vtxA, vtxB, vtxC, vtxD;
    FxBool texchroma = FXFALSE;
    char *extstr;

    GrProc grTexChromaModeExt = NULL;
    GrProc grTexChromaRangeExt = NULL;
    GrColor_t min = 0x00, max = 0x007f7f7f;
    FxU8 mincolor = 0x00;
    FxU8 maxcolor = 0x7f;
    FxFloat red = 1.7f, green = 1.7f, blue = 1.7f;

    /* Process Command Line Arguments */
    while( rv = tlGetOpt( argc, argv, "nrd", &match, &remArgs ) ) {
        if ( rv == -1 ) {
            printf( "Unrecognized command line argument\n" );
            printf( "%s %s\n", name, usage );
            printf( "Available resolutions:\n%s\n",
                    tlGetResolutionList() );
            return;
        }
        switch( match ) {
        case 'n':
            frames = atoi( remArgs[0] );
            break;
        case 'r':
            resolution = tlGetResolutionConstant( remArgs[0], 
                                                  &scrWidth, 
                                                  &scrHeight );
            break;
        case 'd':
            scrgrab = FXTRUE;
            frames = 1;
            strcpy(filename, remArgs[0]);
            break;
        }
    }

    tlSetScreen( scrWidth, scrHeight );

    version = grGetString( GR_VERSION );

    printf( "%s:\n%s\n", name, purpose );
    printf( "%s\n", version );
    printf( "Resolution: %s\n", tlGetResolutionString( resolution ) );
    if ( frames == -1 ) {
        printf( "Press A Key To Begin Test.\n" );
        tlGetCH();
    }
    
    /* Initialize Glide */
    grGlideInit();
    assert( hwconfig = tlVoodooType() );

    grSstSelect( 0 );
    assert( grSstWinOpen( 0,
                      resolution,
                      GR_REFRESH_60Hz,
                      GR_COLORFORMAT_ABGR,
                      GR_ORIGIN_UPPER_LEFT,
                      2, 1 ) );
    
    tlConSet( 0.0f, 0.0f, 1.0f, 1.0f, 
              60, 30, 0xffffff );

    grVertexLayout(GR_PARAM_XY,  GR_VERTEX_X_OFFSET << 2, GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_Q,   GR_VERTEX_OOW_OFFSET << 2, GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_ST0, GR_VERTEX_SOW_TMU0_OFFSET << 2, GR_PARAM_ENABLE);

    grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                    GR_COMBINE_FACTOR_ONE,
                    GR_COMBINE_LOCAL_NONE,
                    GR_COMBINE_OTHER_TEXTURE,
                    FXFALSE );
    grTexMipMapMode( GR_TMU0,
                     GR_MIPMAP_NEAREST,
                     FXFALSE );
    grTexFilterMode( GR_TMU0,
                     GR_TEXTUREFILTER_BILINEAR,
                     GR_TEXTUREFILTER_BILINEAR );

    {
      /*
      ** texChroma extension
      */
      extension = grGetString(GR_EXTENSION);

      extstr = strstr(extension, "CHROMARANGE");
      if (!strncmp(extstr, "CHROMARANGE", 11)) {
        grTexChromaModeExt = grGetProcAddress("grTexChromaModeExt");
        grTexChromaRangeExt = grGetProcAddress("grTexChromaRangeExt");
      }
      grTexChromaModeExt(GR_TMU0, GR_TEXCHROMA_DISABLE_EXT);
      grTexChromaRangeExt(0, min, max, GR_TEXCHROMARANGE_RGB_ALL_EXT);
    }

    assert( tlLoadTexture( "miro.3df", 
                           &baseTexture.info, 
                           &baseTexture.tableType, 
                           &baseTexture.tableData ) );
    /* Download texture data to TMU */
    baseTextureAddr = grTexMinAddress( GR_TMU0 );
    grTexDownloadMipMap( GR_TMU0,
                         baseTextureAddr,
                         GR_MIPMAPLEVELMASK_BOTH,
                         &baseTexture.info );
    if ( baseTexture.tableType != NO_TABLE ) {
        grTexDownloadTable( baseTexture.tableType,
                            &baseTexture.tableData );
    }
    grTexCombine( GR_TMU0,
                  GR_COMBINE_FUNCTION_LOCAL,
                  GR_COMBINE_FACTOR_NONE,
                  GR_COMBINE_FUNCTION_LOCAL,
                  GR_COMBINE_FACTOR_NONE,
                  FXFALSE,
                  FXFALSE );
    grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ZERO,
                          GR_BLEND_ONE, GR_BLEND_ZERO );
    grTexSource( GR_TMU0,
                 baseTextureAddr,
                 GR_MIPMAPLEVELMASK_BOTH,
                 &baseTexture.info );
    grTexClampMode( GR_TMU0, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP);

    {
      vtxA.x = tlScaleX( 0.2f );
      vtxA.y = tlScaleY( 0.2f );
      vtxA.oow = 1.0f;
      
      vtxB.x = tlScaleX( 0.8f );
      vtxB.y = tlScaleY( 0.2f ); 
      vtxB.oow = 1.0f;
      
      vtxC.x = tlScaleX( 0.2f );
      vtxC.y = tlScaleY( 0.8f );
      vtxC.oow = 1.0f;
      
      vtxD.x = tlScaleX( 0.8f ); 
      vtxD.y = tlScaleY( 0.8f );
      vtxD.oow = 1.0f;
      
      vtxA.tmuvtx[0].sow = 0.f;
      vtxA.tmuvtx[0].tow = 0.f;
      
      vtxB.tmuvtx[0].sow = 255.f;
      vtxB.tmuvtx[0].tow = 0.f;
      
      vtxC.tmuvtx[0].sow = 0.f;
      vtxC.tmuvtx[0].tow = 255.f;
      
      vtxD.tmuvtx[0].sow = 255.f;
      vtxD.tmuvtx[0].tow = 255.f;
    }
    
    tlConOutput( "Press a key to quit\n" );
    tlConOutput( "Press c to turn on/off texture chroma\n" );
    tlConOutput( "Press {/} to change max color range\n" );
    tlConOutput( "Press [/] to change min color range\n" );
    while( frames-- && tlOkToRender()) {
      char string[256], tmpstr[64];

      if (texchroma)
        strcpy(string, "");
      else
        strcpy(string, "");
      sprintf(tmpstr, " (%x,%x,%x) (%x,%x,%x) (%f %f %f)   \r", mincolor, mincolor, mincolor, maxcolor, maxcolor, maxcolor,red,green,blue);
      strcat(string, tmpstr);

      tlConOutput( string );

      if (hwconfig == TL_VOODOORUSH) {
        tlGetDimsByConst(resolution,
                         &scrWidth, 
                         &scrHeight );
        
        grClipWindow(0, 0, (FxU32) scrWidth, (FxU32) scrHeight);
      }

      grBufferClear( 0x3f3f3f, 0, 0 );
      
      grDrawTriangle( &vtxA, &vtxB, &vtxD );
      grDrawTriangle( &vtxA, &vtxD, &vtxC );
      
      tlConRender();
      grBufferSwap( 1 );
      
      /* grab the frame buffer */
      if (scrgrab) {
        if (!tlScreenDump(filename, (FxU16)scrWidth, (FxU16)scrHeight))
          printf( "Cannot open %s\n", filename);
        scrgrab = FXFALSE;
      }
      
      while( tlKbHit() ) {
        switch( tlGetCH() ) {
        case 'r':
          red -= 0.1f;
          guGammaCorrectionRGB(red, green, blue);
          break;
        case 'R':
          red += 0.1f;
          guGammaCorrectionRGB(red, green, blue);
          break;
        case 'g':
          green -= 0.1f;
          guGammaCorrectionRGB(red, green, blue);
          break;
        case 'G':
          green += 0.1f;
          guGammaCorrectionRGB(red, green, blue);
          break;
        case 'b':
          blue -= 0.1f;
          guGammaCorrectionRGB(red, green, blue);
          break;
        case 'B':
          blue += 0.1f;
          guGammaCorrectionRGB(red, green, blue);
          break;
        case 'c':
        case 'C':
          texchroma = !texchroma;
          if (texchroma)
            grTexChromaModeExt(GR_TMU0, GR_TEXCHROMA_ENABLE_EXT);
          else
            grTexChromaModeExt(GR_TMU0, GR_TEXCHROMA_DISABLE_EXT);
          break;
        case '{':
          if ((maxcolor > 0x00) && (maxcolor > mincolor))
            maxcolor--;
          max = (maxcolor << 16) | (maxcolor << 8) | maxcolor;
          grTexChromaRangeExt(0, min, max, GR_TEXCHROMARANGE_RGB_ALL_EXT);
          break;
        case '}':
          if (maxcolor < 0xff)
            maxcolor++;
          max = (maxcolor << 16) | (maxcolor << 8) | maxcolor;
          grTexChromaRangeExt(0, min, max, GR_TEXCHROMARANGE_RGB_ALL_EXT);
          break;
        case '[':
          if (mincolor > 0x00)
            mincolor--;
          min = (mincolor << 16) | (mincolor << 8) | mincolor;
          grTexChromaRangeExt(0, min, max, GR_TEXCHROMARANGE_RGB_ALL_EXT);
          break;
        case ']':
          if ((mincolor < 0xff) && (mincolor < maxcolor))
            mincolor++;
          min = (mincolor << 16) | (mincolor << 8) | mincolor;
          grTexChromaRangeExt(0, min, max, GR_TEXCHROMARANGE_RGB_ALL_EXT);
          break;
        default:
          frames = 0;
          break;
        }
      }
    }
    
    grGlideShutdown();
    return;
}





