# FogBuddy
## What is this?
FogBuddy is a third party application that allows searching and automatic equipping of perks in the game Dead by Daylight.
Its implemented entirely using computer vision and doesn't mess with any game files whatsoever.

## Cool! How does it work?
 - for more insight on the computer vision algorithms, I would suggest checking out the python notebooks in [extra](extra/).
They might not always be up to date, but its a lot easier to tinker with/understand whats going on.
 - the UI is made using [Dear ImGui](https://github.com/ocornut/imgui).
 - screenshots and user input are implemented with Windows syscalls (*Windows.h*).
 - perk images are scraped with JavaScript from the Dead by Daylight [wiki](https://deadbydaylight.fandom.com/).
 - the project uses C++17 and DX11 (the python notebooks are just for tinkering, and scraping images is more of a pre-build step).

## Building
Clone the repository.

`git clone https://github.com/MrTipson/FogBuddy.git`

Download the latest [premake](https://premake.github.io/) executable and check [here](https://premake.github.io/docs/using-premake) if your preferred way of building is available.

Download the [opencv](https://opencv.org/releases) library (make sure the version matches with the includes in the build [config](premake5.lua)). Install location doesn't matter.

Run premake and pass the install location as an argument.

`premake5.exe --opencv="C:\path\to\opencv" [action]`

for example:

`premake5.exe --opencv="X:\opencv" vs2022`

Before building the project, perk images also need to be obtained. This can be done either by copying from the folders from an official build or running the JavaScript script:

```
cd extra
npm i
node getPerks.js
```
> If you get an error when running node, its possible that your node installation doesn't support null propagation operators yet.

> The script will not override existing files. When updating images, remove the *data/Killers* and *data/Survivors* directories.

The script should only be ran once (and each time new perks are added).
