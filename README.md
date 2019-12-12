[![statusIcon](https://teamcity.ufna.dev/app/rest/builds/buildType:(id:UfnaDev_VaFogOfWar_ClangFormatCheck)/statusIcon.svg)](https://teamcity.ufna.dev/viewType.html?buildTypeId=UfnaDev_VaFogOfWar_ClangFormatCheck&guest=1)
![GitHub](https://img.shields.io/github/license/ufna/VaFogOfWar)
![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/ufna/VaFogOfWar?include_prereleases)

# VaFogOfWar

A clear and simple solution of Fog of War for Unreal Engine 4.

![SCREENSHOT](SCREENSHOT.jpg)

**VaFogOfWar** implements fast and optimized fog of war solution for topdown games like Dota, StarCraft or League of Legends. It fast enough even to work on mid-end mobile devices, so it works like a charm on desktop.

Check [wiki](http://bit.ly/VaFogOfWar-Docs) for usage examples and development notes.

### Main features

- Three types of layers: current visibility, global visibility (permanent from black), terrain
- Eight height levels supported (river, lowground, highground, etc.)
- Dynamic and static obstacles (trees, rocks, etc)
- Different types of radius strategy: circle, square and others
- Freeform fog blocking volumes as tool for terrain level painting
- Initial terrain levels can be set with heightmap

### Plugin versions

There are two versions of plugin:

1. Free Community version (which is already available) on Github. Just core C++ code to handle the fog, but no demo content and usage examples.

2. Paid Marketplace version: same as community version + demo content and extended usage example with preconfigured post process.
