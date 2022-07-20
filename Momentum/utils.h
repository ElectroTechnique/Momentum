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

//For debug purposes, prints string of parameter values
// FLASHMEM String getCurrentPatchDataWithoutPatchname()
// {
//   auto p = groupvec[activeGroupIndex]->params();
//   return String(groupvec[activeGroupIndex]->getOscLevelA()) + "," + String(groupvec[activeGroupIndex]->getOscLevelB()) + "," + String(groupvec[activeGroupIndex]->getPinkNoiseLevel() - groupvec[activeGroupIndex]->getWhiteNoiseLevel()) + "," +
//          String(p.unisonMode) + "," + String(groupvec[activeGroupIndex]->getOscFX()) + "," + String(p.detune, 5) + "," + String(lfoSyncFreq) + "," + String(midiClkTimeInterval) + "," + String(lfoTempoValue) + "," +
//          String(groupvec[activeGroupIndex]->getKeytrackingAmount()) + "," + String(p.glideSpeed, 5) + "," + String(p.oscPitchA) + "," + String(p.oscPitchB) + "," + String(groupvec[activeGroupIndex]->getWaveformA()) + "," + String(groupvec[activeGroupIndex]->getWaveformB()) + "," +
//          String(groupvec[activeGroupIndex]->getPwmSourceA()) + "," + String(groupvec[activeGroupIndex]->getPwmSourceB()) + "," + String(groupvec[activeGroupIndex]->getPwmAmtA()) + "," + String(groupvec[activeGroupIndex]->getPwmAmtB()) + "," + String(groupvec[activeGroupIndex]->getPwmRateA()) + "," + String(groupvec[activeGroupIndex]->getPwmRateB()) + "," +
//          String(groupvec[activeGroupIndex]->getPwA()) + "," + String(groupvec[activeGroupIndex]->getPwB()) + "," + String(groupvec[activeGroupIndex]->getResonance()) + "," + String(groupvec[activeGroupIndex]->getCutoff()) + "," +
//          String(groupvec[activeGroupIndex]->getFilterMixer()) + "," + String(groupvec[activeGroupIndex]->getFilterEnvelope()) + "," + String(groupvec[activeGroupIndex]->getPitchLfoAmount(), 5) + "," +
//          String(groupvec[activeGroupIndex]->getPitchLfoRate(), 5) + "," + String(groupvec[activeGroupIndex]->getPitchLfoWaveform()) + "," + String(int(groupvec[activeGroupIndex]->getPitchLfoRetrig())) + "," +
//          String(int(groupvec[activeGroupIndex]->getPitchLfoMidiClockSync())) + "," + String(groupvec[activeGroupIndex]->getFilterLfoRate(), 5) + "," +
//          groupvec[activeGroupIndex]->getFilterLfoRetrig() + "," + groupvec[activeGroupIndex]->getFilterLfoMidiClockSync() + "," + groupvec[activeGroupIndex]->getFilterLfoAmt() + "," + groupvec[activeGroupIndex]->getFilterLfoWaveform() + "," + groupvec[activeGroupIndex]->getFilterAttack() + "," +
//          groupvec[activeGroupIndex]->getFilterDecay() + "," + groupvec[activeGroupIndex]->getFilterSustain() + "," + groupvec[activeGroupIndex]->getFilterRelease() + "," + groupvec[activeGroupIndex]->getAmpAttack() + "," + groupvec[activeGroupIndex]->getAmpDecay() + "," + groupvec[activeGroupIndex]->getAmpSustain() + "," + groupvec[activeGroupIndex]->getAmpRelease() + "," +
//          String(groupvec[activeGroupIndex]->getEffectAmount()) + "," + String(groupvec[activeGroupIndex]->getEffectMix()) + "," + String(groupvec[activeGroupIndex]->getPitchEnvelope()) + "," + String(velocitySens) + "," +
//          String(p.chordDetune) + "," + String(groupvec[activeGroupIndex]->getMonophonicMode()) + "," + String(0.0f);
// }

#endif
