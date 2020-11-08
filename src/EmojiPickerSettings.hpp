#pragma once

#include "emojis.hpp"
#include <QSettings>
#include <vector>

class EmojiPickerSettings : public QSettings {
  Q_OBJECT

public:
  explicit EmojiPickerSettings(QObject* parent = nullptr);
  ~EmojiPickerSettings();

  std::vector<Emoji> recentEmojis();
  void setRecentEmojis(const std::vector<Emoji>& recentEmojis);

  std::string localeKey();
  void setLocaleKey(const std::string& localeKey);

  bool skinTonesDisabled();
  void setSkinTonesDisabled(bool skinTonesDisabled);

  bool gendersDisabled();
  void setGendersDisabled(bool gendersDisabled);

  bool openAtMouseLocation();
  void setOpenAtMouseLocation(bool openAtMouseLocation);
};