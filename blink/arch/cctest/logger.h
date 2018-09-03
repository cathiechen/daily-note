// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_CCTEST_LOGGER_H_
#define SERVICES_CCTEST_LOGGER_H_

#include "services/cctest/public/logger.mojom.h"

namespace cctest {

// A service which serves directories to callers.
class Logger : public mojom::Logger {
 public:
  Logger();
  ~Logger() override;

  // Overridden from mojom::Logger:
  void Log(const std::string& message) override;
};

}  // namespace cctest

#endif  // SERVICES_CCTEST_LOGGER_H_