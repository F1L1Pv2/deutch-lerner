#/bin/sh

set -xe

sh build.sh
rm -rfv release
mkdir -p release
cp main.exe release
cp -r audio release/audio
cp -r presets release/presets

tar -cvzf deutcheLerner.tar -C release .