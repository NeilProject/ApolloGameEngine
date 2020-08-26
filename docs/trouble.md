# Houston! We have a problem!

This document is about the most common trouble you can get when you use the Apollo Game Engine or any game created with it.



## Error screen

![Houston! We have a problem!](https://user-images.githubusercontent.com/11202073/91335441-b2f78080-e7d0-11ea-893a-645c1922992f.png)

Under normal circumstaces the screen as displayed above will show.
Please note that the error above was only possible due to the early state of development I was in at the time, but it can be used to demonstrate:
- The cyan letters will mostly always say "Houston! We have a problem!" or another nice header in future versions. 
- In yellow you see the actual error message
- The text shown in white above was nonsensical text used to test if it worked. If a traceback in the scripting is possible, that will be shown in stead.
- Apollo is made to work in multiple states, which can communicate with each other. In magenta the name of the state were the error occured in will be shown. If the state name is "C++" or is prefixed with "C++" then the error was caused by code in Apollo itself (Apollo was written in C++, hence the name of that "state"), and that could imply we're dealing with an internal error which should be reported (unless somebody else already did so). Although it is possible the scripts were so strange that Apollo couldn't handle stuff, never be 100% sure about that.


If you made a game with Apollo yourself, take good note of this information, as it's all the info Apollo can provide to get you closer to solving the issue.
