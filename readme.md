![](/image/waybar-scrolling-mpris.gif)

**Waybar-scrolling-mpris** displays the current media information on Waybar with scrolling for long text.  


## Dependencies
* [playerctl](https://github.com/altdesktop/playerctl)
* gcc

GCC is needed for compilation. If you download the binary file(**waybar-scrolling-mpris**), you won't need it.

## installation
```sh
$ git clone https://github.com/sicista33/waybar-scrolling-mpris.git
$ cd ./waybar-scrolling-mpris
$ gcc ./waybar-scrolling-mpris.c -o waybar-scrolling-mpris
```

If you have downloaded the binary file, You can skip the above steps.  

Move binary to your Waybar script directory  
e.g) If you use **Hyprland**
```sh
$ mv waybar-scrolling-mpris ~/.config/hypr/UserScripts/waybar-scrolling-mpris
```

## Usage
```sh
./waybar-scrolling-mpris [max-width]
```


## How to add Waybar
Define custom module in `~/.config/waybar/modules`
```
...
"custom/waybar-scrolling-mpris": {
    "format": "{}",
    "exec": "~/.config/hypr/UserScripts/waybar-scrolling-mpris 40,
},
```
`exec` filed is the path where you moved binary and maximum length of text to display.  

Edit Waybar config `~/.config/waybar/config`
```
"modules-left": [
    ...,
    "custom/waybar-scrolling-mpris",
],
```

And restart Waybar
```sh
pkill waybar && hyprctl dispatch exe waybar
```

## ⚠️ NOTICE
After installing **waybar-scrolling-mpris**, you need to manually kill zombie process when restarting Waybar. as it **does not** handle this automatically.