///////////////////////////////////////////////////////////////////////////////
//
/// \file       xzdec.c
/// \brief      Simple single-threaded tool to uncompress .xz or .lzma files
//
//  Copyright (C) 2007 Lasse Collin
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////

#include "sysdefs.h"
#include "lzma.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#ifdef _WIN32
#	include <fcntl.h>
#endif

#include "getopt.h"
#include "physmem.h"


#ifdef LZMADEC
#	define TOOL_FORMAT "lzma"
#else
#	define TOOL_FORMAT "xz"
#endif


/// Number of bytes to use memory at maximum
static uint64_t memlimit;

/// Program name to be shown in error messages
static const char *argv0;


static void lzma_attribute((noreturn))
my_exit(void)
{
	int status = EXIT_SUCCESS;

	// Close stdout. We don't care about stderr, because we write to it
	// only when an error has already occurred.
	const int ferror_err = ferror(stdout);
	const int fclose_err = fclose(stdout);

	if (ferror_err || fclose_err) {
		// If it was fclose() that failed, we have the reason
		// in errno. If only ferror() indicated an error,
		// we have no idea what the reason was.
		fprintf(stderr, "%s: Cannot write to standard output: %s\n",
				argv0, fclose_err
					? strerror(errno) : "Unknown error");
		status = EXIT_FAILURE;
	}

	exit(status);
}


static void lzma_attribute((noreturn))
help(void)
{
	printf(
"Usage: %s [OPTION]... [FILE]...\n"
"Uncompress files in the ." TOOL_FORMAT " format to the standard output.\n"
"\n"
"  -c, --stdout       (ignored)\n"
"  -d, --decompress   (ignored)\n"
"  -k, --keep         (ignored)\n"
"  -f, --force        (ignored)\n"
"  -M, --memory=NUM   use NUM bytes of memory at maximum (0 means default);\n"
"                     the suffixes k, M, G, Ki, Mi, and Gi are supported.\n"
"  -h, --help         display this help and exit\n"
"  -V, --version      display version and license information and exit\n"
"\n"
"With no FILE, or when FILE is -, read standard input.\n"
"\n"
"On this configuration, the tool will use about %" PRIu64
		" MiB of memory at maximum.\n"
"\n"
"Report bugs to <" PACKAGE_BUGREPORT "> (in English or Finnish).\n",
		argv0, (memlimit + 512 * 1024) / (1024 * 1024));
	my_exit();
}


static void lzma_attribute((noreturn))
version(void)
{
	printf(TOOL_FORMAT "dec " PACKAGE_VERSION "\n"
			"liblzma %s\n", lzma_version_string());

	my_exit();
}


/// Finds out the amount of physical memory in the system, and sets
/// a default memory usage limit.
static void
set_default_memlimit(void)
{
	const uint64_t mem = physmem();

	if (mem == 0)
		// Cannot autodetect, use 10 MiB as the default limit.
		memlimit = (1U << 23) + (1U << 21);
	else
		// Limit is 33 % of RAM.
		memlimit = mem / 3;

	return;
}


