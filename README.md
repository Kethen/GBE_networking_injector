### GBE_networking_injector

When you want to use GBE only for networking, not other API calls

#### Installation

1. Place steamclient.dll / steamclient64.dll next to game exe (backup existing files)
2. Place MinHook.x86.dll / MinHook.x64.dll next to game exe
3. Place steam_api.dll / steam_api64.dll as goldberg.bsi / goldberg64.bsi next to game exe (backup existing files)
4. Place steamclient_loader.exe from GBE next to game exe, configure ColdClientLoader.ini accordingly
5. Launch steamclient_loader.exe or game exe, depending on whether the game requires injection to load steamclient.dll
