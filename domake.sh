#!/bin/sh
#jmf: automate making normal & wizard binaries, until they merge
#jmf: N.B.: requires symlink directories; untar symlinks.tgz to use

pushd NORMAL && make && popd && pushd WIZARD && make wizard ; popd