/// \brief      Converts a string to uint64_t
///
/// This is rudely copied from src/xz/util.c and modified a little. :-(
///
static uint64_t
str_to_uint64(const char *value)
{
	uint64_t result = 0;

	if (*value < '0' || *value > '9') {
		fprintf(stderr, "%s: %s: Not a number\n", argv0, value);
		exit(EXIT_FAILURE);
	}

	do {
		// Don't overflow.
		if (result > (UINT64_MAX - 9) / 10)
			return UINT64_MAX;

		result *= 10;
		result += *value - '0';
		++value;
	} while (*value >= '0' && *value <= '9');

	if (*value != '\0') {
		// Look for suffix.
		static const struct {
			const char name[4];
			uint32_t multiplier;
		} suffixes[] = {
			{ "k",   1000 },
			{ "kB",  1000 },
			{ "M",   1000000 },
			{ "MB",  1000000 },
			{ "G",   1000000000 },
			{ "GB",  1000000000 },
			{ "Ki",  1024 },
			{ "KiB", 1024 },
			{ "Mi",  1048576 },
			{ "MiB", 1048576 },
			{ "Gi",  1073741824 },
			{ "GiB", 1073741824 }
		};

		uint32_t multiplier = 0;
		for (size_t i = 0; i < ARRAY_SIZE(suffixes); ++i) {
			if (strcmp(value, suffixes[i].name) == 0) {
				multiplier = suffixes[i].multiplier;
				break;
			}
		}

		if (multiplier == 0) {
			fprintf(stderr, "%s: %s: Invalid suffix\n",
					argv0, value);
			exit(EXIT_FAILURE);
		}

		// Don't overflow here either.
		if (result > UINT64_MAX / multiplier)
			result = UINT64_MAX;
		else
			result *= multiplier;
	}

	return result;
}


/// Parses command line options.
static void
parse_options(int argc, char **argv)
{
	static const char short_opts[] = "cdkfM:hV";
	static const struct option long_opts[] = {
		{ "stdout",       no_argument,         NULL, 'c' },
		{ "to-stdout",    no_argument,         NULL, 'c' },
		{ "decompress",   no_argument,         NULL, 'd' },
		{ "uncompress",   no_argument,         NULL, 'd' },
		{ "force",        no_argument,         NULL, 'f' },
		{ "keep",         no_argument,         NULL, 'k' },
		{ "memory",       required_argument,   NULL, 'M' },
		{ "help",         no_argument,         NULL, 'h' },
		{ "version",      no_argument,         NULL, 'V' },
		{ NULL,           0,                   NULL, 0   }
	};

	int c;

	while ((c = getopt_long(argc, argv, short_opts, long_opts, NULL))
			!= -1) {
		switch (c) {
		case 'c':
		case 'd':
		case 'f':
		case 'k':
			break;

		case 'M':
			memlimit = str_to_uint64(optarg);
			if (memlimit == 0)
				set_default_memlimit();

			break;

		case 'h':
			help();

		case 'V':
			version();

		default:
			exit(EXIT_FAILURE);
		}
	}

	return;
}


