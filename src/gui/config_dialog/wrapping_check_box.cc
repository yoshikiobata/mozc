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

#include "gui/config_dialog/wrapping_check_box.h"

#include <algorithm>

#include <QEvent>
#include <QFontMetrics>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionButton>

namespace mozc {
namespace gui {

WrappingCheckBox::WrappingCheckBox(QWidget *parent)
    : QCheckBox(parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void WrappingCheckBox::setText(const QString &text) {
  source_text_ = text;
  QCheckBox::setText(text);
  updateGeometry();
  update();
}

QSize WrappingCheckBox::sizeHint() const {
  const QSize base_size = QCheckBox::sizeHint();
  const int width = std::max(base_size.width(), minimumSizeHint().width());
  return QSize(width, std::max(base_size.height(), TextHeightForWidth(width)));
}

QSize WrappingCheckBox::minimumSizeHint() const {
  const QSize base_size = QCheckBox::minimumSizeHint();
  QStyleOptionButton option;
  initStyleOption(&option);
  const int indicator_width =
      style()->pixelMetric(QStyle::PM_IndicatorWidth, &option, this);
  const int label_spacing =
      style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &option, this);
  const int text_width =
      QFontMetrics(font()).horizontalAdvance(source_text_.simplified());
  const int minimum_width = indicator_width + label_spacing + text_width;
  const int width = std::max(base_size.width(), minimum_width);
  return QSize(width, std::max(base_size.height(), TextHeightForWidth(width)));
}

bool WrappingCheckBox::hasHeightForWidth() const { return true; }

int WrappingCheckBox::heightForWidth(int width) const {
  return std::max(QCheckBox::minimumSizeHint().height(), TextHeightForWidth(width));
}

void WrappingCheckBox::changeEvent(QEvent *event) {
  QCheckBox::changeEvent(event);
  switch (event->type()) {
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::PaletteChange:
    case QEvent::EnabledChange:
    case QEvent::LanguageChange:
      updateGeometry();
      update();
      break;
    default:
      break;
  }
}

void WrappingCheckBox::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QStyleOptionButton option;
  initStyleOption(&option);
  option.text.clear();

  QStylePainter painter(this);
  painter.drawControl(QStyle::CE_CheckBox, option);

  const QRect text_rect = style()->subElementRect(
      QStyle::SE_CheckBoxContents, &option, this);
  style()->drawItemText(&painter, text_rect,
                        Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                        option.palette, isEnabled(), source_text_,
                        foregroundRole());
}

int WrappingCheckBox::TextWidthForWidgetWidth(int width) const {
  QStyleOptionButton option;
  initStyleOption(&option);
  const int indicator_width =
      style()->pixelMetric(QStyle::PM_IndicatorWidth, &option, this);
  const int label_spacing =
      style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &option, this);
  return std::max(1, width - indicator_width - label_spacing);
}

QRect WrappingCheckBox::TextRectForWidgetWidth(int width) const {
  const int text_width = TextWidthForWidgetWidth(width);
  const QRect bounding_rect =
      QFontMetrics(font()).boundingRect(0, 0, text_width, 1 << 20,
                                        Qt::AlignLeft | Qt::AlignTop |
                                            Qt::TextWordWrap,
                                        source_text_);
  return QRect(0, 0, text_width, bounding_rect.height());
}

int WrappingCheckBox::TextHeightForWidth(int width) const {
  QStyleOptionButton option;
  initStyleOption(&option);
  const QRect text_rect = TextRectForWidgetWidth(width);
  const int indicator_height =
      style()->pixelMetric(QStyle::PM_IndicatorHeight, &option, this);
  return std::max(indicator_height, text_rect.height());
}

}  // namespace gui
}  // namespace mozc
