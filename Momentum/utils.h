#ifndef MOMENTUM_UTILS_H
#define MOMENTUM_UTILS_H

#include <Arduino.h>
#include <RokkitHash.h>

// Clamp a value to the given range.
template <typename NumericType>
NumericType inRangeOrDefault(NumericType value, NumericType defaultValue, NumericType min, NumericType max)
{
  static_assert(std::is_arithmetic<NumericType>::value, "NumericType must be numeric");
  if (value < min || value > max)
    return defaultValue;
  return value;
}

// Set new value and clamp it to the given range.
template <typename NumericType>
NumericType clampToRange(NumericType value, NumericType delta, NumericType min, NumericType max)
{
  static_assert(std::is_arithmetic<NumericType>::value, "NumericType must be numeric");
  if ((value + delta) < min)
  {
    value = min;
  }
  else if ((value + delta) > max)
  {
    value = max;
  }
  else
  {
    value += delta;
  }
  return value;
}

String milliToString(float milli)
{
  if (milli < 1000)
    return String(int(milli)) + " ms";
  return String(milli / 1000) + " s";
}

// Computes a hash of the parameter values to create a UID for each patch/sequence/peformance that is stored with it.
// This can also be used to identify identical patches. Hash takes about 2.8us on TeensyMM
FLASHMEM uint32_t getHash(String tohash)
{
  return rokkit(tohash.c_str(), strlen(tohash.c_str()));
}

#endif
