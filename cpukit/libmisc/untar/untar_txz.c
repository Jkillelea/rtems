/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 Chris Johns <chrisj.rtems.org>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/untar.h>

int Untar_XzChunkContext_Init(
  Untar_XzChunkContext *ctx,
  enum xz_mode mode,
  uint32_t dict_max,
  void *inflateBuffer,
  size_t inflateBufferSize
)
{
  int status = UNTAR_SUCCESSFUL;

  xz_crc32_init();

  Untar_ChunkContext_Init(&ctx->base);
  ctx->inflateBuffer = inflateBuffer;
  ctx->inflateBufferSize = inflateBufferSize;
  ctx->strm = xz_dec_init(mode, dict_max);
  if (ctx->strm == NULL) {
    status = UNTAR_FAIL;
  }

  return status;
}

int Untar_FromXzChunk_Print(
  Untar_XzChunkContext *ctx,
  const void *chunk,
  size_t chunk_size,
  const rtems_printer *printer
)
{
  int untar_status = UNTAR_SUCCESSFUL;
  enum xz_ret status = XZ_OK;

  if (ctx->strm == NULL)
    return UNTAR_FAIL;

  ctx->buf.in = (const uint8_t*) chunk;
  ctx->buf.in_pos = 0;
  ctx->buf.in_size = chunk_size;
  ctx->buf.out = (uint8_t *) ctx->inflateBuffer;

  /* Inflate until output buffer is not full */
  do {
    ctx->buf.out_pos = 0;
    ctx->buf.out_size = ctx->inflateBufferSize;
    status = xz_dec_run(ctx->strm, &ctx->buf);
    if (status == XZ_OPTIONS_ERROR)
      status = XZ_OK;
    if (status == XZ_OK && ctx->buf.out_pos != 0) {
      untar_status = Untar_FromChunk_Print(&ctx->base,
                                           ctx->inflateBuffer,
                                           ctx->buf.out_pos,
                                           NULL);
      if (untar_status != UNTAR_SUCCESSFUL) {
        break;
      }
    }
  } while (ctx->buf.in_pos != ctx->buf.in_size && status == XZ_OK);

  if (status != XZ_OK) {
    xz_dec_end(ctx->strm);
    ctx->strm = NULL;
    if (untar_status == UNTAR_SUCCESSFUL) {
      switch (status) {
      case XZ_OK:
      case XZ_STREAM_END:
        break;
      case XZ_UNSUPPORTED_CHECK:
        rtems_printf(printer, "XZ unsupported check\n");
        break;
      case XZ_MEM_ERROR:
        rtems_printf(printer, "XZ memory allocation error\n");
        break;
      case XZ_MEMLIMIT_ERROR:
        rtems_printf(printer, "XZ memory usage limit reached\n");
        break;
      case XZ_FORMAT_ERROR:
        rtems_printf(printer, "Not a XZ file\n");
        break;
      case XZ_OPTIONS_ERROR:
        rtems_printf(printer, "Unsupported options in XZ header\n");
        break;
      case XZ_DATA_ERROR:
        rtems_printf(printer, "XZ file is corrupt (data)\n");
        break;
      case XZ_BUF_ERROR:
        rtems_printf(printer, "XZ file is corrupt (buffer)\n");
        break;
      }
      untar_status = UNTAR_FAIL;
    }
  }

  return untar_status;
}
