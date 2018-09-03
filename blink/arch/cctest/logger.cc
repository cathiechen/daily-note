// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/cctest/logger.h"

#include "base/logging.h"

#include <string>

namespace cctest {

Logger::Logger() {
  LOG(INFO) << "cctest::Logger::Logger";
}

Logger::~Logger() {
  LOG(INFO) << "cctest::Logger::~Logger";
}

void Logger::Log(const std::string& message) {
  LOG(INFO) << "cctest::Logger::Log " << message;
}

}  // namespace file