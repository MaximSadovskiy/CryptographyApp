/*
SymAlg.h

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

#ifndef __SYMALG__
#define __SYMALG__

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

class SymAlg{
    protected:
        bool keyset;

    public:
        typedef std::shared_ptr<SymAlg> Ptr;

        SymAlg();
        virtual ~SymAlg();
        virtual std::string encrypt(const std::string & DATA) = 0;
        virtual std::string decrypt(const std::string & DATA) = 0;
        virtual void setkey(const std::string& KEY) = 0;
        virtual unsigned int datasize() const = 0;
        virtual unsigned int blocksize() const = 0; // blocksize in bits
};

#endif
