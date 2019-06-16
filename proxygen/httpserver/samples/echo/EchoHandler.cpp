/*
 *  Copyright (c) 2015-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "EchoHandler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "EchoStats.h"

#include <cpp_redis/cpp_redis>

#include <iostream>

using namespace proxygen;

namespace EchoService {

EchoHandler::EchoHandler(EchoStats* stats): stats_(stats) {
}

void EchoHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {

  cpp_redis::client client;
  client.connect();

  request_ = std::move(headers);

  if(request_->getPath() == "/api/get")
  {

    client.get("", [](cpp_redis::reply & reply)) {
      std::cout << reply << std::endl;
    }
  }

  stats_->recordRequest();
}

void EchoHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
  if (body_) {
    body_->prependChain(std::move(body));
  } else {
    body_ = std::move(body);
  }
}

void EchoHandler::onEOM() noexcept {
  ResponseBuilder(downstream_)
    .status(200, "OK")
    .header("Request-Number",
            folly::to<std::string>(stats_->getRequestCount()))
    .body(std::move(body_))
    .sendWithEOM();
}

void EchoHandler::onUpgrade(UpgradeProtocol /*protocol*/) noexcept {
  // handler doesn't support upgrades
}

void EchoHandler::requestComplete() noexcept {
  delete this;
}

void EchoHandler::onError(ProxygenError /*err*/) noexcept { delete this; }
}
