///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_flags_decoder.c
/// \brief      Decodes Stream Header and Stream Footer from .xz files
//
//  Copyright (C) 2007 Lasse Collin
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

#include "stream_flags_common.h"


static bool
stream_flags_decode(lzma_stream_flags *options, const uint8_t *in)
{
	// Reserved bits must be unset.
	if (in[0] != 0x00 || (in[1] & 0xF0))
		return true;

	options->version = 0;
	options->check = in[1] & 0x0F;

	return false;
}


extern LZMA_API(lzma_ret)
lzma_stream_header_decode(lzma_stream_flags *options, const uint8_t *in)
{
	// Magic
	if (memcmp(in, lzma_header_magic, sizeof(lzma_header_magic)) != 0)
		return LZMA_FORMAT_ERROR;

	// Verify the CRC32 so we can distinguish between corrupt
	// and unsupported files.
	const uint32_t crc = lzma_crc32(in + sizeof(lzma_header_magic),
			LZMA_STREAM_FLAGS_SIZE, 0);
	if (crc != integer_read_32(in + sizeof(lzma_header_magic)
			+ LZMA_STREAM_FLAGS_SIZE))
		return LZMA_DATA_ERROR;

	// Stream Flags
	if (stream_flags_decode(options, in + sizeof(lzma_header_magic)))
		return LZMA_OPTIONS_ERROR;

	// Set Backward Size to indicate unknown value. That way
	// lzma_stream_flags_compare() can be used to compare Stream Header
	// and Stream Footer while keeping it useful also for comparing
	// two Stream Footers.
	options->backward_size = LZMA_VLI_UNKNOWN;

	return LZMA_OK;
}


extern LZMA_API(lzma_ret)
lzma_stream_footer_decode(lzma_stream_flags *options, const uint8_t *in)
{
	// Magic
	if (memcmp(in + sizeof(uint32_t) * 2 + LZMA_STREAM_FLAGS_SIZE,
			lzma_footer_magic, sizeof(lzma_footer_magic)) != 0)
		return LZMA_FORMAT_ERROR;

	// CRC32
	const uint32_t crc = lzma_crc32(in + sizeof(uint32_t),
			sizeof(uint32_t) + LZMA_STREAM_FLAGS_SIZE, 0);
	if (crc != integer_read_32(in))
		return LZMA_DATA_ERROR;

	// Stream Flags
	if (stream_flags_decode(options, in + sizeof(uint32_t) * 2))
		return LZMA_OPTIONS_ERROR;

	// Backward Size
	options->backward_size = integer_read_32(in + sizeof(uint32_t));
	options->backward_size = (options->backward_size + 1) * 4;

	return LZMA_OK;
}
