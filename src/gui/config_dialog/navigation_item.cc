
#include "gui/config_dialog/navigation_item.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QWidget>

namespace mozc {
namespace gui {

NavigationItem::NavigationItem(const QString &title, QWidget *parent)
        : QWidget(parent) {
  QLabel *titleLabel = new QLabel(title);
  titleLabel->setStyleSheet("background-color: transparent;padding-bottom: 2px");

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(titleLabel);
  //mainLayout->setSpacing(16);
  mainLayout->setContentsMargins(16, 24, 16, 24);
}
}  // namespace gui
}  // namespace mozc
