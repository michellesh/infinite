### Set up symbolic link to sync changes between repo library and Arduino library

Assuming your github repo is `~/dev/repo` and your Arduino sketchbook location is `~/Documents/Arduino` (see your Arduino IDE settings)

1. First, ensure that the directories `~/dev/repo/libraries/RepoShared/` and `~/Documents/Arduino/libraries/RepoShared/` exist.
2. Move or create the `RepoShared.h` file in `~/dev/repo/libraries/RepoShared/`.
3. Open Terminal and navigate to the `~/Documents/Arduino/libraries/RepoShared/` directory.
4. Create a symbolic link to the `RepoShared.h` file in `~/dev/repo/libraries/RepoShared/` using the `ln` command with the `-s` option:

```
ln -s ~/dev/repo/libraries/RepoShared/RepoShared.h ~/Documents/Arduino/libraries/RepoShared/RepoShared.h
```

Now, whenever you make changes to either file, the same changes will be reflected in the other file since they are essentially pointing to the same file.


#### For this repo

```
cd ~/Documents/Arduino/libraries/InfiniteShared/
ln -s ~/dev/infinite/libraries/InfiniteShared/InfiniteShared.h ~/Documents/Arduino/libraries/InfiniteShared/InfiniteShared.h
```
