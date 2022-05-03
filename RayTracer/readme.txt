I've implemented everything; ie. all of the test cases produce the same results as the keys for me. 

How to compile:
Open the RayTracer.sln file and Build with visual studio. I was using Visual Studio 2019 for this project. In the configuration manager, I have it set to Release, so building it as such will build as release, not debug. If debug is wanted, swap the active solution config.

I have all of the test files in the Debug folder. The project is configured so that if you set to Release and run Debug -> Start Debugging, it will run RayTracer.exe with all of the test files. (I implemented it so that you can throw in as many text files as you want and it will loop through generating all the images)

You'll also notice that glm is included for vector and matrix functionality.
 
