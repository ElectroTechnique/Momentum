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

FLASHMEM String milliToString(float milli)
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

// For debug purposes, prints string of parameter values
//  FLASHMEM String getCurrentPatchDataWithoutPatchname()
//  {
//    auto p = groupvec[activeGroupIndex]->params();
//    return String(groupvec[activeGroupIndex]->getOscLevelA()) + "," + String(groupvec[activeGroupIndex]->getOscLevelB()) + "," + String(groupvec[activeGroupIndex]->getPinkNoiseLevel() - groupvec[activeGroupIndex]->getWhiteNoiseLevel()) + "," +
//           String(p.unisonMode) + "," + String(groupvec[activeGroupIndex]->getOscFX()) + "," + String(p.detune, 5) + "," + String(lfoSyncFreq) + "," + String(midiClkTimeInterval) + "," + String(lfoTempoValue) + "," +
//           String(groupvec[activeGroupIndex]->getKeytrackingAmount()) + "," + String(p.glideSpeed, 5) + "," + String(p.oscPitchA) + "," + String(p.oscPitchB) + "," + String(groupvec[activeGroupIndex]->getWaveformA()) + "," + String(groupvec[activeGroupIndex]->getWaveformB()) + "," +
//           String(groupvec[activeGroupIndex]->getPwmSourceA()) + "," + String(groupvec[activeGroupIndex]->getPwmSourceB()) + "," + String(groupvec[activeGroupIndex]->getPwmAmtA()) + "," + String(groupvec[activeGroupIndex]->getPwmAmtB()) + "," + String(groupvec[activeGroupIndex]->getPwmRateA()) + "," + String(groupvec[activeGroupIndex]->getPwmRateB()) + "," +
//           String(groupvec[activeGroupIndex]->getPwA()) + "," + String(groupvec[activeGroupIndex]->getPwB()) + "," + String(groupvec[activeGroupIndex]->getResonance()) + "," + String(groupvec[activeGroupIndex]->getCutoff()) + "," +
//           String(groupvec[activeGroupIndex]->getFilterMixer()) + "," + String(groupvec[activeGroupIndex]->getFilterEnvelope()) + "," + String(groupvec[activeGroupIndex]->getPitchLfoAmount(), 5) + "," +
//           String(groupvec[activeGroupIndex]->getPitchLfoRate(), 5) + "," + String(groupvec[activeGroupIndex]->getPitchLfoWaveform()) + "," + String(int(groupvec[activeGroupIndex]->getPitchLfoRetrig())) + "," +
//           String(int(groupvec[activeGroupIndex]->getPitchLfoMidiClockSync())) + "," + String(groupvec[activeGroupIndex]->getFilterLfoRate(), 5) + "," +
//           groupvec[activeGroupIndex]->getFilterLfoRetrig() + "," + groupvec[activeGroupIndex]->getFilterLfoMidiClockSync() + "," + groupvec[activeGroupIndex]->getFilterLfoAmt() + "," + groupvec[activeGroupIndex]->getFilterLfoWaveform() + "," + groupvec[activeGroupIndex]->getFilterAttack() + "," +
//           groupvec[activeGroupIndex]->getFilterDecay() + "," + groupvec[activeGroupIndex]->getFilterSustain() + "," + groupvec[activeGroupIndex]->getFilterRelease() + "," + groupvec[activeGroupIndex]->getAmpAttack() + "," + groupvec[activeGroupIndex]->getAmpDecay() + "," + groupvec[activeGroupIndex]->getAmpSustain() + "," + groupvec[activeGroupIndex]->getAmpRelease() + "," +
//           String(groupvec[activeGroupIndex]->getEffectAmount()) + "," + String(groupvec[activeGroupIndex]->getEffectMix()) + "," + String(groupvec[activeGroupIndex]->getPitchEnvelope()) + "," + String(velocitySens) + "," +
//           String(p.chordDetune) + "," + String(groupvec[activeGroupIndex]->getMonophonicMode()) + "," + String(0.0f);
//  }

// ONLY WORKS WITH TD1.58?

// https://forum.pjrc.com/threads/33443-How-to-display-free-ram
// void memInfo()
// {
//   constexpr auto RAM_BASE = 0x2020'0000;
//   constexpr auto RAM_SIZE = 512 << 10;
//   constexpr auto FLASH_BASE = 0x6000'0000;
// #if ARDUINO_TEENSY40
//   constexpr auto FLASH_SIZE = 2 << 20;
// #elif ARDUINO_TEENSY41
//   constexpr auto FLASH_SIZE = 8 << 20;
// #elif ARDUINO_TEENSY_MICROMOD
//    constexpr auto FLASH_SIZE = 16 << 20;
// #endif

