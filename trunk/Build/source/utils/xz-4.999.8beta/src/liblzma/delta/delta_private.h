///////////////////////////////////////////////////////////////////////////////
//
/// \file       delta_private.h
/// \brief      Private common stuff for Delta encoder and decoder
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

#ifndef LZMA_DELTA_PRIVATE_H
#define LZMA_DELTA_PRIVATE_H

#include "delta_common.h"

struct lzma_coder_s {
	/// Next coder in the chain
	lzma_next_coder next;

	/// Delta distance
	size_t distance;

	/// Position in history[]
	uint8_t pos;

	/// Buffer to hold history of the original data
	uint8_t history[LZMA_DELTA_DIST_MAX];
};


extern lzma_ret lzma_delta_coder_init(
		lzma_next_coder *next, lzma_allocator *allocator,
		const lzma_filter_info *filters, lzma_code_function code);

#endif
