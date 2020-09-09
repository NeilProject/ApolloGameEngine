# I'd like to use your Apollo Engine, but I don't want to use Neil. I'd just like to use pure Lua in stead. Is that possible?

Yes, only Main.Neil must be used for the start up, but after that you can just switch to pure Lua, and Neil was designed to make this possible, and if this goes wrong let me know as then we got some bugs.



Just put these lines to your Main.Neil script if you wanna avoid Neil as far as possibe:

~~~
Init
	Flow.Load("MYLUASTATE","LuaMainScript.lua")
	Flow.Current="MYLUASTATE"
End
~~~

As soon as a state is loaded with the main script suffixed with ".lua" Apollo will know it's a Lua state.


# A few notes about this though.

For the Apollo callback functions it shouldn't matter. 
~~~lua
function Apollo_Flow()
 ....
end
~~~
In Lua should do the same as 
~~~
void Apollo_Flow()
   ....
end
~~~
in Neil... There are a few rubs though. The core features in the command referrence are written in Neil and their modules have been implemented as Neil Globals. You can still call them all by prefixing though with "Neil.Globals."

So this in Neil script (which would draw a blue line from the upper left to the bottom right)
~~~
void Apollo_Flow()
  Graphics.Color(180,0,255)
  Graphics.Line(0,0,Screen.W,Screen.H)
end
~~~
Would become this in Lua
~~~lua
function Apollo_Flow()
  Neil.Globals.Graphics.Color(180,0,255)
  Neil.Globals.Graphics.Line(0,0,Screen.W,Screen.H)
end
~~~

Of course if you don't want that, this is perfectly fine:
~~~lua
local Graphics = Neil.Globals.Graphics
function Apollo_Flow()
  Graphics.Color(180,0,255)
  Graphics.Line(0,0,Screen.W,Screen.H)
end
~~~


# Library calls

The "require" function of Lua is only useful if the scripts or libraries it calls are made in the traditional Lua way, so either stored as pure files on the disk or calling to libraries that you may have merged inside Apollo itself (and compiled it yourself). For retrieving files from JCR6 packages, it's useless.
You should use "Neil.Use" in stead. Please note, it's a part of Neil, but it can handle Lua and Neil all the same, and for your scripts it won't (if the module has been written properly) even matter if it was written in Neil or Lua.

~~~
local exMod = Neil.Use("Modules/ExternalModule")
exMod.Hello()
~~~
Please note, unlike require you should not use "." for directory separation, but just slashes (no backslashes, not even in Windows). In the Use command the extention should not be present or things will be messed up.
Now Neil will first try out if a Neil script with that name exists, if it can't find that it will try to look for a Lua script. Then it will just compile it and return the result.

Please note, every module is only loaded and compiled ONCE per state (unless you destroy that state and create it anew, but strictly speaking you got a different state then). This was done to save time and to make sure no conflicts with global variable definitions could arise. This also makes all locals (set in the ground scope) always be in the same instance.
That means that this program:
~~~
local e1,e2
e1 = Neil.Use("Module")
e2 = Neil.Use("Module")
print(e1==e2)
~~~
Will return true, as both e1 and e2 will have the same referrence (in normal usage).



### Do I need to prefix stuff from a module written in Neil with Neil.Globals?
That depends on how the module is written. If the module contains this, then yes:
~~~
global void Hello()
    Print("Hello World!")
end
~~~
This because the module generally doesn't return stuff through "Neil.Use"  then. If the module is written like this, then no:
~~~VB
module
    void Hello()
       Print("Hello World")
    end
end
~~~
Then your program can just be like this:
~~~Lua
abc = Neil.Use("Hello")
abc.Hello()
~~~


### Now I have a module written in Lua. Can it be used by somebody who prefers Neil?

Yes, no problem. Here too count a few rules though based on how the module was made.
~~~lua
local MyLuaModule = {}
function MyLuaModule.Hello()
    print("Hello World")
end
return MyLuaModule
~~~

This in particular will be easy for Neil to tackle:
~~~vb
#use "MyLuaModule"
init
   MyLuaModule.Hello()
end
~~~
The #use directive declares MyLuaModule and loads the file "MyLuaModule.lua" 

If the module looks like this:
~~~lua
function Hello()
    print("Hello World!")
end
~~~
Then things get trickier since no value was returned, but Neil can handle that fine, and there are two methods to make it happen.

method 1:
~~~vb
#use "MyLuaModule"
init
    Lua.Hello()
end
~~~
The "Lua" object just contains all Lua globals, so that will do fine. Alternately you can also do this:
~~~vb
#use "MyLuaModule"
import Hello
init
   Hello()
end
~~~

Please note that "import" makes the identifier "Hello" just translate to "Hello" is when you assign something else to it, it will just do the same as what would happen in pure Lua. Neil offers no protection to this like it does to its own identifiers. 


## Using "."  or ":" when working with pure Lua stuff in Neil or vice versa.

Neil classes and groups and modules always require just a . even when you call them in pure Lua. Lua objects are bound to the rules Lua set up for them, so if you create an object in Lua, then even in Neil you should use . or : accordingly in the same manner as would have been required in Lua.


## Speaking of classes. Lua doesn't have the "new" keyword. Is that a problem?

No. Neil only uses the "new" keyword to prevent confusion with a classname being used as a function call or as type name. Fact is that Neil classes can just be called as functions from Lua.
~~~vb
class MyClass
     int a
     int b
     int c
     constructor(int aa,int ab,int ac)
         a = aa
         b = ab
         c = ac
     end
     static void Hi()
        print("Hi there!")
     end
end
~~~

Now when you want to create a new object in Lua, just do this:
~~~lua
local myobject = Neil.Globals.MyClass(1,2,3)
print(myobject.a,myobject.b,myobject.c)
Neil.Globals.MyClass.Hi()
-- That all works!
~~~

Within the class the Neil rules still apply though, so the parameters the constructor takes must still be integers. However, Neil is unlike Lua case insensitive, so in the example above "myobject.A"  and "myobject.a" would refer to the same object member. Of course "myobject" iself is a Lua identifier so still bound to the Lua rules and thus "myobject" will not be the same as "MYOBJECT".
And here goes too that this is completely legal:
~~~lua
local MyClass = Neil.Globals.MyClass
local myobject = MyClass(1,2,3)
print(myobject.a,myobject.b,myobject.c)
MyClass.Hi()
-- That all works!
~~~


# One final note

Macros defined in Neil will NOT affect .lua files in anyway and cannoy be called from Lua files either. This is because Macros are no identifiers or anything, but just stuff Neil uses for its own translations. All macros are first substituted with their respective defintions, and then the new script will translate. Since Lua doesn't have a similar step and Neil leaving Lua files the way they are, nothing will happen if imported Neil scripts contain macros.  Constant and readonly variables will just work the way they should.

