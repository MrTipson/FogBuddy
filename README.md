# FogBuddy
## What is this?
FogBuddy is a third party application that allows searching and automatic equipping of perks in the game Dead by Daylight.
Its implemented entirely using computer vision and doesn't mess with any game files whatsoever.

Here is a quick demo, although it does not show some features, which have been added since:

https://user-images.githubusercontent.com/37011447/233488966-e2d30c5d-1f5a-4dd0-9891-0088e5e16abb.mp4


## Risks
Before going forward, its important to assess the risks associated with this. The relevant documents for this are the [Game rules](https://forum.deadbydaylight.com/en/discussion/19450/game-rules-and-report-system), [Terms of use](https://www.bhvr.com/terms-of-use/) and the [End user licencing aggrement](https://deadbydaylight.com/eula) (**EULA**  from now on).

The main concern is one of the provisions included in *Rules of conduct*, which can be found in both **Terms of use** (section E) and **EULA** (section I). It states that it is a violation if you: `Use or distribute “auto” software programs, “macro” software programs or other “cheat utility” software program or applications.` . This utility definately falls under this, even though I believe the rule is more intended to target bots and cheats.

The second, smaller, concern is the *Other hacks* section of the **Game rules**. It states: `Using 3rd party software or other tools to gain any kind of unfair advantage that wasn’t intended by the game, except if they have been whitelisted by us.` . **Game rules** are more meant for each match, and I don't know if searching perks is an 'unfair advantage', but I still wanted to at least mention it.

The third concern is anti cheat. As this tool only makes screenshots and controls the mouse movement, I believe its unlikely **Easy-Anti Cheat** will trigger. It certainly hasn't happened during development.

I've attempted to reach out to support to get some sort of statement from BHVR, and this is what I ended up getting:

> Thank you for the report enquiry given regarding adding a search tool for more efficiency in searching and equipping tool. Although technically speaking - This is not a bannable offense by our game rules. There is a possibility that you may be ban by our Easy-Anti Cheats (EAC) system as you're using 3rd party software.
> 
>And as we do not encourage players to use any sorts of 3rd party software, you may have to handle with risk if you choose to go with this.

> If anyone at BHVR would like to reach out for any reason, please contact me at the email address with which this ticket was made (September 29, 2022) or on twitter.

## Personal responsibility
If the previous section hasn't made it clear, *im taking this seriously*. **You should too**. I've outlined the risks involved, but if you use this, you are choosing to accept them. If I had to feel responsible for everyone, I just wouldn't release this to the public. *Its as simple as that*.

## You accept the risks? Here is how to install:
Download the latest zip file from the [releases](https://github.com/MrTipson/FogBuddy/releases) and extract the contents anywhere.

## How to use
Start *FogBuddy.exe* and Dead by Daylight (order doesn't matter). If you restart Dead by Daylight, you must restart *FogBuddy* as well.

Every time you swap characters (or change UI size), you have to click on *recalibrate*.
> Recalibrating is not necessary if you're swapping between characters that have the same amount of pages.

The program works best if you have all perks on tier 3, it can handle missing DLCs/perks.

#### Controls
You can toggle the overlay using **Alt+E**.

When you click on a perk, a dot will appear next to the window name. Please do not move your mouse until the dot disappears.

You can also use keyboard controls. Arrow keys move between different elements, space is used to select an item and tab is useful for focusing the input field.
> Sometimes selecting a perk using space does not work first try. While this is being looked into, pay attention to the activity dot to see when its actually doing something.

#### Logging
If you've got 2 monitors, having the *FogBuddy* open on the side might be useful to notice if its having issues. If you submit any [issues](https://github.com/MrTipson/FogBuddy/issues), posting relevant debug log information could also be helpful in resolving them. But please don't post issues where the program just didnt find the perk, even if it was on the page. The detection accuracy is an ongoing battle and there is no need for additional issue posts.

## Cool! How does it work?
 - for more insight on the computer vision algorithms, I would suggest checking out the python notebooks in [extra](extra/).
They might not always be up to date, but its a lot easier to tinker with/understand whats going on.
 - the UI is made using [Dear ImGui](https://github.com/ocornut/imgui).
 - screenshots and user input are implemented with Windows syscalls (*Windows.h*).
 - perk images are scraped with JavaScript from the Dead by Daylight [wiki](https://deadbydaylight.fandom.com/).
 - the project uses C++17 and DX11 (the python notebooks are just for tinkering, and scraping images is more of a pre-build step).

## Contributing
You want to help? I've got a few things on the TO-DO list, such as:
 - focus issues with keyboard controls
 - improving template matching
 - improving robustness for different resolutions
 - supporting windowed mode

but if you have your own ideas, I'd be happy to hear them.

I'd also like to thank [Genergy](https://github.com/gendestry) for helping me with premake and C++ things.

## Building
Clone the repository.

```git clone --recurse-submodules https://github.com/MrTipson/FogBuddy.git```

Download the latest [premake](https://premake.github.io/) executable and check [here](https://premake.github.io/docs/using-premake) if your preferred way of building is available.

Download the [opencv](https://opencv.org/releases) library (make sure the version matches with the includes in the build [config](premake5.lua)). Install location doesn't matter.

Run premake and pass the install location as an argument.

```premake5.exe --opencv="C:\path\to\opencv" [action]```

for example:

```premake5.exe --opencv="X:\opencv" vs2022```

Before building the project, perk images also need to be obtained. This can be done either by copying from the folders from an official build or running the JavaScript script:

```
cd extra
npm i
node getPerks.js
```
> If you get an error when running node, its possible that your node installation doesn't support null propagation operators yet.

> The script will not override existing files. When updating images, remove the *data/Killers* and *data/Survivors* directories.

The script should only be ran once (and each time new perks are added).
