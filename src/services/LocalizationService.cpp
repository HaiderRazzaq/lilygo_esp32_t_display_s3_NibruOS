#include "services/LocalizationService.h"

LocalizationService& LocalizationService::instance() {
  static LocalizationService s_instance;
  return s_instance;
}

const char* LocalizationService::get(TextId id) const {
  const bool ar = (lang_ == Language::Arabic);

  switch (id) {
    // App Titles
    case TextId::AppLauncher:    return ar ? "الرئيسية" : "Launcher";
    case TextId::AppSettings:     return ar ? "الإعدادات" : "Settings";
    case TextId::AppWiFi:         return ar ? "الواي فاي" : "Wi-Fi";
    case TextId::AppTotp:         return ar ? "المصادقة" : "2FA Vault";
    case TextId::AppVault:        return ar ? "الخزنة" : "Passwords";
    case TextId::AppSystemInfo:   return ar ? "النظام" : "System Info";
    case TextId::AppTouchTest:    return ar ? "اللمس" : "Touch Test";

    // App Subtitles
    case TextId::SubLauncher:    return ar ? "نظام نبرو" : "NibruOS";
    case TextId::SubSettings:     return ar ? "إعدادات النظام" : "System";
    case TextId::SubWiFi:         return ar ? "الشبكات" : "Networks";
    case TextId::SubTotp:         return ar ? "رموز التحقق" : "Authenticator";
    case TextId::SubVault:        return ar ? "خزنة كلمات المرور" : "Password Vault";
    case TextId::SubSystemInfo:   return ar ? "المعلومات" : "Telemetry";
    case TextId::SubTouchTest:    return ar ? "فحص الشاشة" : "Diagnostics";

    // Settings Menu Items
    case TextId::MenuDisplay:     return ar ? "الشاشة والسطوع" : "Display & timeout";
    case TextId::MenuTheme:       return ar ? "لون المظهر" : "Accent color";
    case TextId::MenuMemory:      return ar ? "النظام والذاكرة" : "System & memory";
    case TextId::MenuWiFiDiag:    return ar ? "فحص الواي فاي" : "Wi-Fi diagnostics";
    case TextId::MenuDevice:      return ar ? "إعادة التشغيل والضبط" : "Reboot / reset settings";
    case TextId::MenuSecurity:    return ar ? "الأمان ورمز PIN" : "Security & PIN";
    case TextId::MenuLanguage:    return ar ? "لغة الجهاز (Language)" : "Language / اللغة";
    case TextId::MenuTouchTest:   return ar ? "اختبار اللمس" : "Touch test";
    case TextId::MenuWebPortal:   return ar ? "بوابة الويب والتحكم" : "Web Portal (AP Mode)";

    // Display Settings
    case TextId::DisplayTitle:       return ar ? "الشاشة والعرض" : "Display & Screen";
    case TextId::DisplayBrightness:  return ar ? "سطوع الشاشة" : "Screen Brightness";
    case TextId::DisplayTimeout:     return ar ? "مهلة القفل" : "Screen Timeout";
    case TextId::DisplayOrientation: return ar ? "اتجاه الشاشة" : "Orientation";
    case TextId::RotNormal:          return ar ? "عادي (0 درجة)" : "Normal (0 deg)";
    case TextId::RotInverted:        return ar ? "معكوس (180 درجة)" : "Inverted (180 deg)";
    case TextId::SecNever:           return ar ? "أبداً" : "Never";
    case TextId::SecSeconds:         return ar ? "ثانية" : "sec";

    // Theme Settings
    case TextId::ThemeTitle:      return ar ? "مظهر النظام" : "Theme Accent";
    case TextId::ThemeCyan:       return ar ? "سماوي سايبر" : "Cyber Cyan";
    case TextId::ThemeGreen:      return ar ? "أخضر ماتريكس" : "Matrix Green";
    case TextId::ThemeAmber:      return ar ? "ذهبي كهرماني" : "Amber Gold";
    case TextId::ThemeMagenta:    return ar ? "وردي نيون" : "Neon Magenta";

    // Security & PIN
    case TextId::SecurityTitle:   return ar ? "الأمان ورمز PIN" : "Security & PIN";
    case TextId::SetPinTitle:     return ar ? "تعيين رمز PIN" : "Set Security PIN";
    case TextId::PinEnabled:      return ar ? "مفعّل" : "ENABLED";
    case TextId::PinDisabled:     return ar ? "معطّل" : "DISABLED";
    case TextId::ToggleLock:      return ar ? "تبديل حالة القفل" : "Toggle PIN Lock";
    case TextId::ConfigurePin:    return ar ? "[ ضبط رمز PIN ]" : "[ CONFIGURE / CHANGE PIN ]";
    case TextId::LockDeviceNow:   return ar ? "[ قفل الجهاز الآن ]" : "[ LOCK DEVICE NOW ]";
    case TextId::DeviceLocked:    return ar ? "الجهاز مقفل" : "NibruOS Locked";
    case TextId::EnterPinPrompt:  return ar ? "أدخل رمز PIN للأمان" : "Enter Security PIN";
    case TextId::IncorrectPin:    return ar ? "رمز PIN غير صحيح!" : "Incorrect PIN! Try again.";
    case TextId::AntiBruteForce:  return ar ? "وضع الحظر النشط" : "Anti-Brute-Force Lockout Active";
    case TextId::LockedOutWait:   return ar ? "تم القفل! انتظر" : "Locked out! Wait";

    // Common UI Actions
    case TextId::ActionBack:      return ar ? "رجوع" : "Back";
    case TextId::ActionSave:      return ar ? "حفظ" : "Save";
    case TextId::ActionDelete:    return ar ? "حذف" : "DEL";
    case TextId::ActionCancel:    return ar ? "إلغاء" : "Cancel";
    case TextId::ActionConfirm:   return ar ? "تأكيد" : "Confirm";
    case TextId::ActionReboot:    return ar ? "إعادة التشغيل" : "Reboot";
    case TextId::ActionReset:     return ar ? "إعادة الضبط" : "Reset";
    case TextId::StatusEmpty:     return ar ? "(فارغ)" : "(empty)";
    case TextId::StatusConnected: return ar ? "متصل" : "Connected";
    case TextId::StatusDisconnected: return ar ? "غير متصل" : "Disconnected";
    case TextId::StatusScanning:  return ar ? "جاري البحث..." : "Scanning...";

    // Password Vault Actions
    case TextId::VaultTypeUser:   return ar ? "إرسال المستخدم" : "TYPE USER";
    case TextId::VaultTypePass:   return ar ? "إرسال الرمز" : "TYPE PASS";
    case TextId::VaultConfirmSend:return ar ? "تأكيد الإرسال؟" : "CONFIRM SEND";
    case TextId::VaultShowPass:   return "[SHOW]";
    case TextId::VaultHidePass:   return "[HIDE]";

    // Language Menu
    case TextId::LanguageTitle:   return ar ? "لغة النظام" : "System Language";
    case TextId::LangEnglish:     return "English (الإنجليزية)";
    case TextId::LangArabic:      return "العربية (Arabic)";

    default: return "";
  }
}
