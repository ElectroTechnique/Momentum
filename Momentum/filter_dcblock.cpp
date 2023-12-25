/* Audio Library for Teensy, DC Block
 * Copyright (c) 2023, Matt Kuebrich
 *
 * This was adapted from "Tim's Quicker DC Blocker" by Tim Wescott.
 *
 * I can't find where the code first originated (maybe on the defunct comp.dsp Google group),
 * but it's been reposted here by Robert Bristow-Johnson, about halfway down the page.
 * https://www.dsprelated.com/showthread/comp.dsp/172787-1.php
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "filter_dcblock.h"

void AudioFilterDCBlock::update(void)
{

    audio_block_t *block = receiveReadOnly();
    if (!block)
        return;

    A = (long)(32768.0f * (1.0 - pole));

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
        int16_t input = block->data[i];
        curr_y = long(input) - (w >> 15);
        // error = w - (curr_y<<15);
        w += (A * curr_y);
        block->data[i] = saturate16(curr_y);
    }

    transmit(block);
    release(block);
}
