set -xe

rm -rf release
mkdir release

CC=i686-w64-mingw32-gcc
$CC -g -fPIC -c log.c -o log.o -O0
$CC -g -fPIC -c steamclient.c -o steamclient.o -Iminhook_1.3.3/include -O0
$CC -g -shared -static -o release/steamclient.dll steamclient.o log.o -Lminhook_1.3.3/bin -lMinHook.x86 -Wl,-Bstatic -lpthread

CC=x86_64-w64-mingw32-gcc
$CC -g -fPIC -c log.c -o log.o -O0
$CC -g -fPIC -c steamclient.c -o steamclient.o -Iminhook_1.3.3/include -O0
$CC -g -shared -static -o release/steamclient64.dll steamclient.o log.o -Lminhook_1.3.3/bin -lMinHook.x64 -Wl,-Bstatic -lpthread

cp minhook_1.3.3/bin/MinHook.x86.dll release/
cp minhook_1.3.3/bin/MinHook.x64.dll release/

