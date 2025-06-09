CLAVIS_VERSION=$(cat CMakeLists.txt | grep "set(CLAVIS_VERSION" | sed -n 's/^set(CLAVIS_VERSION "\([0-9.]*\)").*/\1/p')


