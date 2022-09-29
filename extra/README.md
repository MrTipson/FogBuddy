# Extras
This directory contains files that are useful during development, but don't get built/included in the installation.

## Python
The following notebooks are included:
 - *calibrate.ipynb* (calculates expected perk size)
 - *findPages.ipynb* (locates page switching buttons)
 - *findperks.ipynb* (finds a perk in the given image)

If you'd just like to peek at how the CV stuff is done, just click on one of the notebooks. GitHub has an integrated notebook viewer, which should show everything preety nicely.

If you want to tinker around, [requirements.txt](requirements.txt) is provided for ease of use, but you might need additional packages (such as *notebook*), depending on your configuration and editor.

## JavaScript
Since the images are not included in the repository, [getPerks.js](getPerks.js) is included as a way to automatically obtain them. Usage instructions are part of the **Building** section of the main [readme](../README.md).