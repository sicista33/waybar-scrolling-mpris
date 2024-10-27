![](/image/waybar-scrolling-mpris.gif)

**Waybar-scrolling-mpris** displays the current media information on Waybar with scrolling for long text.  


[Simple version here](https://github.com/sicista33/waybar-scrolling-mpris/tree/old)


## Dependencies
* [playerctl](https://github.com/altdesktop/playerctl)
* [JSON-c](https://github.com/json-c/json-c)
* CMAKE

CMAKE is needed for build. If you download the binary file(**waybar-scrolling-mpris**), you won't need it.

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

## Usage
Define custom module in `~/.config/waybar/modules`
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
`waybar-scrolling-mpris` support `display-format`, `length` and `icons` options.

|option|default|description|
|:------|:-------|:-----------|
|display-format| {artist} - {title}| Specify the text format to be displayed on Waybar.<br>`{icon}` `{artist}` `{album}` `{title}` `{player}` `{status}`<br>**`{icon}` has a fixed position. If you use this option, it must always be placed at the front.**, 
|length|40|Maximum length of text to display. [5 - 511]|
|icons|-|Specify  the icon representing the current media player's status.<br>**Pay attention to the case of the status string.**|

<br>

You can pass the path to the module config file as a command-line argument.
```
"exec": "~/.config/hypr/UserScripts/waybar-scrolling-mpris [module config file path]"
```
If no argument is passed, the default path(`~/.config/waybar/modules`) will be used.


Edit Waybar config `~/.config/waybar/config`
```
"modules-left": [
    ...,
    "custom/waybar-scrolling-mpris",
],
```

And restart Waybar
```sh
pkill waybar && hyprctl dispatch exec waybar
```
