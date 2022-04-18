#include "Eyestack/Design/AboutEyestack.hpp"
#include "Eyestack/Asset.hpp"
#include "Eyestack/version.hpp"

namespace Eyestack::Design {

constexpr auto faicon = Asset::faicon;

AboutEyestack::AboutEyestack(QWidget* parent)
  : _S(parent)
  , _aboutQt(tr("关于 Qt"))
{
  auto esIcon = Asset::icon("eyestack");

  //  setWindowIcon(esIcon);
  setWindowTitle(tr("关于 Eyestack"));

  setLayout(&_mainLayout);
  _mainLayout.setSizeConstraint(QLayout::SetFixedSize);

  _mainLayout.addLayout(&_upperLayout);
  {
    _upperLayout.addWidget(&_esLogo);
    _esLogo.setPixmap(esIcon.pixmap(128, 128));

    _upperLayout.addLayout(&_rightLayout);
    {
      _rightLayout.addWidget(&_esInfo);
      _esInfo.setTextFormat(Qt::MarkdownText);
      _esInfo.setText(tr("# Eyestack " Eyestack_VERSION "\n"
                         "Build: **" __DATE__ " - " __TIME__ "**\n"
                         "\n---\n\n"
                         "Copyright © 2021-Now IICT. All rights reserved.\n"));

      _rightLayout.addWidget(&_aboutQt, 0, Qt::AlignRight | Qt::AlignBottom);
      _aboutQt.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      connect(
        &_aboutQt, &QPushButton::clicked, this, &_T::when_aboutQt_clicked);
    }
  }

  _mainLayout.addWidget(&_tabWidget);
  _tabWidget.addTab(&_tabFontAwesome, tr("Font Awesome"));

  _tabFontAwesome.setWordWrapMode(QTextOption::NoWrap);
  _tabFontAwesome.setText(R"(Font Awesome Free License
-------------------------

Font Awesome Free is free, open source, and GPL friendly. You can use it for
commercial projects, open source projects, or really almost whatever you want.
Full Font Awesome Free license: https://fontawesome.com/license/free.

# Icons: CC BY 4.0 License (https://creativecommons.org/licenses/by/4.0/)
In the Font Awesome Free download, the CC BY 4.0 license applies to all icons
packaged as SVG and JS file types.

# Fonts: SIL OFL 1.1 License (https://scripts.sil.org/OFL)
In the Font Awesome Free download, the SIL OFL license applies to all icons
packaged as web and desktop font files.

# Code: MIT License (https://opensource.org/licenses/MIT)
In the Font Awesome Free download, the MIT license applies to all non-font and
non-icon files.

# Attribution
Attribution is required by MIT, SIL OFL, and CC BY licenses. Downloaded Font
Awesome Free files already contain embedded comments with sufficient
attribution, so you shouldn't need to do anything additional when using these
files normally.

We've kept attribution comments terse, so we ask that you do not actively work
to remove them from files, especially code. They're a great way for folks to
learn about Font Awesome.

# Brand Icons
All brand icons are trademarks of their respective owners. The use of these
trademarks does not indicate endorsement of the trademark holder by Font
Awesome, nor vice versa. **Please do not use brand logos for any purpose except
to represent the company, product, or service to which they refer.**
)");
}

}
