
[![Logo]({{{rel_path}}}images/logo.png)](index.html)

---

##About

snow is a light, pure and clean toolkit for building frameworks, applications and games using [haxe](http://haxe.org), targetting native platforms and browser.
This means snow by default runs natively on **Mac**, **Windows**, **Linux** desktops, as well as **iOS** and **Android** devices. It also runs directly in the **Web** browser using webgl.

&nbsp;_how?_

&nbsp;

[ <img src="{{{rel_path}}}images/haxe.png" target="_blank" class="small-image"/> ](http://haxe.org)   

Haxe is an expressive, beautiful modern programming language that compiles it's own code into other languages. 
This allows you, a user of haxe and thus snow - to build your code into the target platform code directly.

For example native platforms would use c++, without you ever needing to write c++ code.
Haxe exports to optimized javascript, so when you target a browser - it will run there too.

<div class="breakout">

[Get Started](#guide)
[More Details](#details)
[View Samples](#samples)

</div>

---

##Design

snow embraces a minimal, straight arrow approach by exposing a variety of tools at your disposal,
with as much consistency as possible across targets. This allows you to focus on yout code and not the platform code.

snow is built as a facilitator only - it is focused on portability, clarity, consistency and maintainability.
This allows you to build frameworks, games and applications without it getting in the way.

snow is also as self contained as possible.
It is built with no external dependencies, except for a build pipeline called [flow](#), which makes the application.

<div class="guide">
<a name="guide"> &nbsp;</a>
<h2>Guide</h2>

**Starter guides**<br/>
<br/>
[Setup]({{{rel_path}}}guide/Setup.html) <br/>
[one - start here]({{{rel_path}}}guide/one.html) <br/>
[two - life of a snow app]({{{rel_path}}}guide/two.html) <br/>
[three - dealing with assets]({{{rel_path}}}guide/three.html) <br/>
[four - interactivity]({{{rel_path}}}guide/four.html) <br/>

<br/>
**In depth guides** <br/>
<br/>
[Windowing]({{{rel_path}}}guide/windowing.html) | [Audio]({{{rel_path}}}guide/audio.html) | [Assets]({{{rel_path}}}guide/assets.html) | [Input]({{{rel_path}}}guide/input.html) | [rendering]({{{rel_path}}}guide/render.html) | [IO]({{{rel_path}}}guide/io.html) | [Utils]({{{rel_path}}}guide/utils.html)
<br/>
<br/>
**API docs**<br/>
<br/>
[API reference]({{{rel_path}}}api/index.html)

</div>

---

<a name="details"> &nbsp;</a>
<h2>Details</h2>

###Structure

snow consists of three main layers :

- #### API layer
- #### adapter layer
- #### support layer

**API**

This is the user facing API that you as a snow user interact with.
The API is where you access everything from that you would use in your app.

It is written in haxe code, and resides in the `snow/` folder.

**adapter layer**

This is the code that adapts the API to the platform you are building for.

It is also written in haxe code, and resides in the `snow/` folder.
On native targets, this includes code to load functions from snow.ndll.
On the web target, this includes code that interacts with the browser.


**support layer**

This is the low level platform specific code that interacts with the target.

On **web**, this is written in haxe code, and resides in the `snow` folder (mostly in the adapter).

On **native**, this is written in c++ - and is built into and loaded from `snow.ndll`.
It resides in `project/src` and is compiled using `hxcpp`

---

<img src="{{{rel_path}}}images/structure.png" class="center"/>

---


<a name="samples"> &nbsp;</a>
<h2>Samples</h2>

