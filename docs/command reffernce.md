This is just a quick command referrence. I am not teaching you here how to use the Neil scripting language (nor how the underlying Lua engine works), it's just a quick referrence.


# Core 
These functions will allow you to get a few things done from the very core of Apollo.

## void Flow.GoToFlow(string flow) 
Can also be called as GoToFlow(flow)
Will switch from one flow to another. Please note, the function calling this will be completed, as well as the entire callback. Only callbacks called later will be affected.

## Property String Flow.CurrentFlow
Contains the current flow. You can also assign a new flow it it... Will have the same effect as "GoToFlow"

## void Flow.Quit(int exitcode=0)
Will end Apollo and leave an exit code if applied.
Notes:
- NEVER use os.exit() provided by Lua for this. Apollo uses drivers and puts data in the memory it has to unload first. os.exit() won't take care of that. Flow.Quit() does.
- Flow.Quit() will finish the callback that it was called in first, and possibly a few more callbacks can be called also. This is a safety pre-caution. Once called the Quit procedure cannot be undone, though.


# FPS
These functions calculate the FPS for you or help you in the FPS calculation. I recommend to use these only for debug purposes.

## Property Number FPS.N
Numberic variable containing the FPS

## Property String FPS.S
Converted into a string

## void FPS.Show(int x,int y)
Will draw the FPS (same way as it's returned by FPS.S) on any spot you want (during a drawing stage).
Used font is a system font.

## Property Int MinTicks
Contains the minimal number of "ticks" (a thousandth of a second) that must minimally pass between two cycles. 
You can change this by assinging a value.
This is particularly a handy setting when dealing with games created for slower computers, but which would run way too fast on fast computers (without MinTicks the computer just handles stuff as fast as it can. You do not always want that).
MinTicks is just a way to make sure your game runs just as fast on all computers.




# Events
These functions are used to check for events triggered by keyboard, mouse etc.

## bool Events.KeyDown(keycode)
Returns true if the key of the specific keycode has been held

## bool Events.KeyHit(keycode)
Returns true if the key of the specific keycode was hit

## bool Events.Terminate()
Returns true if the user tried to close the application




# General Graphics
These are just general graphics routines. Please note, they don't handle images and text. There are seperate classes for that!

## void Graphics.Cls()
Clears screen. Please note, this happens automatically at the start of every cycle allowing to draw. I only put it in whenever you need to step out the normal way of drawing.

## void Graphics.ClsColor(int r,int g, int b)
Sets the color to clear the screen in next time Graphics.Cls() is called.
This also affects the color of a new screen at every new graphics cycle.

## void Graphics.Color(int r,int g,int b)
Sets the drawing color.

## (int,int,int) Graphics.GetColor()
Gets the drawing color. Please note, this function returns 3 values in one go.

## int Graphics.Alpha([ int alpha ])
Sets and gets the alpha value.
~~~C
Graphics.Alpha(100) // Sets Alpha to value 100
int A=Graphics.Alpha() // Gets Alpha and assigns it to variable A
~~~
Well that's the idea

## void Graphics.Flip()
Shows all stuff drawn during a drawing stage. (At the end of a drawing callback, this function is called automatically).

## void Graphics.Rect(int x, int y, int w, int h, bool open=false)
Draws a rectangle.
The "open" parameter will if set true draw a line beased rectangle. Otherwise a filled one.

## void Graphics.Line(int x1, int y1, int x2, int y2)
Draws a line

## property int ScreenWidth
Contains the width of the screen (or in case of a windowed game, the window)

## property int ScreenHeight
Contains the height of the screen (or in case of a windowed game, the window)



# Class Image
The class image handles loading and drawing images. It has the following members:

## static Image Load(String File, [String Tag])

Loads an image. The tag system can be used to give an image a tag. Tagged images will not be auto-release when the class object is destroyed, and can be used over multiple state without taxing the RAM too much.

## static void KillTag(String Tag)

Kills the image tied to a tag. Please note this will make all objects using this tag unuable. So use with care.

## void Kill()

Kills the image tied to an object. Normally you don't have to do this, as the garbage collector will do this automatically unless an image is tagged. However using this command on a tagged image will affect all other images with the same tag, so be careful. Don't use if you don't know what you are doing!

## void Draw(int x, int y, int frame=0)

Draws an image.

## void Hot(int x,int y)

Sets the "hotspot" of an image. Now names differ per engine for this, as some call it insertion point and others call it handle, an well, I've more names of this. It's basically the point from which the image is drawn.

## void HotCenter()

Sets the "hotspot" to the center of the image

## void HotBottom()

Sets the "hotspot" to the center bottom point of the image (for sprites depicting persons this is a good spot as the spot is then between their feet (when the size of the sprite is well-set that is), which is a very logical way to draw from).




# Class Audio
This class handles audio

## static Audio Load(String File,[String Tag])
Loads Audio

## int Play([int channel],[int/bool loops])
Plays audio. Returns the channel used. If channel is set it will always use that channel otherwise the first free channel found.
If "loops" is set to true or -1, sound will loop infinitely. If it is an integer value is set it will loop the number of times given.

## static void Stop([int channel])
Stops a channel from playing. When no channel is given all channels will be stopped.



# Group Kthura

Now using Kthura is a bit of thing I need to cover in a separate document, due to Kthura being a bit of a complex system. Allthough I know (from experience) that Kthura completely written in script can work, and pretty fast to, I now decided to keep Kthura wholly in C++ (for more speed), but linking a system like that to a multi-state scripting engine was nasty.

