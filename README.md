# [기술 블로그](https://jeongjoohong.github.io/blog/)

[![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)](https://raw.githubusercontent.com/jeongjoohong/blog/master/LICENSE)

Jekyll을 이용하여 생성된 블로그의 저장소 입니다. 프로젝트의 블로그를 작성하는데 사용하고 있습니다.
변경기록은 [CHANGELOG.md](https://github.com/jeongjoohong/blog/blob/gh-pages/CHANGELOG.md)를 통해 확인할 수 있습니다.

---

## Features

- Compatible with Jekyll 3.x and GitHub Pages
- Several responsive layout options (single, archive index, splash, and paginated home page)
- UI localized text in Korean

## Pages

| Name    | Description |
| ------- | ----------- |
| [home](https://jeongjoohong.github.io/project/) | Paginated home page. |
| [category](https://jeongjoohong.github.io/project/categories/) | Posts grouped by category. |
| [tag](https://jeongjoohong.github.io/project/tags/) | Posts grouped by tags. |


## Structure

```
project
├── _data                      # data files for customizing the theme
|  ├── navigations.yml         # main navigation links
|  └── ui-text.yml             # text used throughout the theme's UI
├── _includes
|  ├── analytics-providers     # snippets for analytics (Google and custom)
|  ├── comments-providers      # snippets for comments (Disqus, Facebook, Google+, and custom)
|  ├── footer                  # custom snippets to add to site footer
|  ├── head                    # custom snippets to add to site head
|  ├── base_path               # site.url + site.baseurl shortcut
|  ├── feature_row             # feature row helper
|  ├── gallery                 # image gallery helper
|  ├── group-by-array          # group by array helper for archives
|  ├── nav_list                # navigation list helper
|  ├── toc                     # table of contents helper
|  └── ...
├── _layouts
|  ├── archive-taxonomy.html   # tag/category archive for Jekyll Archives plugin
|  ├── archive.html            # archive listing documents in an array
|  ├── compress.html           # compresses HTML in pure Liquid
|  ├── default.html            # base for all other layouts
|  ├── home.html               # home page
|  ├── single.html             # single document (post/page/etc)
|  └── splash.html             # splash page
├── _sass                      # SCSS partials
├── assets
|  ├── css
|  |  └── main.scss            # main stylesheet, loads SCSS partials from _sass
|  ├── fonts
|  |  └── fontawesome-webfont  # Font Awesome webfonts
|  ├── images                  # image assets for posts/pages/collections/etc.
|  ├── js
|  |  ├── plugins              # jQuery plugins
|  |  ├── vendor               # vendor scripts
|  |  ├── _main.js             # plugin settings and other scripts to load after jQuery
|  |  └── main.min.js          # optimized and concatenated script file loaded before </body>
├── _config.yml                # site configuration
├── Gemfile                    # gem file dependencies
├── index.html                 # paginated home page showing recent posts
└── package.json               # NPM build scripts
```

## Stylesheets

```
project
├── _sass
|  ├── vendor               # vendor SCSS partials
|  |   ├── breakpoint       # media query mixins
|  |   ├── font-awesome     # Font Awesome icons
|  |   ├── magnific-popup   # Magnific Popup lightbox
|  |   └── susy             # Susy grid system
|  ├── _animations.scss     # animations
|  ├── _archive.scss        # archives (list, grid, feature views)
|  ├── _base.scss           # base HTML elements
|  ├── _buttons.scss        # buttons
|  ├── _footer.scss         # footer
|  ├── _masthead.scss       # masthead
|  ├── _mixins.scss         # mixins (em function, clearfix)
|  ├── _navigation.scss     # nav links (breadcrumb, priority+, toc, pagination, etc.)
|  ├── _notices.scss        # notices
|  ├── _page.scss           # pages
|  ├── _print.scss          # print styles
|  ├── _reset.scss          # reset
|  ├── _sidebar.scss        # sidebar
|  ├── _syntax.scss         # syntax highlighting
|  ├── _tables.scss         # tables
|  ├── _utilities.scss      # utility classes (text/image alignment)
|  └── _variables.scss      # theme defaults (fonts, colors, etc.)
├── assets
|  ├── css
|  |  └── main.scss         # main stylesheet, loads SCSS partials in _sass
```

## JavaScript

```
project
├── assets
|  ├── js
|  |  ├── plugins
|  |  |   ├── jquery.fitvids.js            # fluid width video embeds
|  |  |   ├── jquery.greedy-navigation.js  # priority plus navigation
|  |  |   ├── jquery.magnific-popup.js     # responsive lightbox
|  |  |   └── jquery.smooth-scroll.min.js  # make same-page links scroll smoothly
|  |  ├── vendor
|  |  |   └── jquery
|  |  |       └── jquery-1.12.1.min.js
|  |  ├── _main.js                         # jQuery plugin settings and other scripts
|  |  └── main.min.js                      # concatenated and minified scripts
```

---

## Credits

- [Jekyll](http://jekyllrb.com/)
- [Minimal Mistakes Jekyll Theme](https://mmistakes.github.io/minimal-mistakes/)

## License

See the [LICENSE](https://github.com/jeongjoohong/project/blob/gh-pages/LICENSE) file.
