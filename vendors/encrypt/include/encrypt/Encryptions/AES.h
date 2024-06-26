/*
AES.h

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

#include <algorithm>
#include <vector>

#ifndef __AES__
#define __AES__

#include "encrypt/common/cryptomath.h"
#include "encrypt/common/includes.h"
#include "SymAlg.h"

#include "AES_Const.h"

class AES : public SymAlg{
    private:
        uint8_t b, rounds, columns;
        std::vector <std::vector <uint32_t> > keys;

        void shiftrow(std::vector <uint32_t> & data);
        void invshiftrow(std::vector <uint32_t> & data);
        uint8_t GF(uint8_t a, uint8_t b);
        void mixcolumns(std::vector <uint32_t> & data);
        void invmixcolumns(std::vector <uint32_t> & data);
        std::string OUTT(std::vector <uint32_t> & data);

    public:
        AES();
        AES(const std::string & KEY);
        void setkey(const std::string & KEY);
        std::string encrypt(const std::string & DATA);
        std::string decrypt(const std::string & DATA);
        unsigned int blocksize() const;
        unsigned int datasize() const { return 16; };
};

#endif
