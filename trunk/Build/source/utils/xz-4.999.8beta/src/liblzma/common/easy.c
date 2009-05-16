///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy.c
/// \brief      Easy Stream encoder initialization
//
//  Copyright (C) 2008 Lasse Collin
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////

#include "stream_encoder.h"


struct lzma_coder_s {
	lzma_next_coder stream_encoder;

	/// Options for LZMA2
	lzma_options_lzma opt_lzma;

	/// We need to keep the filters array available in case
	/// LZMA_FULL_FLUSH is used.
	lzma_filter filters[LZMA_FILTERS_MAX + 1];
};


static bool
easy_set_filters(lzma_coder *coder, uint32_t preset)
{
	if (lzma_lzma_preset(&coder->opt_lzma, preset))
		return true;

	coder->filters[0].id = LZMA_FILTER_LZMA2;
	coder->filters[0].options = &coder->opt_lzma;
	coder->filters[1].id = LZMA_VLI_UNKNOWN;

	return false;
}


static lzma_ret
easy_encode(lzma_coder *coder, lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size, lzma_action action)
{
	return coder->stream_encoder.code(
			coder->stream_encoder.coder, allocator,
			in, in_pos, in_size, out, out_pos, out_size, action);
}


static void
easy_encoder_end(lzma_coder *coder, lzma_allocator *allocator)
{
	lzma_next_end(&coder->stream_encoder, allocator);
	lzma_free(coder, allocator);
	return;
}


static lzma_ret
easy_encoder_init(lzma_next_coder *next, lzma_allocator *allocator,
		uint32_t preset, lzma_check check)
{
	lzma_next_coder_init(easy_encoder_init, next, allocator);

	if (next->coder == NULL) {
		next->coder = lzma_alloc(sizeof(lzma_coder), allocator);
		if (next->coder == NULL)
			return LZMA_MEM_ERROR;

		next->code = &easy_encode;
		next->end = &easy_encoder_end;

		next->coder->stream_encoder = LZMA_NEXT_CODER_INIT;
	}

	if (easy_set_filters(next->coder, preset))
		return LZMA_OPTIONS_ERROR;

	return lzma_stream_encoder_init(&next->coder->stream_encoder,
			allocator, next->coder->filters, check);
}


extern LZMA_API(lzma_ret)
lzma_easy_encoder(lzma_stream *strm, uint32_t preset, lzma_check check)
{
	lzma_next_strm_init(easy_encoder_init, strm, preset, check);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_SYNC_FLUSH] = true;
	strm->internal->supported_actions[LZMA_FULL_FLUSH] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}


extern LZMA_API(uint64_t)
lzma_easy_encoder_memusage(uint32_t preset)
{
	lzma_coder coder;
	if (easy_set_filters(&coder, preset))
		return UINT32_MAX;

	return lzma_raw_encoder_memusage(coder.filters);
}


extern LZMA_API(uint64_t)
lzma_easy_decoder_memusage(uint32_t preset)
{
	lzma_coder coder;
	if (easy_set_filters(&coder, preset))
		return UINT32_MAX;

	return lzma_raw_decoder_memusage(coder.filters);
}
