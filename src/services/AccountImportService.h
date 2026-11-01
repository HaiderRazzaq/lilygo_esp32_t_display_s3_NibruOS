#pragma once
#include <stdint.h>
#include <stddef.h>

enum class ImportMode : uint8_t { Usb, Wifi };
enum class ImportState : uint8_t { Idle, Waiting, Review, Expired };
struct ImportedAccount {
  char name[24]{};
  char secret[53]{};
  uint8_t digits = 6, period = 30;
};
struct AccountImportTransport {
  bool (*open)(ImportMode);
  void (*close)();
  int (*read)(ImportMode, uint8_t*, size_t);
  AccountImportTransport(bool (*o)(ImportMode), void (*c)(), int (*r)(ImportMode, uint8_t*, size_t))
      : open(o), close(c), read(r) {}
};
class AccountImportService {
public:
  static constexpr uint16_t Port = 8766;
  static constexpr uint32_t TimeoutMs = 120000;
  explicit AccountImportService(const AccountImportTransport* transport = nullptr) : transport_(transport) {}
  bool start(ImportMode mode, uint32_t now);
  void stop();
  void tick(uint32_t now);
  ImportState state() const { return state_; }
  const char* pairingCode() const { return pairing_; }
  const ImportedAccount* candidate() const { return state_ == ImportState::Review ? &candidate_ : nullptr; }
  uint32_t remainingSeconds(uint32_t now) const;
private:
  const AccountImportTransport* transport_;
  ImportMode mode_ = ImportMode::Usb;
  ImportState state_ = ImportState::Idle;
  uint32_t started_ = 0;
  char pairing_[17]{};
  uint8_t key_[32]{};
  char frame_[256]{};
  uint16_t used_ = 0;
  bool overflow_ = false;
  uint8_t rejected_ = 0;
  ImportedAccount candidate_{};
  bool decodeFrame();
};
