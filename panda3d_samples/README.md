# Panda3D Samples

These samples are ported from samples in Panda3D (https://github.com/panda3d/panda3d).

## Looking and Gripping
This tutorial will cover exposing joints and manipulating them.Specifically,
we will take control of the neck joint of a humanoid character and rotate that
joint to always face the mouse cursor. This will in turn make the head of the
character "look" at the mouse cursor. We will also expose the hand joint and
use it as a point to "attach" objects that the character can hold. By
parenting an object to a hand joint, the object will stay in the character's
hand even if the hand is moving through an animation.

![panda3d-looking-and-gripping](https://user-images.githubusercontent.com/937305/32311439-06a40b10-bfdc-11e7-82cd-52aa92c440ef.gif)
