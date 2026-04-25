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

#include "gui/config_dialog/aligned_wrapped_check_box_row.h"

#include <algorithm>

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionButton>

namespace mozc {
namespace gui {

AlignedWrappedCheckBoxRow::AlignedWrappedCheckBoxRow(QWidget *parent)
    : QWidget(parent),
      checkbox_column_layout_(new QVBoxLayout),
      checkbox_(new QCheckBox(this)),
      label_(new QLabel(this)) {
  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(3);

  checkbox_column_layout_->setContentsMargins(0, 0, 0, 0);
  checkbox_column_layout_->setSpacing(0);
  checkbox_column_layout_->addWidget(checkbox_);
  checkbox_column_layout_->addStretch();

  auto *checkbox_column_widget = new QWidget(this);
  checkbox_column_widget->setLayout(checkbox_column_layout_);

  checkbox_->setText(QString());
  checkbox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFocusProxy(checkbox_);

  label_->setWordWrap(true);
  label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  label_->setBuddy(checkbox_);
  label_->installEventFilter(this);
  auto *label_layout = new QVBoxLayout;
  label_layout->setContentsMargins(0, 1, 0, 0);
  label_layout->setSpacing(0);
  label_layout->addWidget(label_);

  auto *label_widget = new QWidget(this);
  label_widget->setLayout(label_layout);

  layout->addWidget(checkbox_column_widget, 0, Qt::AlignTop);
  layout->addWidget(label_widget, 1);

  connect(checkbox_, SIGNAL(clicked()), this, SIGNAL(clicked()));
  connect(checkbox_, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

  UpdateCheckboxOffset();
}

void AlignedWrappedCheckBoxRow::setText(const QString &text) {
  label_->setText(text);
  UpdateCheckboxOffset();
}

void AlignedWrappedCheckBoxRow::setChecked(bool checked) {
  checkbox_->setChecked(checked);
}

bool AlignedWrappedCheckBoxRow::isChecked() const {
  return checkbox_->isChecked();
}

void AlignedWrappedCheckBoxRow::toggle() {
  checkbox_->toggle();
}

bool AlignedWrappedCheckBoxRow::eventFilter(QObject *watched, QEvent *event) {
  if (watched == label_ && event->type() == QEvent::MouseButtonRelease &&
      isEnabled()) {
    checkbox_->toggle();
    return true;
  }
  return QWidget::eventFilter(watched, event);
}

void AlignedWrappedCheckBoxRow::changeEvent(QEvent *event) {
  QWidget::changeEvent(event);
  switch (event->type()) {
    case QEvent::EnabledChange:
      label_->setEnabled(isEnabled());
      break;
    case QEvent::FontChange:
    case QEvent::StyleChange:
      UpdateCheckboxOffset();
      break;
    default:
      break;
  }
}

void AlignedWrappedCheckBoxRow::UpdateCheckboxOffset() {
  const int indicator_height =
      checkbox_->style()->pixelMetric(QStyle::PM_IndicatorHeight, nullptr,
                                      checkbox_);
  const int first_line_height = label_->fontMetrics().height();
  const int top_offset = std::max(0, (first_line_height - indicator_height) / 2);
  checkbox_column_layout_->setContentsMargins(0, top_offset, 0, 0);
}

}  // namespace gui
}  // namespace mozc
