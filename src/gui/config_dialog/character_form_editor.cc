// Copyright 2010-2021, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "gui/config_dialog/character_form_editor.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGui>
#include <memory>
#include <string>

#include "absl/strings/string_view.h"
#include "config/config_handler.h"

namespace mozc {
namespace gui {

namespace {
QString FormToString(config::Config::CharacterForm form) {
  switch (form) {
    case config::Config::FULL_WIDTH:
      return QObject::tr("Fullwidth");
      break;
    case config::Config::HALF_WIDTH:
      return QObject::tr("Halfwidth");
      break;
    case config::Config::LAST_FORM:
      return QObject::tr("Remember");
      break;
    default:
      return QObject::tr("Unknown");
      break;
  }
}

config::Config::CharacterForm StringToForm(const QString &str) {
  if (str == QObject::tr("Fullwidth")) {
    return config::Config::FULL_WIDTH;
  } else if (str == QObject::tr("Halfwidth")) {
    return config::Config::HALF_WIDTH;
  } else if (str == QObject::tr("Remember")) {
    return config::Config::LAST_FORM;
  }
  return config::Config::FULL_WIDTH;  // failsafe
}

QString GroupToString(absl::string_view str) {
  if (str == "ア") {
    return QObject::tr("Katakana");
  } else if (str == "0") {
    return QObject::tr("Numbers");
  } else if (str == "A") {
    return QObject::tr("Alphabets");
  }
  return QString::fromUtf8(str.data(), static_cast<int>(str.size()));
}

std::string StringToGroup(const QString &str) {
  if (str == QObject::tr("Katakana")) {
    // return "ア";
    return "ア";
  } else if (str == QObject::tr("Numbers")) {
    return "0";
  } else if (str == QObject::tr("Alphabets")) {
    return "A";
  }
  return str.toStdString();
}

QComboBox *CreateFormComboBox(QWidget *parent, QStringList strings) {
  auto *combo_box = new QComboBox(parent);
  combo_box->addItems(strings);
  combo_box->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  return combo_box;
}

QHBoxLayout *CreateFormLayout(QWidget *parent, QComboBox *combo_box) {
  auto *layout = new QHBoxLayout();
  layout->setContentsMargins(8, 8, 8, 8);
  layout->addWidget(combo_box);
  return layout;
}

QWidget *CreateGroupCell(QWidget *parent, const QString &text) {
  auto *container = new QWidget(parent);
  auto *layout = new QVBoxLayout(container);
  layout->setContentsMargins(8, 8, 8, 8);

  auto *label = new QLabel(text, container);
  label->setStyleSheet("color: palette(button-text);");
  layout->addWidget(label);

  return container;
}
}  // namespace

CharacterFormEditor::CharacterFormEditor(QWidget *parent)
    : QTableWidget(parent) {
  setColumnCount(3);
  setSelectionMode(QAbstractItemView::NoSelection);
  setFocusPolicy(Qt::NoFocus);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  verticalHeader()->hide();
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  setShowGrid(false);
  setStyleSheet(
      "QTableWidget { background-color: palette(window); }"
      "QHeaderView::section { padding-top: 2px; padding-bottom: 2px; }"
      "QTableWidget::item:selected { background: transparent; }");
}

void CharacterFormEditor::Load(const config::Config &config) {
  clearContents();

  std::unique_ptr<config::Config> default_config;
  const config::Config *target_config = &config;

  // make sure that table isn't empty.
  if (config.character_form_rules_size() == 0) {
    default_config = std::make_unique<config::Config>();
    config::ConfigHandler::GetDefaultConfig(default_config.get());
    target_config = default_config.get();
  }

  const QStringList headers = {tr("Group"), tr("Composition"),
                               tr("Conversion")};
  setHorizontalHeaderLabels(headers);

  auto strings = QStringList() << QObject::tr("Fullwidth") << QObject::tr("Halfwidth") << QObject::tr("Remember");
  setRowCount(target_config->character_form_rules_size());
  for (int row = 0; row < target_config->character_form_rules_size(); ++row) {
    const config::Config::CharacterFormRule &rule =
        target_config->character_form_rules(row);
    auto group = GroupToString(rule.group());
    auto *group_container = CreateGroupCell(this, group);

    auto *preedit_container = new QWidget(this);
    auto *preedit_combo = CreateFormComboBox(preedit_container, strings);
    preedit_combo->setObjectName("preedit_combo");
    preedit_container->setLayout(
        CreateFormLayout(preedit_container, preedit_combo));

    auto *conversion_container = new QWidget(this);
    auto *conversion_combo = CreateFormComboBox(conversion_container, strings);
    conversion_combo->setObjectName("conversion_combo");
    conversion_container->setLayout(
        CreateFormLayout(conversion_container, conversion_combo));
    QObject::connect(preedit_combo, SIGNAL(activated(int)), this,
                     SIGNAL(ItemModified()));
    QObject::connect(conversion_combo, SIGNAL(activated(int)), this,
                     SIGNAL(ItemModified()));

    preedit_combo->setCurrentText(
        FormToString(rule.preedit_character_form()));
    conversion_combo->setCurrentText(
        FormToString(rule.conversion_character_form()));

    setCellWidget(row, 0, group_container);
    setCellWidget(row, 1, preedit_container);
    setCellWidget(row, 2, conversion_container);
    // Preedit Katakan is always FULLWIDTH
    // This item should not be editable
    if (group == QObject::tr("Katakana")) {
      preedit_combo->setEnabled(false);
    }
  }
  resizeRowsToContents();
}

void CharacterFormEditor::Save(config::Config *config) {
  if (rowCount() == 0) {
    return;
  }

  config->clear_character_form_rules();
  for (int row = 0; row < rowCount(); ++row) {
    if (item(row, 0)->text().isEmpty()) {
      continue;
    }

    const std::string group = StringToGroup(item(row, 0)->text());
    config::Config::CharacterForm preedit_form =
        StringToForm(cellWidget(row, 1)->findChild<QComboBox *>("preedit_combo")->currentText());
    config::Config::CharacterForm conversion_form =
        StringToForm(cellWidget(row, 2)->findChild<QComboBox *>("conversion_combo")->currentText());
    config::Config::CharacterFormRule *rule =
        config->add_character_form_rules();
    rule->set_group(group);
    rule->set_preedit_character_form(preedit_form);
    rule->set_conversion_character_form(conversion_form);
  }
}
}  // namespace gui
}  // namespace mozc
