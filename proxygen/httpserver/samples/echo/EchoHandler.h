/*
 *  Copyright (c) 2015-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include <folly/Memory.h>
#include <proxygen/httpserver/RequestHandler.h>

namespace proxygen {
class ResponseHandler;
}

namespace EchoService {

class EchoStats;

class EchoHandler : public proxygen::RequestHandler {
 public:
  explicit EchoHandler(EchoStats* stats);

  void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers)
      noexcept override;

  void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

  void onEOM() noexcept override;

  void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override;

  void requestComplete() noexcept override;

  void onError(proxygen::ProxygenError err) noexcept override;

 private:
  EchoStats* const stats_{nullptr};

  std::unique_ptr<folly::IOBuf> body_;

  std::unique_ptr<proxygen::HTTPMessage> request_;

  std::string gvalue_;
};

}
