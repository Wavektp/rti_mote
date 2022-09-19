/**
 * rti_mote.ino
 *
 * Developed for a feasibility study of prototype of RTI (Radio Tomographical
 * Imamging) systems. This file is part of NKRAFA RTI MOTE Project.
 * https://github.com/Singhanata/rti_mote.git
 *
 * RTI Mote equipped with IR communication on ESP32 boards
 * version 0.1
 * 2022-09-09
 *
 * Copyright (c) 2022 Krongboon Singhanat (krongboon_s@rtaf.mi.th)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "rti.h"

RTI rti;

void setup() {
  rti.begin();
}

void loop() {
  rti.routine();
}
