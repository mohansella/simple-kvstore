#pragma once

namespace mohansella::kvstore
{

    //zero or greater is fine.
    enum ErrorCode
    {
        
        CODE_FAILURE_LARGE_KEY = -1,
        CODE_FAILURE_LARGE_VALUE = -2,
        CODE_FAILURE_KEY_ALREADY_EXISTS = -3,
        CODE_FAILURE_KEY_NOT_EXISTS = -4,
        CODE_FAILURE_SIZE_LIMIT_REACHED = -5,

        CODE_ZERO = 0,
        

    };

}