//   // note: these values are defined by the linker, they are not valid memory
//   // locations in all cases - by defining them as arrays, the C++ compiler
//   // will use the address of these definitions - it's a big hack, but there's
//   // really no clean way to get at linker-defined symbols from the .ld file

//   extern char _stext[], _etext[], _sbss[], _ebss[], _sdata[], _edata[],
//       _estack[], _heap_start[], _heap_end[], _itcm_block_count[], *__brkval;

//   auto sp = (char *)__builtin_frame_address(0);

//   printf("_stext        %08x\n", _stext);
//   printf("_etext        %08x +%db\n", _etext, _etext - _stext);
//   printf("_sdata        %08x\n", _sdata);
//   printf("_edata        %08x +%db\n", _edata, _edata - _sdata);
//   printf("_sbss         %08x\n", _sbss);
//   printf("_ebss         %08x +%db\n", _ebss, _ebss - _sbss);
//   printf("curr stack    %08x +%db\n", sp, sp - _ebss);
//   printf("_estack       %08x +%db\n", _estack, _estack - sp);
//   printf("_heap_start   %08x\n", _heap_start);
//   printf("__brkval      %08x +%db\n", __brkval, __brkval - _heap_start);
//   printf("_heap_end     %08x +%db\n", _heap_end, _heap_end - __brkval);
// #if ARDUINO_TEENSY41
//   extern char _extram_start[], _extram_end[], *__brkval;
//   printf("_extram_start %08x\n", _extram_start);
//   printf("_extram_end   %08x +%db\n", _extram_end,
//          _extram_end - _extram_start);
// #endif
//   printf("\n");

//   printf("<ITCM>  %08x .. %08x\n",
//          _stext, _stext + ((int)_itcm_block_count << 15) - 1);
//   printf("<DTCM>  %08x .. %08x\n",
//          _sdata, _estack - 1);
//   printf("<RAM>   %08x .. %08x\n",
//          RAM_BASE, RAM_BASE + RAM_SIZE - 1);
//   printf("<FLASH> %08x .. %08x\n",
//          FLASH_BASE, FLASH_BASE + FLASH_SIZE - 1);
// #if ARDUINO_TEENSY41
//   extern uint8_t external_psram_size;
//   if (external_psram_size > 0)
//     printf("<PSRAM> %08x .. %08x\n",
//            _extram_start, _extram_start + (external_psram_size << 20) - 1);
// #endif
//   printf("\n");

//   auto stack = sp - _ebss;
//   printf("avail STACK %8d b %5d kb\n", stack, stack >> 10);

//   auto heap = _heap_end - __brkval;
//   printf("avail HEAP  %8d b %5d kb\n", heap, heap >> 10);

// #if ARDUINO_TEENSY41
//   auto psram = _extram_start + (external_psram_size << 20) - _extram_end;
//   printf("avail PSRAM %8d b %5d kb\n", psram, psram >> 10);
// #endif
// }

// uint32_t *ptrFreeITCM;   // Set to Usable ITCM free RAM
// uint32_t sizeofFreeITCM; // sizeof free RAM in uint32_t units.
// uint32_t SizeLeft_etext;
// // extern uint32_t _stext;
// //  extern uint32_t _etext;
// extern char _stext[], _etext[];

// FLASHMEM void getFreeITCM()
// { // end of CODE ITCM, skip full 32 bits
//   if(DEBUG) Serial.println("\n\n++++++++++++++++++++++");
//   SizeLeft_etext = (32 * 1024) - (((uint32_t)&_etext - (uint32_t)&_stext) % (32 * 1024));
//   sizeofFreeITCM = SizeLeft_etext - 4;
//   sizeofFreeITCM /= sizeof(ptrFreeITCM[0]);
//   ptrFreeITCM = (uint32_t *)((uint32_t)&_stext + (uint32_t)&_etext + 4);
//   printf("Size of Free ITCM in Bytes = %u\n", sizeofFreeITCM * sizeof(ptrFreeITCM[0]));
//   printf("Start of Free ITCM = %u [%X] \n", ptrFreeITCM, ptrFreeITCM);
//   printf("End of Free ITCM = %u [%X] \n", ptrFreeITCM + sizeofFreeITCM, ptrFreeITCM + sizeofFreeITCM);
//   for (uint32_t ii = 0; ii < sizeofFreeITCM; ii++)
//     ptrFreeITCM[ii] = 1;
//   uint32_t jj = 0;
//   for (uint32_t ii = 0; ii < sizeofFreeITCM; ii++)
//     jj += ptrFreeITCM[ii];
//   printf("ITCM DWORD cnt = %u [#bytes=%u] \n", jj, jj * 4);
// }

#endif
