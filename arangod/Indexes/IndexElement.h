////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_INDEXES_INDEX_ELEMENT_H
#define ARANGOD_INDEXES_INDEX_ELEMENT_H 1

#include "Basics/Common.h"
#include "VocBase/vocbase.h"
#include "VocBase/voc-types.h"

namespace arangodb {

namespace velocypack {
class Slice;
}

class IndexLookupContext;

/// @brief velocypack sub-object (for indexes, as part of IndexElement, 
/// if the last byte in data[] is 0, then the VelocyPack data is managed 
/// by the index element. If the last byte in data[] is 1, then 
/// value.data contains the actual VelocyPack data in place.
struct IndexElementValue {
 public:
  IndexElementValue() {}
  ~IndexElementValue() {}

  /// @brief fill a IndexElementValue structure with a subvalue
  void fill(VPackSlice const value) {
    VPackValueLength len = value.byteSize();
    if (len <= maxValueLength()) {
      setValue(value.start(), static_cast<size_t>(len));
    } else {
      setManaged(value.start(), static_cast<size_t>(len));
    }
  }

  void free() {
    if (isManaged()) {
      delete[] value.managed.data;
    }
  }
  
  /// @brief velocypack sub-object (for indexes, as part of IndexElement, 
  /// if offset is non-zero, then it is an offset into the VelocyPack data in
  /// the data or WAL file. If offset is 0, then data contains the actual data
  /// in place.
  arangodb::velocypack::Slice slice() const {
    if (isValue()) {
      return arangodb::velocypack::Slice(&value.data[0]);
    } 
    return arangodb::velocypack::Slice(value.managed.data);
  }
  
  inline bool isManaged() const noexcept {
    return !isValue();
  }

  inline bool isValue() const noexcept {
    return value.data[maxValueLength()] == 1;
  }

 private:
  void setManaged(uint8_t const* data, size_t length) {
    value.managed.data = new uint8_t[length];
    value.managed.size = static_cast<uint32_t>(length);
    memcpy(value.managed.data, data, length);
    value.data[maxValueLength()] = 0; // type = offset
  }
    
  void setValue(uint8_t const* data, size_t length) noexcept {
    TRI_ASSERT(length > 0);
    TRI_ASSERT(length <= maxValueLength());
    memcpy(&value.data[0], data, length);
    value.data[maxValueLength()] = 1; // type = value
  }

  static constexpr size_t maxValueLength() noexcept {
    return sizeof(value.data) - 1;
  }
 
 private:
  union {
    uint8_t data[16];
    struct {
      uint8_t* data;
      uint32_t size;
    } managed;
  } value;
};

static_assert(sizeof(IndexElementValue) == 16, "invalid size of IndexElementValue");

/// @brief Unified index element. Do not directly construct it.
struct IndexElement {
  // Do not use new for this struct, use create()!
 private:
  IndexElement(TRI_voc_rid_t revisionId, size_t numSubs); 

  IndexElement() = delete;
  IndexElement(IndexElement const&) = delete;
  IndexElement& operator=(IndexElement const&) = delete;
  ~IndexElement() = delete;

 public:
  /// @brief get the revision id of the document
  inline TRI_voc_rid_t revisionId() const { return _revisionId; }
  
  inline IndexElementValue const* subObject(size_t position) const {
    char const* p = reinterpret_cast<char const*>(this) + sizeof(TRI_voc_rid_t) + position * sizeof(IndexElementValue);
    return reinterpret_cast<IndexElementValue const*>(p);
  }
  
  inline arangodb::velocypack::Slice slice(size_t position) const {
    return subObject(position)->slice();
  }
  
  inline arangodb::velocypack::Slice slice() const {
    return slice(0);
  }

  uint64_t hashString(uint64_t seed) const {
    return slice(0).hashString(seed);
  }

  /// @brief allocate a new index element from a vector of slices
  static IndexElement* create(TRI_voc_rid_t revisionId, std::vector<arangodb::velocypack::Slice> const& values);
  
