/*
Encryptions.h

Copyright (c) 2013 - 2017 Jason Lee @ calccrypto at gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef ENCRYPTIONS_H
#define ENCRYPTIONS_H

// common includes
#include "encrypt/common/includes.h"
#include "encrypt/common/cryptomath.h"
#include "encrypt/Encryptions/SymAlg.h"

// Algorithms
#include "encrypt/Encryptions/AES.h"
#include "encrypt/Encryptions/Blowfish.h"
#include "encrypt/Encryptions/Camellia.h"
#include "encrypt/Encryptions/CAST128.h"
#include "encrypt/Encryptions/CAST256.h"
#include "encrypt/Encryptions/DES.h"
#include "encrypt/Encryptions/DESX.h"
#include "encrypt/Encryptions/GOST.h"
#include "encrypt/Encryptions/IDEA.h"
#include "encrypt/Encryptions/MISTY1.h"
#include "encrypt/Encryptions/RC2.h"
#include "encrypt/Encryptions/RC4.h"
#include "encrypt/Encryptions/RC5.h"
#include "encrypt/Encryptions/RC6.h"
#include "encrypt/Encryptions/SAFERK64.h"
#include "encrypt/Encryptions/SEED.h"
#include "encrypt/Encryptions/Skipjack.h"
#include "encrypt/Encryptions/TDES.h"
#include "encrypt/Encryptions/TEA.h"
#include "encrypt/Encryptions/Twofish.h"
#include "encrypt/Encryptions/XTEA.h"

// Modes of Operation
#include "encrypt/modes/ECB.h"
#include "encrypt/modes/CBC.h"
#include "encrypt/modes/CFB.h"
#include "encrypt/modes/CTR.h"
#include "encrypt/modes/OFB.h"
#include "encrypt/modes/PCPB.h"

#endif