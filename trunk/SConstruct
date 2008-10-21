# scons script written by Nguyen Hung Vu ( vuhung16plus*nospam*@gmail.dot.com )
# based on Chrome scons build script

import os
import shutil
import sys

env = Environment()

env.Prepend(
  CPPPATH = [
    'include',
    'include/corecg',
    'corecg',
    'sgl',
    'picture',
  ],
)

env.Append(
  CPPDEFINES = [
    'SKIA_DISABLE_SUPPORT_FOR_DECODERS',
  ],
)

if env['PLATFORM'] == 'win32':
  env.Append(
    CCFLAGS = [
      '/TP',
      '/wd4244',
      '/wd4267',
      '/wd4345',
      '/wd4390',
      '/wd4554',
      '/wd4800',
    ],
  )


elif env['PLATFORM'] in ('posix', 'darwin'):
  env.Append(CCFLAGS = ['-Wno-unused-variable'])

input_files = [
  'animator/SkTime.cpp',
  'corecg/Sk64.cpp',
  'corecg/SkBuffer.cpp',
  'corecg/SkChunkAlloc.cpp',
  'corecg/SkCordic.cpp',
  'corecg/SkDebug.cpp',
  'corecg/SkDebug_stdio.cpp',
  'corecg/SkFloat.cpp',
  'corecg/SkInterpolator.cpp',
  'corecg/SkMath.cpp',
  'corecg/SkMatrix.cpp',
  'corecg/SkMemory_stdlib.cpp',
  'corecg/SkPoint.cpp',
  'corecg/SkRect.cpp',
  'corecg/SkRegion.cpp',
  'effects/Sk1DPathEffect.cpp',
  'effects/Sk2DPathEffect.cpp',
  'effects/SkAvoidXfermode.cpp',
  'effects/SkBlurDrawLooper.cpp',
  'effects/SkBlurMask.cpp',
  'effects/SkBlurMaskFilter.cpp',
  'effects/SkCamera.cpp',
  'effects/SkColorFilters.cpp',
  'effects/SkColorMatrix.cpp',
  'effects/SkColorMatrixFilter.cpp',
  'effects/SkCornerPathEffect.cpp',
  'effects/SkCullPoints.cpp',
  'effects/SkDashPathEffect.cpp',
  'effects/SkDiscretePathEffect.cpp',
  'effects/SkEmbossMask.cpp',
  'effects/SkEmbossMaskFilter.cpp',
  'effects/SkGradientShader.cpp',
  'effects/SkKernel33MaskFilter.cpp',
  'effects/SkLayerRasterizer.cpp',
  'effects/SkPaintFlagsDrawFilter.cpp',
  'effects/SkPixelXorXfermode.cpp',
  'effects/SkShaderExtras.cpp',
  'effects/SkTransparentShader.cpp',
  'effects/SkUnitMappers.cpp',
  'images/SkImageDecoder.cpp',
  'images/SkImageRef.cpp',
  'images/SkStream.cpp',
  'images/SkStream.cpp',
  'picture/SkPictureFlat.cpp',
  'picture/SkPicturePlayback.cpp',
  'picture/SkPictureRecord.cpp',
  'ports/SkFontHost_none.cpp',
  'ports/SkGlobals_global.cpp',
  'ports/SkImageDecoder_Factory.cpp',
  'ports/SkOSFile_stdio.cpp',
  'sgl/SkAlphaRuns.cpp',
  'sgl/SkBitmap.cpp',
  'sgl/SkBitmapProcShader.cpp',
  'sgl/SkBitmapProcState.cpp',
  'sgl/SkBitmapProcState_matrixProcs.cpp',
  'sgl/SkBitmapSampler.cpp',
  'sgl/SkBitmapShader.cpp',
  'sgl/SkBlitRow_D16.cpp',
  'sgl/SkBlitRow_D4444.cpp',
  'sgl/SkBlitter.cpp',
  'sgl/SkBlitter_4444.cpp',
  'sgl/SkBlitter_A1.cpp',
  'sgl/SkBlitter_A8.cpp',
  'sgl/SkBlitter_ARGB32.cpp',
  'sgl/SkBlitter_RGB16.cpp',
  'sgl/SkBlitter_Sprite.cpp',
  'sgl/SkCanvas.cpp',
  'sgl/SkColor.cpp',
  'sgl/SkColorFilter.cpp',
  'sgl/SkColorTable.cpp',
  'sgl/SkDeque.cpp',
  'sgl/SkDevice.cpp',
  'sgl/SkDither.cpp',
  'sgl/SkDraw.cpp',
  'sgl/SkEdge.cpp',
  'sgl/SkFilterProc.cpp',
  'sgl/SkFlattenable.cpp',
  'sgl/SkGeometry.cpp',
  'sgl/SkGlobals.cpp',
  'sgl/SkGlyphCache.cpp',
  'sgl/SkGraphics.cpp',
  'sgl/SkMask.cpp',
  'sgl/SkMaskFilter.cpp',
  'sgl/SkPackBits.cpp',
  'sgl/SkPaint.cpp',
  'sgl/SkPath.cpp',
  'sgl/SkPathEffect.cpp',
  'sgl/SkPathMeasure.cpp',
  'sgl/SkPicture.cpp',
  'sgl/SkPixelRef.cpp',
  'sgl/SkProcSpriteBlitter.cpp',
  'sgl/SkPtrRecorder.cpp',
  'sgl/SkRasterizer.cpp',
  'sgl/SkRefCnt.cpp',
  'sgl/SkRegion_path.cpp',
  'sgl/SkScalerContext.cpp',
  'sgl/SkScan.cpp',
  'sgl/SkScan_Antihair.cpp',
  'sgl/SkScan_AntiPath.cpp',
  'sgl/SkScan_Hairline.cpp',
  'sgl/SkScan_Path.cpp',
  'sgl/SkShader.cpp',
  'sgl/SkSpriteBlitter_ARGB32.cpp',
  'sgl/SkSpriteBlitter_RGB16.cpp',
  'sgl/SkString.cpp',
  'sgl/SkStroke.cpp',
  'sgl/SkStrokerPriv.cpp',
  'sgl/SkTSearch.cpp',
  'sgl/SkTypeface_fake.cpp',
  'sgl/SkUtils.cpp',
  'sgl/SkWriter32.cpp',
  'sgl/SkXfermode.cpp',
]

if env['PLATFORM'] in ('darwin', 'posix'):
  input_files.append('ports/SkThread_pthread.cpp')

if env['PLATFORM'] == 'win32':
  input_files.append('ports/SkThread_win.cpp')

  env_p = env.Clone(
    PCHSTOP = 'SkTypes.h',
    PDB = 'vc80.pdb',
  )

  pch, obj = env_p.PCH(['skia.pch', 'precompiled.obj'], 'precompiled.cc')
  env_p['PCH'] = pch
  input_files += [obj]

env.Library('skia', input_files)

