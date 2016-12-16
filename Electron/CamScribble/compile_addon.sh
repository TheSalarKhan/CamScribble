#!/bin/bash
cd ObjectFactoryNodeNative;
HOME=~/.electron-gyp node-gyp rebuild --target=1.4.4 --arch=x64 --dist-url=https://atom.io/download/atom-shell;
cp ./build/Release/addon.node ../ElectronApplication/node_modules/;
