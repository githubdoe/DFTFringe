### Live View using Camera or video stream
___
**Live view** can show videos from a URL or a USB camera.


When in loop mode it samples the video stream and process each sample creating a temporary wave front file.  The RMS of that file is examined and if lower than the RMS threshold you set it is added to an average.  It displays that average back on the normal 3D surface of DFTFringe.  If that RMS is greater than the threshold then it display that frame instead of the average so you can see what might be wrong with the setup.  When ever the current RMS value drops below that threshold it will then display the average wavefront.  When you stop the loop it will save the average wave front in the wave front list.   This can quickly remove variations due to air currents and random vibrations.  

Depending on you computer and camera the sample and analyze rate can be as fast as one per second or every few seconds.

###
### Connecting to a video feed or camera

Live view can show videos from a URL or a USB camera.
USB cameras are selected on a windows machine by their ID number.

**Settings tab** (Lower right) lets you select the source of the video. Use 0, 1, or 2 for USB attached cameras.  

A selection for a URL stream might look like this:  'http://192.168.50.5:5000/video_feed' 
###
**Connection problem**
___
Some cameras take several blank frames to start with.  The code things that so many blank frames indicate a connection problem.  The default is 15.  You can set how many blank frames to allow to get your camera connected in the settings.

When a connection was working but drops for some reason you will get a status message stating that.  You must go to settings and click on the device once again to get it to start.


###
### Live video view  
The Live Video view (left side of the controls) shows the cameras view. In it you will see the interferogram and if enabled the DFT of that igram.
###
### Automated Live Analysis

Before starting the automated analysis loop, ensure the following steps are completed:

1. Press the **Grab igram** button To import the igram into DFTFringe and outline it as usual. 

2. Press the **Auto outline** button or outline the igram yourself.   The analysis loop will then use that outline for all the rest of the samples until you change it.

2. Press the **Outline OK** button to then see the DFT and adjust the blue filter if necessary.

3. Now you are ready to start the analysis of each video frame that it can grab.  Press the **Start Loop** button.  

It will loop taking an image, analyze it, compute its RMS and display the RMS and average RMS and the 3D surface plot will update in the standard DFTFringe 3D display. The zernike values will display the current sample's Values and the contour plot will also display the current sample's values.  The 3D plot will display the Average of all the samples at or below the Max RMS value.

###
### Max RMS
___
**Max RMS value** You can set the Max RMS value where values higher than that will not be used in the analysis.

###
### Auto RMS Setup

Enable the checkbox if you want the Max RMS value to be set to the value of the first analyzed wavefront times a percentage. This will happen the first time you **Start** the analysis. From then on the Max value will not be modified by the program. You can still modify it yourself however.

###

If a wavefront's RMS is equal or below the max RMS value it will added to the average and the average will be displayed in the 3D view.  Otherwise that wave front will be displayed in the 3D view and a status field will turn red.  

Once the looping has started you might want to pause it to adjust some settings without resetting the averaging.

The **Start** button always resets the averaging if it was selected to be done.

The **Stop** button always stops the current looping and any averaging happening.

The average is saved  you press Stop or press the "Save Average" button.
###
### Trend View
___
The trend view below the live view show a graph of two values computed from each analysis over time.
* RMS - shows the average RMS value.  It scale is on the left side.

* Best Fit Conic or SA - Show either of those values as a running average of the last 5 samples.  It's scale is on the left side of the graph.  You can select what it shows in the settings.

* Split bar - the Trend view has a border line between it and the live view that can be moved to change the size of the live view and Trend view.  You can drag that bar all the way down to show only the Live view or all the way up to show only the Trend graph.

###
### Controls
___
The panel on the right has several controls.
* Video zoom factor - usually fit to window is best
* DFT Size - Usually **512** is best but if your fringes are very close together and you have a large image then **1024 x 1024** is better.  The larger the DFT the slower the video is sampled. 
* DFT Transparency - values from 0 to 255.  99 seems to be a good value.  Sets how visible the igram is under the DFT.
* DFT Contrast - changes the colors of the DFT.  .7 seems to be a good value.

###
### More Settings
___

Other settings values in the settings menu.
* Camera Resolution - you can change some camera's resolution.  If the camera does not accept this or one of the values it is ignored.  
* Do not add wave front to wave front list - If not set it Will save each analyzed wave front while in the analysis loop to the wave front list.
* Show Best fit conic in trend display - if not set it will display the SA value after the null is removed.


