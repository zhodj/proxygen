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
#include <folly/json.h>
#include <iostream>

using namespace proxygen;

namespace EchoService {

EchoHandler::EchoHandler(EchoStats* stats) : stats_(stats) {
}

void EchoHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {

  request_ = std::move(headers);
  std::cout << "request string=" << request_->getQueryString() << std::endl;

  /*
  cpp_redis::client client;
  client.connect("127.0.0.1", 6379);

  std::string code = request_->getQueryParam("code");

  if (request_->getPath() == "/api/get") {

    client.get(
        code, [this](cpp_redis::reply& reply) { gvalue_ = reply.as_string(); });

    client.sync_commit();
  }

  folly::dynamic parsed = folly::parseJson(gvalue_.c_str());
  auto value = parsed[code];

  folly::dynamic sonOfAJ = folly::dynamic::object("data", value);
 
  gvalue_ = folly::toJson(sonOfAJ);
  */
  gvalue_ = "ok!";

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
      .body(folly::IOBuf::copyBuffer(gvalue_))
      .sendWithEOM();
}

void EchoHandler::onUpgrade(UpgradeProtocol /*protocol*/) noexcept {
  // handler doesn't support upgrades
}

void EchoHandler::requestComplete() noexcept {
  delete this;
}

void EchoHandler::onError(ProxygenError /*err*/) noexcept {
  delete this;
}
} // namespace EchoService
