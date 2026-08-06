#include <b5/DiagnosticArchive.h>

namespace cyd::b5 {

bool DiagnosticArchive::begin(fs::FS& fs) {
  fs_ = &fs;
  status_.mounted = true;
  if (!fs_->exists("/diag")) fs_->mkdir("/diag");
  File file = fs_->open(ACTIVE, FILE_APPEND);
  status_.writable = static_cast<bool>(file);
  if (file) {
    status_.bytes = file.size();
    file.close();
  }
  return status_.writable;
}

bool DiagnosticArchive::rotateIfNeeded(size_t incomingBytes) {
  if (status_.bytes + incomingBytes <= MAX_BYTES) return true;
  if (fs_->exists(PREVIOUS)) fs_->remove(PREVIOUS);
  if (fs_->exists(ACTIVE) && !fs_->rename(ACTIVE, PREVIOUS)) return false;
  status_.bytes = 0;
  ++status_.rotations;
  return true;
}

bool DiagnosticArchive::append(const char* line) {
  if (!fs_ || !status_.writable || !line) return false;
  const size_t length = strlen(line) + 1;
  if (!rotateIfNeeded(length)) return false;
  File file = fs_->open(ACTIVE, FILE_APPEND);
  if (!file) return false;
  const size_t written = file.println(line);
  file.flush();
  file.close();
  if (written == 0) return false;
  status_.bytes += written;
  ++status_.records;
  return true;
}

}  // namespace cyd::b5
