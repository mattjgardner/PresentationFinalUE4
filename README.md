# PresentationFinalUE4
Presentations in Unreal Engine

A software for presenting ideas through presentations and videos and virtual collaborative design and testing using PhysX.
Modifications have been made to the UE4 PhysX source code as outlined in 'Personal Documentation'

How to use this project:

There are separate read me’s for the python scripts so be sure to read them first before using this project. These can be found on my GitHub at https://github.com/mattjgardner

To summarise: you will use the python to create images from a PowerPoint and sound files and import those in to unreal engine. The python plugin must be enabled for this to work. If it’s not already enabled: click edit -> plugins then find the python and enable it.

Important notes for unreal engine:
-This is made in UE 4.26.0 downloaded from GitHub source
-I recommend using source as it downloads debug symbols making it easier when you make a mistake
-IMPORTANT: You can only compile server builds using source. This will be necessary for amazon gamelift server builds.
-IMPORTANT: KantanCharts plugin will only work if you have it installed to the engine. Copy the plugin folder to UnrealEngineFolder/Engine/Plugins/Marketplace/
-The code will work in 4.26.0 but there’s no guarantees it will work in later (or earlier versions)
-To try in other versions, download project, delete the sln file (or rename to old.sln or something) then right click the .uproject and click regenerate solution files. Open the file in visual studio and rebuild.
-Other errors for this can sometimes be solved by deleting the intermediate and binaries folders then recompiling.



To test the project:
-Double click the uproject file to open in unreal engine
-Click the drop down menu next to the play button and click play as client
-Navigate the world and interact however you wish to make sure things are working
-The Oculus avatar needs to be overlapping the slide actor bound box so make sure that works
-Sound files may need to be added to the avatar manually if they’re not added in the constructor


Some other useful notes:
-If you’re going to hardcore a path in c++, wrap the code with #if WITH_EDITOR and #endif it might be worth considering declaring the UPROPERTY as BlueprintReadWrite and setting the path/file in the editor to stop packaging errors
-To add folders to your code to keep it organised, the build.cs file must be modified as this one is by specifying the path and using publicaddmodule
-CUDA integration will be in a seperate example project and a tutorial on how to do it created

Compiling/packaging the project:
Server:
-To compile a server version in visual studio, to the left of the green play and debug button is a drop down which you should change to “Development Server” then click compile (this requires a server target.cs file like the project)
-In the editor change the packaging settings to Win64 and the target to ProjectNameServer
-In the packaging settings you must add the maps to package as well as the server default map
-Package and a folder will be created called WindowsServer in the specified directory

Client/WindowsNoEditor:
-In the packaging settings you must add maps to the client package settings as well as the client default map
-Change the target to ProjectName
-Package and a folder will be created called WindowsNoEditor in the specified directory

To test these it’s advisable to test on GameLiftLocal Server (a separate read me in the personal documentation is created for this)
To launch a GameLift server documentation is also included in the personal documentation folder (Images to guide you are also included) to get the IP address for the server is included in this documentation

Any presentation can be used in this project just be sure to follow the steps above.


PhysXVehicle information:
-Any mesh can be used as the car mesh such as CAD models but beware number of triangles/polys as it could effect performance
-Press e to start controlling the vehicle to see how it performs
-change suspension settings at run time
-change torque curve in editor
-change change wheel size at run time (won’t change model, just the invisible wheel components)
-change mesh in editor

To use the package:
-users download WindowsNoEditor (as a zip file)
-WindowsServer uploaded to Amazon GameLift
-Host gets IP using AWS CLI
-Clients join by IP
-Client and Host explore the world and watch the presentation as a group and try out the PhysXVehicle etc.

Most of the code should be commented/self explanatory but if there are any questions feel free to message me on:

Discord:
Matty#8579

Email:
matty.g.1997.mg@gmail.com
