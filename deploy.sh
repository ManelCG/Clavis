#! /bin/bash
#
# deploy.sh - Full release driver for Clavis.
#
# Verifies the version was bumped (and bumps it interactively if not), rebuilds
# so the version-dependent files are regenerated, captures a changelog, tags +
# pushes the GitHub repo, and publishes both AUR packages.
#
# Single source of truth for the version is CMakeLists.txt:
#     set(CLAVIS_VERSION "X.Y.Z")
#     set(VERSION_PATCH  "N")      # == pkgrel
# The release tag is  v${CLAVIS_VERSION}-${VERSION_PATCH}.
# CMake configure_file regenerates aur-clavis*/PKGBUILD, clavis.desktop and
# Info.plist from those values on every ./make.sh, so the bump is written once.

set -euo pipefail

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR"

RELEASE_BRANCH="develop"
MAIN_BRANCH="main"
REMOTE="origin"
CMAKELISTS="CMakeLists.txt"
CHANGELOG="CHANGELOG.md"
AUR_SUBMODULES=("aur-clavis" "aur-clavis-git")

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
die()  { echo -e "\033[1;31mERROR:\033[0m $*" >&2; exit 1; }
info() { echo -e "\033[1;34m==>\033[0m $*"; }
warn() { echo -e "\033[1;33mWARN:\033[0m $*" >&2; }

confirm() {
  # confirm "Question?"  -> returns 0 on yes
  local reply
  read -r -p "$1 [y/N] " reply
  [[ "$reply" =~ ^[Yy]$ ]]
}

Usage() {
  cat <<EOF
Usage: $0 [--help]

Performs a full Clavis release:
  1. Checks the version in $CMAKELISTS is bumped (prompts to bump otherwise).
  2. Builds (./make.sh) to regenerate version-dependent files.
  3. Prompts for changelog notes -> $CHANGELOG + annotated git tag.
  4. Commits, tags, merges $RELEASE_BRANCH -> $MAIN_BRANCH, and pushes.
  5. Regenerates .SRCINFO and pushes both AUR packages.

Run it from a clean working tree on the '$RELEASE_BRANCH' branch.
EOF
}

read_version() {
  CLAVIS_VERSION=$(sed -n 's/^set(CLAVIS_VERSION "\([^"]*\)").*/\1/p' "$CMAKELISTS")
  VERSION_PATCH=$(sed -n 's/^set(VERSION_PATCH "\([^"]*\)").*/\1/p' "$CMAKELISTS")
  [ -n "$CLAVIS_VERSION" ] || die "Could not read CLAVIS_VERSION from $CMAKELISTS"
  [ -n "$VERSION_PATCH" ]  || die "Could not read VERSION_PATCH from $CMAKELISTS"
  TAG="v${CLAVIS_VERSION}-${VERSION_PATCH}"
}

tag_exists() { git rev-parse -q --verify "refs/tags/$1" >/dev/null 2>&1; }

write_version() {
  # write_version <version> <patch>
  sed -i "s/^set(CLAVIS_VERSION \"[^\"]*\")/set(CLAVIS_VERSION \"$1\")/" "$CMAKELISTS"
  sed -i "s/^set(VERSION_PATCH \"[^\"]*\")/set(VERSION_PATCH \"$2\")/"   "$CMAKELISTS"
}

if [ "${1:-}" == "--help" ] || [ "${1:-}" == "-h" ]; then Usage; exit 0; fi

# ---------------------------------------------------------------------------
# 1. Pre-flight
# ---------------------------------------------------------------------------
info "Pre-flight checks"

command -v git      >/dev/null || die "git is required"
command -v makepkg  >/dev/null || die "makepkg is required (run on Arch Linux)"

CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
[ "$CURRENT_BRANCH" == "$RELEASE_BRANCH" ] || \
  die "Must be on '$RELEASE_BRANCH' (currently on '$CURRENT_BRANCH')."

# Only uncommitted *tracked* changes block a release; untracked local files
# (build dirs, editor configs, etc.) are fine to leave in place.
[ -z "$(git status --porcelain -uno)" ] || \
  die "You have uncommitted changes to tracked files. Commit or stash them first."

