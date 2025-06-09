./make.sh

CLAVIS_VERSION=$(cat CMakeLists.txt | grep "set(CLAVIS_VERSION" | sed -n 's/^set(CLAVIS_VERSION "\([0-9.]*\)").*/\1/p')
CLAVIS_VERSION_NAME="v${CLAVIS_VERSION}"

git tag ${CLAVIS_VERSION_NAME}
git push
git push origin ${CLAVIS_VERSION_NAME}

cd aur-clavis
makepkg --printsrcinfo > .SRCINFO
git add PKGBUILD
git add .SRCINFO
git commit -m "${CLAVIS_VERSION_NAME}"
git push

cd ..

cd aur-clavis-git
makepkg --printsrcinfo > .SRCINFO
git add PKGBUILD
git add .SRCINFO
git commit -m "${CLAVIS_VERSION_NAME}"
git push

cd ..
