/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#if !defined(_facebook_ch_hash_h_)
#define _facebook_ch_hash_h_

#include <inttypes.h>
#include <sys/types.h>

#include "mcrouter/lib/fbi/decls.h"

__BEGIN_DECLS

/**
 * furc_hash -- a stateless consistent hash function
 *
 * This function accepts a |key| of length |len| and a value |m| that
 * establishes the range of output to be [0 : (m-1)].  The result will be
 * uniformly distributed within that range based on the key, and has the
 * property that changes in |m| will produce the minimum amount of re-
 * distribution of keys.
 *
 * For example, if |m| is increased from 11 to 12, 1/12th of keys for each
 * output value [0 : 10] will be reassigned the value of 11 while the
 * remaining 11/12th of keys will produce the same value as before.
 *
 * On average, a call to this function will take less than 400ns for |m| up to
 * 131071 (average key length 13 bytes); but there is a small chance that it
 * will take several times this -- up to 4us in very rare cases.  It uses
 * MurmurHash64A() internally, a hash function with both fast performance
 * and excellent statistical properties.  This endows furc_hash() with good
 * performance even for longer keys.
 *
 * This is adapted from the fbomb source, which included the
 * following copyright notice:
 *
 * Copyright (c) 2009- Facebook -- All Rights Reserved
 */

uint32_t furc_hash(const char* const key, const size_t len,
                   const uint32_t m);

/**
 * Same as furc_hash but doesn't allocate 8KB array on stack
 *
 * @param hash  should have at least FURC_CACHE_SIZE elements
 */
uint32_t furc_hash_array(const char* const key, const size_t len,
                         const uint32_t m, uint64_t* hash);

uint32_t furc_maximum_pool_size(void);

/**
 * MurmurHash2, 64-bit versions, by Austin Appleby
 *
 * The same caveats as 32-bit MurmurHash2 apply here - beware of alignment
 * and endian-ness issues if used across multiple platforms.
 *
 * 64-bit hash for 64-bit platforms
 */
uint64_t murmur_hash_64A(const void* const key, const size_t len,
                         const uint32_t seed);

/**
 * Computes a CRC32 hash of the first |len| characters of the |key|.
 */
uint32_t crc32_hash(const char* const key, const size_t len);

/* Maximum number tries for in-range result before just returning 0. */
#define FURC_MAX_TRIES 32

/* Gap in bit index per try; limits us to 2^FURC_SHIFT shards.  Making this
 * larger will sacrifice a modest amount of performance.
 */
#define FURC_SHIFT 23

/* Size of cache for hash values; should be > MAXTRIES * (FURCSHIFT + 1) */
#define FURC_CACHE_SIZE 1024

__END_DECLS

#endif /* #if !defined(_facebook_ch_hash_h_) */
