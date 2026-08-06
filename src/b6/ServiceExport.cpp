#include <b6/ServiceExport.h>

namespace cyd::b6 {

bool ServiceExport::begin(fs::FS& fs) {
  fs_ = &fs;
  status_.available = true;
  if (!fs_->exists("/service")) fs_->mkdir("/service");
  return fs_->exists("/service");
}

bool ServiceExport::writeText(const char* path, const char* text) {
  File file = fs_->open(path, FILE_WRITE);
  if (!file) return false;
  const size_t written = file.println(text);
  file.close();
  status_.bytesCopied += written;
  return written > 0;
}

bool ServiceExport::copyFile(const char* source, const char* target) {
  if (!fs_->exists(source)) return true;
  File in = fs_->open(source, FILE_READ);
  File out = fs_->open(target, FILE_WRITE);
  if (!in || !out) {
    if (in) in.close();
    if (out) out.close();
    return false;
  }
  uint8_t buffer[256];
  while (in.available()) {
    const size_t count = in.read(buffer, sizeof(buffer));
    if (count == 0 || out.write(buffer, count) != count) {
      in.close();
      out.close();
      return false;
    }
    status_.bytesCopied += count;
  }
  in.close();
  out.close();
  ++status_.filesCopied;
  return true;
}

bool ServiceExport::createBundle(const char* manifestLine) {
  if (!fs_ || !status_.available) return false;
  fs_->remove("/service/manifest.txt");
  fs_->remove("/service/b5-current.log");
  fs_->remove("/service/b5-previous.log");
  status_.filesCopied = 0;
  status_.bytesCopied = 0;

  bool ok = writeText("/service/manifest.txt", manifestLine);
  ok = copyFile("/diag/b5-current.log", "/service/b5-current.log") && ok;
  ok = copyFile("/diag/b5-previous.log", "/service/b5-previous.log") && ok;
  status_.lastExportOk = ok;
  if (ok) ++status_.exports;
  return ok;
}

}  // namespace cyd::b6
