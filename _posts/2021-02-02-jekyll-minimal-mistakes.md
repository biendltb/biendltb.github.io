---
layout: single
title: "Jekyll and Minimal Mistakes for Github Pages"
date: 2021-01-03 15:44:00 +0700
categories: tech
classes:
  - dark-theme
tags:
  - jekyll
  - minimal mistakes
  - github pages
---

Create your own personal blog with Jekyll and Minimal Mistakes theme in some simple steps.

## Generate the repo

1. Click [here](https://github.com/mmistakes/mm-github-pages-starter/generate) to generate a repo which is readily set up for Jekyll and Minimal Mistakes theme.

2. Name your repo as _`<your_github_username>.github.io`_

## Development in a local machine

1. Clone your repo to a folder

2. Uninstall the default Ruby version in your PC

3. Install Ruby gems with `rbenv` ([ref](https://gorails.com/setup/ubuntu/18.04))

    Note: Try to not go for a too new version if you do not needed (e.g. 3.0.0 gives me errors while 2.7.2 does not).

```bash
cd
git clone https://github.com/rbenv/rbenv.git ~/.rbenv
echo 'export PATH="$HOME/.rbenv/bin:$PATH"' >> ~/.bashrc
echo 'eval "$(rbenv init -)"' >> ~/.bashrc
exec $SHELL

git clone https://github.com/rbenv/ruby-build.git ~/.rbenv/plugins/ruby-build
echo 'export PATH="$HOME/.rbenv/plugins/ruby-build/bin:$PATH"' >> ~/.bashrc
exec $SHELL

rbenv install 2.7.2
rbenv global 2.7.2
ruby -v
```

4. Install Jekyll and Bundler

```bash
gem install jekyll bundler
```

5. Launch your site

```bash

cd /path/to/your/repo

bundler exec jekyll serve
```

Now you could modify your site according to [guides](https://mmistakes.github.io/minimal-mistakes/docs/configuration/) from Minimal Mistakes.


## Modify a layout

Minimal Mistakes gives you many layouts. They are hide in its gem library for recent versions. If there is something you would like to change in the layout. First, check it in the defaults configs of `_config.yaml` file. 

If you cannot change it there, following below steps:

1. Create a `_layouts` folder in the root of your repo
```bash
mkdir _layouts
```

2. To get the default layouts of the theme, you first need to let the bundle download and install it in your machine. Adding bellow lines to the `Gemfile` in your repo and remove it later. Push it to your remote could create error for the Github Pages since it supports a very limit number of plugins.
```txt
gem "minimal-mistakes-jekyll"
```

3. Let bundle install the theme in your local by running
```bash
bundle
```

4. Get the path to the theme layouts
```bash
bundle info minimal-mistakes-jekyll
```

5. You could find the `_layouts` folder in the path (e.g. `/home/biendltb/.rbenv/versions/2.7.2/lib/ruby/gems/2.7.0/gems/minimal-mistakes-jekyll-4.21.0/_layouts/`).

    Copy the layout file that you want to modify to the `_layouts` folder in your repo. Modify it and push to the remote.
