#pragma once
#include <stdint.h>

/**
 * @brief Internationalization & Localization Service for NibruOS.
 * Supports English and Arabic system languages with centralized string dictionaries.
 */
enum class Language : uint8_t {
  English = 0,
  Arabic = 1
};

enum class TextId : uint16_t {
  // App Titles
  AppLauncher,
  AppSettings,
  AppWiFi,
  AppTotp,
  AppVault,
  AppSystemInfo,
  AppTouchTest,

  // App Subtitles
  SubLauncher,
  SubSettings,
  SubWiFi,
  SubTotp,
  SubVault,
  SubSystemInfo,
  SubTouchTest,

  // Settings Menu Items
  MenuDisplay,
  MenuTheme,
  MenuMemory,
  MenuWiFiDiag,
  MenuDevice,
  MenuSecurity,
  MenuLanguage,
  MenuTouchTest,
  MenuWebPortal,

  // Display Settings
  DisplayTitle,
  DisplayBrightness,
  DisplayTimeout,
  DisplayOrientation,
  RotNormal,
  RotInverted,
  SecNever,
  SecSeconds,

  // Theme Settings
  ThemeTitle,
  ThemeCyan,
  ThemeGreen,
  ThemeAmber,
  ThemeMagenta,

  // Security & PIN
  SecurityTitle,
  SetPinTitle,
  PinEnabled,
  PinDisabled,
  ToggleLock,
  ConfigurePin,
  LockDeviceNow,
  DeviceLocked,
  EnterPinPrompt,
  IncorrectPin,
  AntiBruteForce,
  LockedOutWait,

  // Common UI Actions
  ActionBack,
  ActionSave,
  ActionDelete,
  ActionCancel,
  ActionConfirm,
  ActionReboot,
  ActionReset,
  StatusEmpty,
  StatusConnected,
  StatusDisconnected,
  StatusScanning,

  // Password Vault UI
  VaultTypeUser,
  VaultTypePass,
  VaultConfirmSend,
  VaultShowPass,
  VaultHidePass,

  // Language Menu
  LanguageTitle,
  LangEnglish,
  LangArabic,

  Count
};

class LocalizationService {
public:
  static LocalizationService& instance();

  void setLanguage(Language lang) { lang_ = lang; }
  Language language() const { return lang_; }
  bool isArabic() const { return lang_ == Language::Arabic; }

  const char* get(TextId id) const;

private:
  Language lang_ = Language::English;
};