static void
uncompress(lzma_stream *strm, FILE *file, const char *filename)
{
	lzma_ret ret;

	// Initialize the decoder
#ifdef LZMADEC
	ret = lzma_alone_decoder(strm, memlimit);
#else
	ret = lzma_stream_decoder(strm, memlimit, LZMA_CONCATENATED);
#endif

	// The only reasonable error here is LZMA_MEM_ERROR.
	// FIXME: Maybe also LZMA_MEMLIMIT_ERROR in future?
	if (ret != LZMA_OK) {
		fprintf(stderr, "%s: ", argv0);

		if (ret == LZMA_MEM_ERROR)
			fprintf(stderr, "%s\n", strerror(ENOMEM));
		else
			fprintf(stderr, "Internal program error (bug)\n");

		exit(EXIT_FAILURE);
	}

	// Input and output buffers
	uint8_t in_buf[BUFSIZ];
	uint8_t out_buf[BUFSIZ];

	strm->avail_in = 0;
	strm->next_out = out_buf;
	strm->avail_out = BUFSIZ;

	lzma_action action = LZMA_RUN;

	while (true) {
		if (strm->avail_in == 0) {
			strm->next_in = in_buf;
			strm->avail_in = fread(in_buf, 1, BUFSIZ, file);

			if (ferror(file)) {
				// POSIX says that fread() sets errno if
				// an error occurred. ferror() doesn't
				// touch errno.
				fprintf(stderr, "%s: %s: Error reading "
						"input file: %s\n",
						argv0, filename,
						strerror(errno));
				exit(EXIT_FAILURE);
			}

#ifndef LZMADEC
			// When using LZMA_CONCATENATED, we need to tell
			// liblzma when it has got all the input.
			if (feof(file))
				action = LZMA_FINISH;
#endif
		}

		ret = lzma_code(strm, action);

		// Write and check write error before checking decoder error.
		// This way as much data as possible gets written to output
		// even if decoder detected an error.
		if (strm->avail_out == 0 || ret != LZMA_OK) {
			const size_t write_size = BUFSIZ - strm->avail_out;

			if (fwrite(out_buf, 1, write_size, stdout)
					!= write_size) {
				// Wouldn't be a surprise if writing to stderr
				// would fail too but at least try to show an
				// error message.
				fprintf(stderr, "%s: Cannot write to "
						"standard output: %s\n", argv0,
						strerror(errno));
				exit(EXIT_FAILURE);
			}

			strm->next_out = out_buf;
			strm->avail_out = BUFSIZ;
		}

		if (ret != LZMA_OK) {
			if (ret == LZMA_STREAM_END) {
#ifdef LZMADEC
				// Check that there's no trailing garbage.
				if (strm->avail_in != 0
						|| fread(in_buf, 1, 1, file)
							!= 0
						|| !feof(file))
					ret = LZMA_DATA_ERROR;
				else
					return;
#else
				// lzma_stream_decoder() already guarantees
				// that there's no trailing garbage.
				assert(strm->avail_in == 0);
				assert(action == LZMA_FINISH);
				assert(feof(file));
				return;
#endif
			}

			const char *msg;
			switch (ret) {
			case LZMA_MEM_ERROR:
				msg = strerror(ENOMEM);
				break;

			case LZMA_MEMLIMIT_ERROR:
				msg = "Memory usage limit reached";
				break;

			case LZMA_FORMAT_ERROR:
				msg = "File format not recognized";
				break;

			case LZMA_OPTIONS_ERROR:
				// FIXME: Better message?
				msg = "Unsupported compression options";
				break;

			case LZMA_DATA_ERROR:
				msg = "File is corrupt";
				break;

			case LZMA_BUF_ERROR:
				msg = "Unexpected end of input";
				break;

			default:
				msg = "Internal program error (bug)";
				break;
			}

			fprintf(stderr, "%s: %s: %s\n", argv0, filename, msg);

			exit(EXIT_FAILURE);
		}
	}
}


int
main(int argc, char **argv)
{
	// Set the argv0 global so that we can print the command name in
	// error and help messages.
	argv0 = argv[0];

	// Detect amount of installed RAM and set the memory usage limit.
	// This is needed before parsing the command line arguments.
	set_default_memlimit();

	// Parse the command line options.
	parse_options(argc, argv);

	// The same lzma_stream is used for all files that we decode. This way
	// we don't need to reallocate memory for every file if they use same
	// compression settings.
	lzma_stream strm = LZMA_STREAM_INIT;

	// Some systems require setting stdin and stdout to binary mode.
#ifdef _WIN32
	setmode(fileno(stdin), O_BINARY);
	setmode(fileno(stdout), O_BINARY);
#endif

	if (optind == argc) {
		// No filenames given, decode from stdin.
		uncompress(&strm, stdin, "(stdin)");
	} else {
		// Loop through the filenames given on the command line.
		do {
			// "-" indicates stdin.
			if (strcmp(argv[optind], "-") == 0) {
				uncompress(&strm, stdin, "(stdin)");
			} else {
				FILE *file = fopen(argv[optind], "rb");
				if (file == NULL) {
					fprintf(stderr, "%s: %s: %s\n",
							argv0, argv[optind],
							strerror(errno));
					exit(EXIT_FAILURE);
				}

				uncompress(&strm, file, argv[optind]);
				fclose(file);
			}
		} while (++optind < argc);
	}

#ifndef NDEBUG
	// Free the memory only when debugging. Freeing wastes some time,
	// but allows detecting possible memory leaks with Valgrind.
	lzma_end(&strm);
#endif

	my_exit();
}
