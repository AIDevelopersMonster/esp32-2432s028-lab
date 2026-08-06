#pragma once

#include <Arduino.h>
#include <FS.h>

namespace cyd::b6 {

struct ExportStatus {
  bool available = false;
  bool lastExportOk = false;
  uint32_t exports = 0;
  uint32_t filesCopied = 0;
  uint64_t bytesCopied = 0;
};

class ServiceExport {
 public:
  bool begin(fs::FS& fs);
  bool createBundle(const char* manifestLine);
  const ExportStatus& status() const { return status_; }

 private:
  fs::FS* fs_ = nullptr;
  ExportStatus status_{};

  bool copyFile(const char* source, const char* target);
  bool writeText(const char* path, const char* text);
};

}  // namespace cyd::b6
