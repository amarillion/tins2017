#!/usr/bin/bash -e

BASE=/mingw32/bin
ALLEGRO_BASE=/mingw32/i686-w64-mingw32/bin
WINDOWS_BASE=/c/Windows/System32

mkdir -p build/{release,debug}_win

cp $ALLEGRO_BASE/allegro_monolith-5.2.dll build/release_win
cp $ALLEGRO_BASE/allegro_monolith-debug-5.2.dll build/debug_win

for DEST in "build/release_win" "build/debug_win"
do
	# cp $BASE/libgcc_s_seh-1.dll $DEST # 64-bit only
	cp $BASE/libgcc_s_dw2-1.dll $DEST

	cp $BASE/libstdc++-6.dll $DEST
	cp $BASE/libfreetype-6.dll $DEST
	cp $BASE/libbz2-1.dll $DEST
	cp $BASE/libharfbuzz-0.dll $DEST
	cp $BASE/libglib-2.0-0.dll $DEST
	cp $BASE/libogg-0.dll $DEST
	cp $BASE/libpng16-16.dll $DEST
	cp $BASE/libFLAC-8.dll $DEST
	cp $BASE/libphysfs.dll $DEST
	cp $BASE/libintl-8.dll $DEST
	cp $BASE/libtheoradec-1.dll $DEST
	cp $BASE/libvorbis-0.dll $DEST
	cp $BASE/libvorbisfile-3.dll $DEST
	cp $BASE/libdumb.dll $DEST
	cp $BASE/zlib1.dll $DEST
	cp $BASE/libwinpthread-1.dll $DEST
	cp $BASE/libiconv-2.dll $DEST
	cp $BASE/libgraphite2.dll $DEST
	cp $BASE/libpcre-1.dll $DEST
	cp $WINDOWS_BASE/xinput1_3.dll $DEST

	echo Checking $DEST/*.exe
	pushd $DEST
	for i in $(objdump -p *.exe *.dll | grep 'DLL Name:' | sort | uniq | sed "s/\s*DLL Name: //")
	do
		if [ -e $i ]
		then
			echo "FOUND: $i"
		else
			echo "MISSING: $i"
		fi
	done
	popd

done
