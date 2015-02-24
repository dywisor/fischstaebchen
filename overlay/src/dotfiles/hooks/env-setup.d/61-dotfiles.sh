## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

ishare_add_flag want-dotfiles-import

:> "${DOTFILES_SRC_LIST_FILE:?}" || \
   die "Failed to initialize dotfiles config!"

[ -s "${DOTFILES_SRC_LIST_FILE}.in" ] || return 0

if grep -v -- "^\S+${TARBALL_FILEEXT_RE}\$" \
   "${DOTFILES_SRC_LIST_FILE}"
then
   die "bad dotfiles config."
fi

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
' "${DOTFILES_SRC_LIST_FILE}.in"
} > "${DOTFILES_SRC_LIST_FILE}" || die "Failed to create dotfiles config!"


autodie rm -- "${DOTFILES_SRC_LIST_FILE:?}.in"
