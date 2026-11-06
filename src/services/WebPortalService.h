#pragma once
#include <stdint.h>
#include <stddef.h>

struct AppContext;

enum class WebPortalState : uint8_t {
  Idle,
  Starting,
  Active,
  Stopping,
  Error
};

/**
 * @brief High-security local Web Portal and Device Control Center for NibruOS.
 * Broadcasts an ephemeral SoftAP with random WPA2 credentials and serves a responsive
 * web application for easily importing Vault credentials and TOTP accounts from any
 * smartphone or PC browser without installing external software.
 */
class WebPortalService {
public:
  static constexpr uint16_t kHttpPort = 80;
  static constexpr uint32_t kDefaultTimeoutSec = 180; // 3 minutes active session

  WebPortalService() = default;

  bool start(uint32_t nowMs, AppContext& ctx);
  void stop(AppContext& ctx);
  void tick(uint32_t nowMs, AppContext& ctx);

  bool isActive() const { return state_ == WebPortalState::Active; }
  WebPortalState state() const { return state_; }
  bool isBleSuspended() const { return bleSuspended_; }

  const char* ssid() const { return ssid_; }
  const char* password() const { return password_; }
  const char* webPin() const { return webPin_; }
  const char* serverIp() const { return "192.168.4.1"; }
  uint32_t remainingSeconds(uint32_t nowMs) const;

  // Cross-platform request simulator for host tests and desktop simulator
  int simulateRequest(AppContext& ctx, const char* method, const char* path, const char* body, char* outResponse, size_t maxLen);

private:
  WebPortalState state_ = WebPortalState::Idle;
  uint32_t startedMs_ = 0;
  uint32_t timeoutSec_ = kDefaultTimeoutSec;
  bool bleSuspended_ = false;

  char ssid_[24]{};
  char password_[16]{};
  char webPin_[8]{};

  void generateCredentials();
  int handleStatus(AppContext& ctx, char* outResponse, size_t maxLen);
  int handleTotpList(AppContext& ctx, char* outResponse, size_t maxLen);
  int handleAddTotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
  int handleRemoveTotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
  int handleNextHotp(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
  int handleVaultList(AppContext& ctx, char* outResponse, size_t maxLen);
  int handleAddVault(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
  int handleRemoveVault(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
  int handleBackup(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
  int handleRestore(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
  int handleDeviceDisplay(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
  int handleDeviceLock(AppContext& ctx, const char* body, char* outResponse, size_t maxLen);
};
