#pragma once

#include <Arduino.h>
#include <FS.h>

namespace cyd::b5 {

struct ArchiveStatus {
  bool mounted = false;
  bool writable = false;
  uint32_t records = 0;
  uint32_t rotations = 0;
  uint64_t bytes = 0;
};

class DiagnosticArchive {
 public:
  bool begin(fs::FS& fs);
  bool append(const char* line);
  const ArchiveStatus& status() const { return status_; }

 private:
  fs::FS* fs_ = nullptr;
  ArchiveStatus status_{};
  static constexpr const char* ACTIVE = "/diag/b5-current.log";
  static constexpr const char* PREVIOUS = "/diag/b5-previous.log";
  static constexpr uint64_t MAX_BYTES = 64U * 1024U;

  bool rotateIfNeeded(size_t incomingBytes);
};

}  // namespace cyd::b5
