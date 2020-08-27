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

