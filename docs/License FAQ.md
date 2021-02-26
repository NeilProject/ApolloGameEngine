In the basics Apollo is licensed under the GPL 3 license, with all the freedom and restrictions the license provides. 
However in this document I want to make sure anything that is unclear is cleared up.


# I am free to make anything I want with Apollo

Well, anything the law allows, I suppose. Yes.

# Am I allowed to make a game people have to buy.

Yes!

# That doesn't require me to make the source code open?

- If you only use a pre-compiled version obtained from one of the official sources, you don't have to many anything public, since only the source code of Apollo has to be made public. The content of your JCR files is considered "the data", and that includes your Neil/Lua scripts with with them calling out to underlying APIs being GPL licensed.
- If you however modify Apollo itself, then you must release the modified source code of Apollo as required in the GPL3 license. This will then only apply to the source code of Apollo itself. Not of your scripts. 
- One rub though, if you used 3rd party scripts written in Neil/Lua in your project, that is GPL licensed, then the license of those scripts will require you to set the game fully in the open-source.

# Am I required to put you in the credits of my game?

Well, I think it's good manners to do that anyway, regardless if it's required, eh?
Now I tried to look things up here (I wanted to make sure of my case before applying the license after all), but as your game is "the data" the GPL requirements may not affect it, but here's the rub... Since you'll need to supply the Apollo engine with it (or else it won't work after all), the GPL3 license will require that a document with copyright information about the engine is always supplied.

