/*
   Copyright (C) 2000-2007 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING. If not, write to the
   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
   MA  02110-1301  USA.
*/


/* dh.cpp implements Diffie-Hellman support
*/

#include "yassl_runtime.hpp"
#include "yassl_dh.hpp"
#include "yassl_asn.hpp"
#include <math.h>

namespace TaoCrypt {


namespace {  // locals

unsigned int DiscreteLogWorkFactor(unsigned int n)
{
    // assuming discrete log takes about the same time as factoring
    if (n<5)
        return 0;
    else
        return (unsigned int)(2.4 * pow((double)n, 1.0/3.0) *
                pow(log(double(n)), 2.0/3.0) - 5);
}

} // namespace locals


// Generate a DH Key Pair
void DH::GenerateKeyPair(RandomNumberGenerator& rng, byte* priv, byte* pub)
{
    GeneratePrivate(rng, priv);
    GeneratePublic(priv, pub);
}


// Generate private value
void DH::GeneratePrivate(RandomNumberGenerator& rng, byte* priv)
{
    Integer x(rng, Integer::One(), min(p_ - 1,
        Integer::Power2(2*DiscreteLogWorkFactor(p_.BitCount())) ) );
    x.Encode(priv, p_.ByteCount());
}


// Generate public value
void DH::GeneratePublic(const byte* priv, byte* pub)
{
    const word32 bc(p_.ByteCount());
    Integer x(priv, bc);
    Integer y(a_exp_b_mod_c(g_, x, p_));
    y.Encode(pub, bc);
}


// Generate Agreement
void DH::Agree(byte* agree, const byte* priv, const byte* otherPub, word32
               otherSz)
{
    const word32 bc(p_.ByteCount());
    Integer x(priv, bc);
    Integer y;
    if (otherSz)
        y.Decode(otherPub, otherSz);
    else
        y.Decode(otherPub, bc);

    Integer z(a_exp_b_mod_c(y, x, p_));
    z.Encode(agree, bc);
}


DH::DH(Source& source)
{
    Initialize(source);
}


void DH::Initialize(Source& source)
{
    DH_Decoder decoder(source);
    decoder.Decode(*this);
}


} // namespace
