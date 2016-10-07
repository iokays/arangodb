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

#include "IndexElement.h"
#include "Indexes/IndexLookupContext.h"

using namespace arangodb;

IndexElement::IndexElement(TRI_voc_rid_t revisionId, size_t numSubs) 
    : _revisionId(revisionId) {
    
  for (size_t i = 0; i < numSubs; ++i) {
    subObject(i)->fill(arangodb::velocypack::Slice::noneSlice());
  }
}

/// @brief allocate a new index element
IndexElement* IndexElement::create(TRI_voc_rid_t revisionId, std::vector<arangodb::velocypack::Slice> const& values) {
  TRI_ASSERT(!values.empty());

  IndexElement* element = create(revisionId, values.size());

  if (element == nullptr) {
    return nullptr;
  }

  try {
    for (size_t i = 0; i < values.size(); ++i) {
      element->subObject(i)->fill(values[i]);
    }
    return element;
  } catch (...) {
    element->free(values.size());
    return nullptr;
  }
}

/// @brief allocate a new index element from a slice
IndexElement* IndexElement::create(TRI_voc_rid_t revisionId, arangodb::velocypack::Slice const& value) {
  IndexElement* element = create(revisionId, 1);
  
  if (element == nullptr) {
    return nullptr;
  }

  try {
    element->subObject(0)->fill(value);
    return element;
  } catch (...) {
    element->free(1);
    return nullptr;
  }
}

IndexElement* IndexElement::create(TRI_voc_rid_t revisionId, size_t numSubs) {
  void* space = TRI_Allocate(TRI_UNKNOWN_MEM_ZONE, baseMemoryUsage(numSubs), false);

  if (space == nullptr) {
    return nullptr;
  }

  // will not fail
  return new (space) IndexElement(revisionId, numSubs);
}

void IndexElement::free(size_t numSubs) {
  for (size_t i = 0; i < numSubs; ++i) {
    subObject(i)->free();
  }
  TRI_Free(TRI_UNKNOWN_MEM_ZONE, this);
}
   
SimpleIndexElement::SimpleIndexElement(TRI_voc_rid_t revisionId, arangodb::velocypack::Slice const& value, uint32_t offset)
    : _revisionId(revisionId), _hashAndOffset(hash(value) | (static_cast<uint64_t>(offset) << 32)) {} 
  
uint64_t SimpleIndexElement::hash(arangodb::velocypack::Slice const& value) {
  TRI_ASSERT(value.isString());
  return value.hashString() & 0x00000000FFFFFFFFULL;
}

VPackSlice SimpleIndexElement::slice(IndexLookupContext* context) const {
  uint8_t const* vpack = context->lookup(_revisionId);
  if (vpack == nullptr) {
    THROW_ARANGO_EXCEPTION(TRI_ERROR_ARANGO_DOCUMENT_NOT_FOUND);
  } 
  return VPackSlice(vpack + offset());
}

