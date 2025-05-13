#include <sstream>
#include "Approval_EmptyFileCreatorDisposer.h"

namespace ApprovalTests
{

    EmptyFileCreatorDisposer::EmptyFileCreatorDisposer(EmptyFileCreator creator)
    {
        previous_result = std::move(EmptyFileCreatorFactory::currentCreator);
        EmptyFileCreatorFactory::currentCreator = std::move(creator);
    }

    EmptyFileCreatorDisposer::~EmptyFileCreatorDisposer()
    {
        EmptyFileCreatorFactory::currentCreator = std::move(previous_result);
    }
}
