/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _HLL6ARRAY_INTERNAL_HPP_
#define _HLL6ARRAY_INTERNAL_HPP_

#include <cstring>

#include "Hll6Array.hpp"

namespace datasketches {

template<typename A>
Hll6Iterator<A>::Hll6Iterator(const Hll6Array<A>& hllArray, const int lengthPairs)
  : HllPairIterator<A>(lengthPairs),
    hllArray(hllArray),
    bitOffset(-6)
{}

template<typename A>
Hll6Iterator<A>::~Hll6Iterator() { }

template<typename A>
int Hll6Iterator<A>::value() {
  bitOffset += 6;
  const int shift = bitOffset & 0x7;
  const int byteIdx = bitOffset >> 3;
  const uint16_t twoByteVal = (hllArray.hllByteArr[byteIdx + 1] << 8)
                              | hllArray.hllByteArr[byteIdx];
  return (uint8_t) (twoByteVal >> shift) & 0x3F;
}

template<typename A>
Hll6Array<A>::Hll6Array(const int lgConfigK, const bool startFullSize) :
    HllArray<A>(lgConfigK, target_hll_type::HLL_6, startFullSize) {
  const int numBytes = this->hll6ArrBytes(lgConfigK);
  typedef typename std::allocator_traits<A>::template rebind_alloc<uint8_t> uint8Alloc;
  this->hllByteArr = uint8Alloc().allocate(numBytes);
  std::fill(this->hllByteArr, this->hllByteArr + numBytes, 0);
}

template<typename A>
Hll6Array<A>::Hll6Array(const Hll6Array<A>& that) :
  HllArray<A>(that)
{
  // can determine hllByteArr size in parent class, no need to allocate here
}

template<typename A>
Hll6Array<A>::~Hll6Array() {
  // hllByteArr deleted in parent
}

template<typename A>
std::function<void(HllSketchImpl<A>*)> Hll6Array<A>::get_deleter() const {
  return [](HllSketchImpl<A>* ptr) {
    typedef typename std::allocator_traits<A>::template rebind_alloc<Hll6Array<A>> hll6Alloc;
    Hll6Array<A>* hll = static_cast<Hll6Array<A>*>(ptr);
    hll->~Hll6Array();
    hll6Alloc().deallocate(hll, 1);
  };
}

template<typename A>
Hll6Array<A>* Hll6Array<A>::copy() const {
  typedef typename std::allocator_traits<A>::template rebind_alloc<Hll6Array<A>> hll6Alloc;
  return new (hll6Alloc().allocate(1)) Hll6Array<A>(*this);
}

template<typename A>
pair_iterator_with_deleter<A> Hll6Array<A>::getIterator() const {
  typedef typename std::allocator_traits<A>::template rebind_alloc<Hll6Iterator<A>> itrAlloc;
  Hll6Iterator<A>* itr = new (itrAlloc().allocate(1)) Hll6Iterator<A>(*this, 1 << this->lgConfigK);
  return pair_iterator_with_deleter<A>(
    itr,
    [](PairIterator<A>* ptr) {
      Hll6Iterator<A>* hll = static_cast<Hll6Iterator<A>*>(ptr);
      hll->~Hll6Iterator();
      itrAlloc().deallocate(hll, 1);
    }
  );
}

template<typename A>
int Hll6Array<A>::getSlot(const int slotNo) const {
  const int startBit = slotNo * 6;
  const int shift = startBit & 0x7;
  const int byteIdx = startBit >> 3;  
  const uint16_t twoByteVal = (this->hllByteArr[byteIdx + 1] << 8) | this->hllByteArr[byteIdx];
  return (uint8_t) (twoByteVal >> shift) & 0x3F;
}

template<typename A>
void Hll6Array<A>::putSlot(const int slotNo, const int value) {
  const int startBit = slotNo * 6;
  const int shift = startBit & 0x7;
  const int byteIdx = startBit >> 3;
  const uint16_t valShifted = (value & 0x3F) << shift;
  uint16_t curMasked = (this->hllByteArr[byteIdx + 1] << 8) | this->hllByteArr[byteIdx];
  curMasked &= (~(HllUtil<A>::VAL_MASK_6 << shift));
  uint16_t insert = curMasked | valShifted;
  this->hllByteArr[byteIdx]     = insert & 0xFF;
  this->hllByteArr[byteIdx + 1] = (insert & 0xFF00) >> 8;
}

template<typename A>
int Hll6Array<A>::getHllByteArrBytes() const {
  return this->hll6ArrBytes(this->lgConfigK);
}

template<typename A>
HllSketchImpl<A>* Hll6Array<A>::couponUpdate(const int coupon) {
  const int configKmask = (1 << this->lgConfigK) - 1;
  const int slotNo = HllUtil<A>::getLow26(coupon) & configKmask;
  const int newVal = HllUtil<A>::getValue(coupon);
  if (newVal <= 0) {
    throw std::logic_error("newVal must be a positive integer: " + std::to_string(newVal));
  }

  const int curVal = getSlot(slotNo);
  if (newVal > curVal) {
    putSlot(slotNo, newVal);
    this->hipAndKxQIncrementalUpdate(*this, curVal, newVal);
    if (curVal == 0) {
      this->decNumAtCurMin(); // interpret numAtCurMin as num zeros
      if (this->getNumAtCurMin() < 0) { 
        throw std::logic_error("getNumAtCurMin() must return a nonnegative integer: " + std::to_string(this->getNumAtCurMin()));
      }
    }
  }
  return this;
}

}

#endif // _HLL6ARRAY_INTERNAL_HPP_
