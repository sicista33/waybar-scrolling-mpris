![](/image/waybar-scrolling-mpris.gif#center)

**Waybar-scrolling-mpris** displays the current media information on Waybar with scrolling for long text.  



<br>

## Dependencies
* [playerctl](https://github.com/altdesktop/playerctl)
* [JSON-c](https://github.com/json-c/json-c)
* CMAKE

`CMAKE` and `JSON-c` is needed for build. If you download the binary file(**build/waybar-scrolling-mpris**), you won't need it.

<br>

## installation
Clone this project first
```sh
$ git clone https://github.com/sicista33/waybar-scrolling-mpris.git
$ cd ./waybar-scrolling-mpris/build
```

Build with CMAKE (If you have downloaded the binary file, You can skip this steps)
```sh
$ cmake ..
$ make
```

Move binary to your Waybar script directory  
e.g) If you use **Hyprland**
```sh
$ mv waybar-scrolling-mpris ~/.config/hypr/UserScripts/waybar-scrolling-mpris
```

<br>

## Config
**waybar-scrolling-mpris** support `display-format`, `length` and `icons` options.

|option|default|description|
|:------|:-------|:-----------|
|display-format| {artist} - {title}| Specify the text format to be displayed on Waybar.<br>`{icon}` `{artist}` `{album}` `{title}` `{player}` `{status}`<br>**`{icon}` has a fixed position. If you use this option, it must always be placed at the front.**, 
|length|40|Maximum length of text to display. [5 - 511]|
|icons|-|Specify  the icon representing the current media player's status.<br>**Pay attention to the case of the status string.**|

<br>

You can pass the path to the module config file as a command-line argument. e.g)
```
"exec": "~/.config/hypr/UserScripts/waybar-scrolling-mpris ~/.config/waybar/modules"
```
If no argument is passed, it will search for the default config file in valid [Waybar configuration directories](https://github.com/Alexays/Waybar/wiki/Configuration)(except for `SYSCONFDIR`).


*If you want use without config parsing, use [old version](https://github.com/sicista33/waybar-scrolling-mpris/tree/old)*

<br>

## Usage
Define custom module in your waybar config.
e.g) `~/.config/waybar/modules`
```
...
"custom/waybar-scrolling-mpris": {
    "format": "{}",
    "display-format": "{icon}  {artist} - {title}",
    "exec": "~/.config/hypr/UserScripts/waybar-scrolling-mpris",
    "icons": {
        "Paused": "⏸",
        "Playing": "▶",
        "Stopped": "■",
    },
    "length": 40,
},
```

<br>

Edit your Waybar config to add module
```
"modules-left": [
    ...,
    "custom/waybar-scrolling-mpris",
],
```

<br>

And restart Waybar e.g) If you use **Hyprland**
```sh
pkill waybar && hyprctl dispatch exec waybar
```