info "Fetching $REMOTE..."
git fetch "$REMOTE" --tags

if git rev-parse --verify -q "$REMOTE/$RELEASE_BRANCH" >/dev/null; then
  BEHIND=$(git rev-list --count "HEAD..$REMOTE/$RELEASE_BRANCH")
  [ "$BEHIND" -eq 0 ] || \
    die "$RELEASE_BRANCH is $BEHIND commit(s) behind $REMOTE/$RELEASE_BRANCH. Pull first."
fi

info "Ensuring AUR submodules are checked out..."
git submodule update --init "${AUR_SUBMODULES[@]}"

# ---------------------------------------------------------------------------
# 2. Read version + bump check
# ---------------------------------------------------------------------------
read_version
info "CMakeLists version: $CLAVIS_VERSION-$VERSION_PATCH  (tag $TAG)"

if tag_exists "$TAG"; then
  warn "Tag $TAG already exists -> version has NOT been bumped."

  # -------------------------------------------------------------------------
  # 3. Interactive bump
  # -------------------------------------------------------------------------
  IFS='.' read -r MAJOR MINOR PATCH <<< "$CLAVIS_VERSION"
  echo
  echo "Current version: $CLAVIS_VERSION (pkgrel $VERSION_PATCH)"
  echo "Choose a bump:"
  echo "  1) Packaging patch  -> $CLAVIS_VERSION-$((VERSION_PATCH + 1))   (rebuild, same source)"
  echo "  2) Patch            -> $MAJOR.$MINOR.$((PATCH + 1))-1"
  echo "  3) Minor            -> $MAJOR.$((MINOR + 1)).0-1"
  echo "  4) Major            -> $((MAJOR + 1)).0.0-1"
  echo "  5) Custom"
  read -r -p "Selection [1-5]: " CHOICE

  case "$CHOICE" in
    1) NEW_VERSION="$CLAVIS_VERSION";              NEW_PATCH=$((VERSION_PATCH + 1)) ;;
    2) NEW_VERSION="$MAJOR.$MINOR.$((PATCH + 1))"; NEW_PATCH=1 ;;
    3) NEW_VERSION="$MAJOR.$((MINOR + 1)).0";      NEW_PATCH=1 ;;
    4) NEW_VERSION="$((MAJOR + 1)).0.0";           NEW_PATCH=1 ;;
    5) read -r -p "New version (X.Y.Z): " NEW_VERSION
       read -r -p "New patch (pkgrel): " NEW_PATCH ;;
    *) die "Invalid selection." ;;
  esac

  [[ "$NEW_VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || die "Invalid version: $NEW_VERSION"
  [[ "$NEW_PATCH" =~ ^[0-9]+$ ]]                   || die "Invalid patch: $NEW_PATCH"

  NEW_TAG="v${NEW_VERSION}-${NEW_PATCH}"
  tag_exists "$NEW_TAG" && die "Tag $NEW_TAG already exists too."

  confirm "Bump $CMAKELISTS to $NEW_VERSION-$NEW_PATCH?" || die "Aborted."
  write_version "$NEW_VERSION" "$NEW_PATCH"
  read_version
  info "Bumped to $CLAVIS_VERSION-$VERSION_PATCH (tag $TAG)"
else
  info "Version is already bumped -> releasing $TAG"
fi

# ---------------------------------------------------------------------------
# 4. Build (regenerates PKGBUILDs, clavis.desktop, Info.plist)
# ---------------------------------------------------------------------------
info "Building (./make.sh) to regenerate version-dependent files..."
./make.sh || die "Build failed."

# ---------------------------------------------------------------------------
# 5. Changelog
# ---------------------------------------------------------------------------
info "Composing changelog for $TAG"

LAST_TAG=$(git describe --tags --abbrev=0 2>/dev/null || true)
NOTES_FILE=$(mktemp)
trap 'rm -f "$NOTES_FILE"' EXIT

{
  if [ -n "$LAST_TAG" ]; then
    git log --pretty='- %s' "$LAST_TAG"..HEAD
  else
    git log --pretty='- %s'
  fi
  echo
  echo "# Release notes for $TAG."
  echo "# Lines starting with '#' are ignored. Edit the list above, save and quit."
  if [ -n "$LAST_TAG" ]; then
    echo "# Commits shown are $LAST_TAG..HEAD."
  fi
} > "$NOTES_FILE"

"${EDITOR:-vim}" "$NOTES_FILE"

# Drop comment lines and trailing blank lines.
NOTES=$(grep -v '^#' "$NOTES_FILE" | sed -e 's/[[:space:]]*$//' | sed -e '/./,$!d' | tac | sed -e '/./,$!d' | tac)
[ -n "$NOTES" ] || die "Empty changelog, aborting."

# Rewrite the cleaned notes back to the file for the annotated tag message.
printf '%s\n' "$NOTES" > "$NOTES_FILE"

# Prepend a section to CHANGELOG.md (Keep-a-Changelog style).
NEW_CHANGELOG=$(mktemp)
{
  if [ ! -f "$CHANGELOG" ]; then
    echo "# Changelog"
    echo
  fi
  echo "## [${CLAVIS_VERSION}-${VERSION_PATCH}] - $(date +%F)"
  echo
  printf '%s\n' "$NOTES"
  echo
  [ -f "$CHANGELOG" ] && cat "$CHANGELOG"
} > "$NEW_CHANGELOG"
mv "$NEW_CHANGELOG" "$CHANGELOG"
info "Updated $CHANGELOG"

# ---------------------------------------------------------------------------
# 6. Commit + annotated tag
# ---------------------------------------------------------------------------
info "Committing release and tagging $TAG"
git add "$CMAKELISTS" "$CHANGELOG"
git commit -m "Release $TAG"
git tag -a "$TAG" -F "$NOTES_FILE"

# ---------------------------------------------------------------------------
# 7. Publish main repo
# ---------------------------------------------------------------------------
info "Merging $RELEASE_BRANCH -> $MAIN_BRANCH and pushing"
confirm "Push $TAG to $REMOTE (branches $RELEASE_BRANCH, $MAIN_BRANCH + tag)?" || die "Aborted before push."

git checkout "$MAIN_BRANCH"
git merge --no-ff "$RELEASE_BRANCH" -m "Release $TAG"
git checkout "$RELEASE_BRANCH"
git push "$REMOTE" "$RELEASE_BRANCH" "$MAIN_BRANCH"
git push "$REMOTE" "$TAG"

# ---------------------------------------------------------------------------
# 8. Publish AUR packages
# ---------------------------------------------------------------------------
AUR_UPDATED=()
for sub in "${AUR_SUBMODULES[@]}"; do
  info "Publishing AUR package: $sub"
  (
    cd "$sub"
    git checkout master
    makepkg --printsrcinfo > .SRCINFO
    if [ -z "$(git status --porcelain -- PKGBUILD .SRCINFO)" ]; then
      warn "No version change in $sub, skipping commit."
      exit 0
    fi
    git add PKGBUILD .SRCINFO
    git commit -m "$TAG"
    git push
  )
  # Track which submodule pointers actually moved.
  if [ -n "$(git status --porcelain -- "$sub")" ]; then
    AUR_UPDATED+=("$sub")
  fi
done

# Record moved AUR submodule pointers in the main repo (optional).
if [ "${#AUR_UPDATED[@]}" -gt 0 ]; then
  if confirm "Record updated AUR submodule pointers (${AUR_UPDATED[*]}) in $RELEASE_BRANCH?"; then
    git add "${AUR_UPDATED[@]}"
    git commit -m "Bump AUR submodules to $TAG"
    git push "$REMOTE" "$RELEASE_BRANCH"
  fi
fi

# ---------------------------------------------------------------------------
# 9. Done
# ---------------------------------------------------------------------------
echo
info "Release $TAG complete."
echo "  - Tagged and pushed $RELEASE_BRANCH + $MAIN_BRANCH to $REMOTE"
echo "  - Annotated tag $TAG carries the changelog (GitHub release notes)"
echo "  - AUR packages updated: ${AUR_UPDATED[*]:-none}"
