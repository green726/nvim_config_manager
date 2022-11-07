# nvim_config_manager
200 lines of C to manage multiple Neovim configurations

# Installing
Download the latest binary release from the releases page
Alternatively, you can clone this repository and run "meson build" (Assuming you have meson installed), cd into the build directory, run ninja, and then run the output executable. 

# Running
Run the exe with "help" passed to list the commands. 

# Information
The program quite simply links a number to your neovim config init.lua (or vimrc) paths allowing you to easily manage multiple configurations. 
It creates a .txt file in your neovim config directory that contains all of your added configuration.

# WHY?
I was sick and bored. Result is dumb C code. 
