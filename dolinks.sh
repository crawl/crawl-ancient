#!/bin/sh

mkdir -p NORMAL
mkdir -p WIZARD

pushd NORMAL ; ln -s ../Source/*.h ../Source/*.cc ../Source/makefile* . ; popd
pushd WIZARD ; ln -s ../Source/*.h ../Source/*.cc ../Source/makefile* . ; popd
