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

static const char name[]    = "test26";
static const char purpose[] = "tests grLfbWriteRegion, and grLfbReadRegion";
static const char usage[]   = "-n <frames> -r <res>";

static const char *renderBufferString[] = {
    "GR_BUFFER_FRONTBUFFER",
    "GR_BUFFER_BACKBUFFER "
};

static const char *sourceFormatString[] = {
    "GR_LFB_SRC_FMT_565       ",
    "GR_LFB_SRC_FMT_555       ",
    "GR_LFB_SRC_FMT_1555      ",
    "INVALID FORMAT           ",
    "GR_LFB_SRC_FMT_888       ",
    "GR_LFB_SRC_FMT_8888      ",
    "INVALID FORMAT           ",
    "INVALID FORMAT           ",
    "INVALID FORMAT           ",
    "INVALID FORMAT           ",
    "INVALID FORMAT           ",
    "INVALID FORMAT           ",
    "GR_LFB_SRC_FMT_565_DEPTH ",
    "GR_LFB_SRC_FMT_555_DEPTH ",
    "GR_LFB_SRC_FMT_1555_DEPTH",
    "GR_LFB_SRC_FMT_ZA16      "
};


void main( int argc, char **argv) {
    char match; 
    char **remArgs;
    int  rv;

    GrScreenResolution_t resolution = GR_RESOLUTION_640x480;
    float                scrWidth   = 640.0f;
    float                scrHeight  = 480.0f;
    int frames                      = -1;

    TlTexture texture;
    void *image;
    void *readImage;
    FxU32 bpp;

    GrLfbSrcFmt_t sourceFormat;

    static FxU32 imageWidth;
    static FxU32 imageHeight;

    static void imageConvert( void *dst, 
                              void *src,
                              GrLfbSrcFmt_t format, 
                              FxU32 *bpp );

    /* Process Command Line Arguments */
    while( rv = tlGetOpt( argc, argv, "nr", &match, &remArgs ) ) {
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

    /* Set up Render State - disable dithering */
    grDitherMode( GR_DITHER_DISABLE );

    /* Load image from disk */
    assert( tlLoadTexture( "decal1.3df", 
                           &texture.info, 
                           &texture.tableType, 
                           &texture.tableData ) );

    imageWidth  = 256;
    imageHeight = 256;
    image       = calloc( imageWidth * imageHeight, sizeof( FxU32 ) );
    readImage   = calloc( imageWidth * imageHeight, sizeof( FxU32 ) );

    if ( ( imageWidth > (FxU32)scrWidth ) ||
         ( imageHeight > (FxU32)scrHeight ) ) {
      char errMsg[1024];
      sprintf(errMsg,
              "imgWidth: %d, imgHeight: %d\nscrWidth: %d, scrHeight: %d\n",
              (int) imageWidth, (int) imageHeight, 
              (int) scrWidth, (int) scrHeight);

      tlErrorMessage(errMsg);
      return;
    }

    sourceFormat = GR_LFB_SRC_FMT_565;

    imageConvert( image, texture.info.data, sourceFormat, &bpp );

    while( frames-- ) {
        static int x, y;

        if (!tlOkToRender())
          continue;

        if (hwconfig == TL_VOODOORUSH) {
          tlGetDimsByConst(resolution,
                           &scrWidth, 
                           &scrHeight );
        
          grClipWindow(0, 0, (FxU32) scrWidth, (FxU32) scrHeight);
        }

        grBufferClear( 0x00303030, 0, 0 );

        if ( ( imageWidth  < (FxU32)tlScaleX(1.0f) ) &&
             ( imageHeight < (FxU32)tlScaleY(1.0f) ) &&
             grLfbWriteRegion( GR_BUFFER_BACKBUFFER,
                               x, y, sourceFormat,
                               imageWidth, 256, FXFALSE, 256*bpp, image ) ) {
            tlConClear();
            tlConOutput( "1   - lfb source format (%s)\n"
                         "+/- - change width of source image copied\n"
                         "any other key to quit\n\n",
                         sourceFormatString[sourceFormat] );
            if ( sourceFormat == GR_LFB_SRC_FMT_565 ) {
                grLfbReadRegion( GR_BUFFER_BACKBUFFER,
                                 x, y, imageWidth, 256,
                                 256*bpp, readImage );
                if ( memcmp( image, readImage, 256*256*2 ) ) {
                    tlConOutput( "Failed Readback Test\n" );
                } else {
                    tlConOutput( "Passed Readback Test\n" );
                }
            } 
        } else {
            tlConClear();
            tlConOutput( "1   - lfb source format (%s)\n"
                         "+/- - change width of source image copied\n"
                         "any other key to quit\n\n",
                         sourceFormatString[sourceFormat] );
            tlConOutput( "grLfbWriteRegion Failed\n" );
        }

        x++;
        if ( x + 256 >= (int)tlScaleX(1.0f) ) {
            x = 0; y++;
        }
        if ( y + 256 >= (int)tlScaleY(1.0f) ) {
            y = 0;
        }

        tlConRender();
        grBufferSwap( 1 );
        while( tlKbHit() ) {
            switch( tlGetCH() ) {
            case '1':
                sourceFormat++;
                if ( sourceFormat > GR_LFB_SRC_FMT_ZA16 )
                    sourceFormat = GR_LFB_SRC_FMT_565;
                imageConvert( image, texture.info.data, sourceFormat, &bpp );
                break;
            case '+':
                if ( imageWidth < 256 ) imageWidth++;
                break;
            case '-':
                if ( imageWidth > 0 ) imageWidth--;
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


static void imageConvert( void *dst, void *src, 
                          GrLfbSrcFmt_t format, FxU32 *bpp ) {
    FxU32 x, y; 
    FxU32 *longData   = dst;
    FxU16 *shortData  = dst;
    FxU16 *srcData    = src;
    FxU32 longStride  = 256;
    FxU32 shortStride = 256;
    FxU32 longColor;
    FxU16 shortColor;

    switch( format ) {
    case GR_LFB_SRC_FMT_565:
    case GR_LFB_SRC_FMT_555:
    case GR_LFB_SRC_FMT_1555:
        *bpp = 2;
        break;
    case GR_LFB_SRC_FMT_888:
    case GR_LFB_SRC_FMT_8888:
    case GR_LFB_SRC_FMT_565_DEPTH:
    case GR_LFB_SRC_FMT_1555_DEPTH:
    case GR_LFB_SRC_FMT_555_DEPTH:
        *bpp = 4;
        break;
    }

    for( y = 0; y < 256; y++ ) {
        for( x = 0; x < 256; x++ ) {
            switch( format ) {
            case GR_LFB_SRC_FMT_565:
                shortData[y*shortStride+x] = 
                    srcData[y*256+x];
                break;
            case GR_LFB_SRC_FMT_555:
            case GR_LFB_SRC_FMT_1555:
                shortColor = srcData[y*256+x];
                shortColor = 
                    (0x8000) | // Alpha == 1
                    ((shortColor >> 1) & 0x7C00) |
                    ((shortColor >> 1) & 0x03E0) |
                    ((shortColor) & 0x1f);
                shortData[y*shortStride+x] =
                    shortColor;
                break;
            case GR_LFB_SRC_FMT_888:
            case GR_LFB_SRC_FMT_8888:
                longColor = srcData[y*256+x];
                longColor =
                    (0xFF000000)                 |
                    ((longColor<<8)&0x00F80000)  |
                    ((longColor<<5)&0x0000FC00)  |
                    ((longColor<<3)&0x000000F8);
                longData[y*longStride+x] = longColor;
                break;
            case GR_LFB_SRC_FMT_565_DEPTH:
                longColor = srcData[y*256+x];
                longData[y*longStride+x] = longColor;
                break;
            case GR_LFB_SRC_FMT_1555_DEPTH:
            case GR_LFB_SRC_FMT_555_DEPTH:
                longColor = srcData[y*256+x];
                longColor =
                    (0x00008000)                  |
                    ((longColor>>1) & 0x00007C00) |
                    ((longColor>>1) & 0x000003E0) |
                    ((longColor   ) & 0x0000001f);
                longData[y*longStride+x] = longColor;
            default:
                break;
            }
        }
    }
    return;
}



