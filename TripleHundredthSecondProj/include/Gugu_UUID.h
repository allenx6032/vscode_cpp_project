#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Types.h"

#include <string>

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {

class UUID
{
public:

    static UUID Generate();
    static UUID FromString(const std::string& value);

    bool IsZero() const;
    std::string ToString() const;

    bool operator == (const UUID& right) const;
    bool operator != (const UUID& right) const;
    bool operator < (const UUID& right) const; // Used by std sorts

private:

    uint64 m_data1 = 0;
    uint64 m_data2 = 0;
};

UUID GenerateUUID();
std::string GenerateUUIDAsString();

std::ostream& operator << (std::ostream& stream, const UUID& uuid); // Used by toString

}   // namespace gugu
