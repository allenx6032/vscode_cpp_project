﻿#pragma once

#include <Brynet_SSLHelper.hpp>
#include <Brynet_TcpConnection.hpp>

namespace brynet { namespace net { namespace detail {

class ConnectionOption final
{
public:
    std::vector<TcpConnection::EnterCallback> enterCallback;
    std::function<void()> enterFailedCallback;
    SSLHelper::Ptr sslHelper;
    bool useSSL = false;
    bool forceSameThreadLoop = false;
    size_t maxRecvBufferSize = 128;
};

}}}// namespace brynet::net::detail
