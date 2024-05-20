# 3D Controller Overlay

3D Controller Overlay is a simple program for content creaters to show what their controller is doing in 3D space without the need for a handcam.

How to Compile:
- Download this repository or clone it using git.

Windows:
- If you want to compile using my batch file you need to have command line access to the gcc/g++ compiler. I use MinGW, you can download it from https://winlibs.com/ extract it somewhere then add the path to the bin folder inside mingw32 (eg. C:\mingw32\bin) to your PATH in Environment Variables. 
- Run windows_build.bat

Linux: (Debian based, Ubuntu, Mint, etc.) 
- If you want to use my install scripts you will need bear (it just updates the compile_commands.json each time it builds)
    sudo apt-get install bear
    Or, you can just take bear out of the install script.

- You will need to install glfw3 and sdl2 (unless you get libraries and link them yourself)
    sudo apt-get install libsdl2-dev
    sudo apt-get install glfw3

- chmod +x linux_build.sh (if needed)

- ./linux_build

Linux: (Arch based, Manjaro, SteamOS etc.)
- All the same things for ubuntu but using Pacman or some package installer like yay or yum. Hopefully if you're using arch based linux you already kinda know what you're doing.

Mac:
- I got this built and running on an Intel based mac without too much headache. I believe I needed brew and maybe a couple other things but it wans't too bad. I don't have access to a mac atm but when I get one again I'll update these instructions.
- Haven't tried any apple silicon macs but if I get my hands on one I'll try it out and update the instructions.

Enjoy!

Larf
