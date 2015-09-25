## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

ishare_add_flag want-dotfiles-import

:> "${DOTFILES_SRC_LIST_FILE:?}" || \
   die "Failed to initialize dotfiles config!"


if [ -s "${DOTFILES_SRC_LIST_FILE}.in.file" ]; then
{
awk \
-v tre="${TARBALL_FILEEXT_RE:?}" \
-v gh="@@GITHUB_ARCHIVE_TYPE@@://@@GITHUB_ARCHIVE_SITE@@" \
\
'
( $2 != "" ) { exit 9; }
{ ok=0; t = "bad"; }

( $1 ~ ("^"gh".*/tarball") )   { t="tarball"; ok=1; }
( $1 ~ ("^\\S+" tre "$") )     { t="tarball"; ok=1; }

(!ok) { printf("bad dotfiles uri: %s\n",$1) > "/dev/stderr"; exit 9; }
{ printf("%s ",t); print; }
' "${DOTFILES_SRC_LIST_FILE}.in.file"
} >> "${DOTFILES_SRC_LIST_FILE}" || die "Failed to add files to dotfiles config!"

fi

if [ -s "${DOTFILES_SRC_LIST_FILE}.in.git" ]; then
   sed -e 's=^=git =' \
      < "${DOTFILES_SRC_LIST_FILE}.in.git" \
      >> "${DOTFILES_SRC_LIST_FILE}" || \
      die "Failed to add git repos to dotfiles config!"
fi

autodie rm -- "${DOTFILES_SRC_LIST_FILE:?}.in.file"
autodie rm -- "${DOTFILES_SRC_LIST_FILE:?}.in.git"
