//========================================================================
//
// PopplerCache.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright (C) 2009 Koji Otani <sho@bbr.jp>
// Copyright (C) 2009 Albert Astals Cid <aacid@kde.org>
//
//========================================================================

#ifndef POPPLER_CACHE_H
#define POPPLER_CACHE_H

class PopplerCacheItem
{
  public:
   virtual ~PopplerCacheItem();
};

class PopplerCacheKey
{
  public:
    virtual ~PopplerCacheKey();
    virtual bool operator==(const PopplerCacheKey &key) const = 0;
};

class PopplerCache
{
  public:
    PopplerCache(int cacheSizeA);
    ~PopplerCache();
    
    /* The item returned is owned by the cache */
    PopplerCacheItem *lookup(const PopplerCacheKey &key);
    
    /* The key and item pointers ownership is taken by the cache */
    void put(PopplerCacheKey *key, PopplerCacheItem *item);
    
    /* The max size of the cache */
    int size();
    
    /* The number of items in the cache */
    int numberOfItems();
    
    /* The n-th item in the cache */
    PopplerCacheItem *item(int index);
    
    /* The n-th key in the cache */
    PopplerCacheKey *key(int index);
  
  private:
    PopplerCache(const PopplerCache &cache); // not allowed
  
    PopplerCacheKey **keys;
    PopplerCacheItem **items;
    int lastValidCacheIndex;
    int cacheSize;
};

#endif
