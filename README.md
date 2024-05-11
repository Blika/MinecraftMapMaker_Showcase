# MinecraftMapMaker_Showcase
A showcase of minecraft map image generator that renders a map image based on the data given.

# What does it do?

Basically, it gives you an image as an output. All you need is to provide a block height map (currently no tools to do it, as it's just a showcase) and tweak settings. This program will use all of your computer cores during rendering to speed up the process.

There are multiple settings (located in input/settings.json):

* Block height map and color map which are essential for this to work. This showcase contains an example so you can tweak it and get different map images.
* Darkening coefficient that, obviously, randomly darkens pixel so the output image looks a bit better.

> No darkening.

![](https://github.com/Blika/MinecraftMapMaker_Showcase/assets/61899272/891cc169-966d-4144-91f1-bfd392b1ec71)

> Darkening set at 0.1.

![](https://github.com/Blika/MinecraftMapMaker_Showcase/assets/61899272/1cdb5f9e-44c0-4a0d-a54a-75be3c1260e5)

* Shading coeffiecent that further darkens pixels when shadows or outlines are processed.
* Light direction which decides from there the light is processed. (Essential for outlines and shadows to work)
* Light radius. You can think that "light" in this project is a sun, and this settings desides how big this sun is. Without it the shadows would be somewhat unnatural (very long and tall). (Essential for shadows to work)
* Light offset that decides where the light is actually located. If we only provide a light direction the program won't know exactly how to render light so we would have to assume it (meaning it would be hardcoded). (Essential for shadows to work)
* Drawing outlines. It renders outlines based on light direction. As a result, it adds volume to the output map.

> Darkening 0.1, outlines enabled. The result looks so much better now.

![](https://github.com/Blika/MinecraftMapMaker_Showcase/assets/61899272/6228078e-ef1a-401a-9a35-b8652e83a18b)

* Drawing shadows decides if shadows are rendered. It heavily depends on light settings for obvious reasons and by tweaking this you're going to get different outputs. Sadly, as we only provide a height map, so we assume that there isn't any empty space under blocks. Therefore, some shadows may look strange but still quite good if your light settings are okay.

> Darkening 0.1, outlines enabled, shadows enabled. Based on shadows you can guess each object actual height. Trees are quite tall so from here their shadows may not look good but buildings' shadows are okay.

![](https://github.com/Blika/MinecraftMapMaker_Showcase/assets/61899272/ae50f1c8-356c-4765-8e6c-0a8529a70c91)

# Usage

To use this tool you can either download it from releases or build it yourself using cmake. Currently only Windows release is available.