  /// @brief allocate a new index element from a slice
  static IndexElement* create(TRI_voc_rid_t revisionId, arangodb::velocypack::Slice const& value);

  void free(size_t numSubs);
  
  /// @brief base memory usage of an index element
  static constexpr size_t baseMemoryUsage(size_t numSubs) {
    return sizeof(TRI_voc_rid_t) + (sizeof(IndexElementValue) * numSubs);
  }
  
 private:
  static IndexElement* create(TRI_voc_rid_t revisionId, size_t numSubs);

  inline IndexElementValue* subObject(size_t position) {
    char* p = reinterpret_cast<char*>(this) + baseMemoryUsage(position);
    return reinterpret_cast<IndexElementValue*>(p);
  }
  
 private:
  TRI_voc_rid_t _revisionId;
};

struct SimpleIndexElement {
 public:
  constexpr SimpleIndexElement() : _revisionId(0), _hashAndOffset(0) {}
  SimpleIndexElement(TRI_voc_rid_t revisionId, arangodb::velocypack::Slice const& value, uint32_t offset); 
  SimpleIndexElement(SimpleIndexElement const& other) : _revisionId(other._revisionId), _hashAndOffset(other._hashAndOffset) {}
  SimpleIndexElement& operator=(SimpleIndexElement const& other) {
    _revisionId = other._revisionId;
    _hashAndOffset = other._hashAndOffset;
    return *this;
  }

  /// @brief get the revision id of the document
  inline TRI_voc_rid_t revisionId() const { return _revisionId; }
  inline uint64_t hash() const { return _hashAndOffset & 0xFFFFFFFFULL; }
  inline uint32_t offset() const { return static_cast<uint32_t>((_hashAndOffset & 0xFFFFFFFF00000000ULL) >> 32); }
  arangodb::velocypack::Slice slice(IndexLookupContext*) const;
  
  inline operator bool() const { return _revisionId != 0; }
  inline bool operator==(SimpleIndexElement const& other) const {
    return _revisionId == other._revisionId && _hashAndOffset == other._hashAndOffset;
  }
   
  static uint64_t hash(arangodb::velocypack::Slice const& value);
  inline void updateRevisionId(TRI_voc_rid_t revisionId, uint32_t offset) { 
    _revisionId = revisionId; 
    _hashAndOffset &= 0xFFFFFFFFULL; 
    _hashAndOffset |= (static_cast<uint64_t>(offset) << 32);
  }
  
 private:
  TRI_voc_rid_t _revisionId;
  uint64_t _hashAndOffset;
};

class IndexElementGuard {
 public:
  IndexElementGuard(IndexElement* element, size_t numSubs) : _element(element), _numSubs(numSubs) {}
  ~IndexElementGuard() {
    if (_element != nullptr) {
      _element->free(_numSubs);
    }
  }
  IndexElement* get() const { return _element; }
  IndexElement* release() { 
    IndexElement* tmp = _element;
    _element = nullptr;
    return tmp;
  }

  operator bool() const { return _element != nullptr; }
  bool operator==(nullptr_t) const { return _element == nullptr; }
  bool operator!=(nullptr_t) const { return _element != nullptr; }

  IndexElement* operator->() { return _element; }
  IndexElement const* operator->() const { return _element; }

 private:
  IndexElement* _element;
  size_t const _numSubs;
};

class IndexLookupResult {
 public:
  constexpr IndexLookupResult() : _revisionId(0) {}
  explicit IndexLookupResult(TRI_voc_rid_t revisionId) : _revisionId(revisionId) {}
  IndexLookupResult(IndexLookupResult const& other) : _revisionId(other._revisionId) {}
  IndexLookupResult& operator=(IndexLookupResult const& other) {
    _revisionId = other._revisionId;
    return *this;
  }

  inline operator bool() const { return _revisionId != 0; }

  inline TRI_voc_rid_t revisionId() const { return _revisionId; }

 private:
  TRI_voc_rid_t _revisionId;
};

}

#endif
