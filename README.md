# This is the third and final of the total three assignments for the [Systems Programming Course](http://cgi.di.uoa.gr/~antoulas/k24/) during my B.Sc in Informatics and Telecommunications. It is based on my [first](https://github.com/john-fotis/SysPro1) and [second](https://github.com/john-fotis/SysPro2) assignment.

## Short Description:
The purpose of this project, is to convert the client-server application of the previous project, from multi-processing to multi-threading server. The model is an inverted client-server approach, which means, there is a single *travel monitor client*, which generates multiple *monitor servers*. These are responsible to answer all the user travel requests made to the *travel-client*. The main goal here is to enhance the previous model, while practicing on dynamic use of [fork-exec](https://en.wikipedia.org/wiki/Fork%E2%80%93exec) on the parent side. Regarding the child-servers, those are modified in order to read all their input files, not one-by-one anymore, but with the help of a pool of [POSIX threads](https://en.wikipedia.org/wiki/POSIX_Threads). The communication is also altered to [TCP sockets](https://en.wikipedia.org/wiki/Network_socket).

## Requirements:
- Ubuntu-based linux distribution (Developed in Ubuntu 20.04 LTS)
- GNU Make 4.2.1
- GCC 7.5.0 or newer
- Bash Shell

## Details & How to run:
You can run everything, in the root path of the project, using the [Makefile](https://github.com/john-fotis/SysPro3/blob/main/Makefile). Type `make help` for all the available options. You can also run both the script and application with the appropriate make rules. All the run parameters can be changed through the [Makefile](https://github.com/john-fotis/SysPro3/blob/main/Makefile).

## Bash-Script: <br/>
  You first need to produce an `input_dir` directory which contains subdirectories named after countries. Each country has a number of input record-files for the main application. For this purpose you need the [citizenRecordsFile](https://github.com/john-fotis/SysPro3/blob/main/citizenRecordsFile) <br/>
  
  To do this you have two options:
  1) `make scriptRun` or
  2) `./create_infiles.sh [citizenRecordsFile] [input_dir] [numFilesPerDirectory]`

  **Notes:**
  **citizenRecordsFile** is case-sensitive, however, it can be modified in the [create_infiles.sh](https://github.com/john-fotis/SysPro3/blob/main/create_infiles.sh). `input_dir` is the target directory with the final input files for the main application. Number of files per directory is at least 1.

## Application: <br/>
  The application reads all the input files in the existing *input_dir*. The server is responsible to spread the workload - based on countries - **evenly** among the available monitors. Each monitor initiates its database after reading its assigned record files. The pool of threads is responsible for this task, simulating the [producer-consumers problem](https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem), while reading the files from a shared [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer) of files. The main monitor acts as the producer in this case, filling the buffer with input files until there are none left to be read. The other threads consume these files and store the valid information into the main database of the monitor-server. Afterwards, the monitor is ready to accept a client connection. If this step takes more than a certain **TIME_OUT** value, the execution is aborted. Time out is set by default to 10s and can be changed on the execution command. If everything is fine during the initiation step, the travel client connects to all the monitor servers and receives a portion of data from them. This data works as cache, and is used to answer user-requests fast, using only travel-client resources. In case the request can't be answered with the help of this cache, then it is promoted to some or all the available monitor-servers. The UI is sollely achieved through the travel-client, via a terminal-menu. You can type `/help` to get all the available commands when using the application. <br/>

  The above can be tested by compiling with `make` or `make all`. Then, run the application with:
  1) `make run` or
  2) `./travelMonitorClient -m [numMonitors] -b [socketBufferSize] -c [cyclicBufferSize] -s [sizeOfBloom] -i [input_dir] -t [numThreads] (-o [timeOutSeconds])`
  
  **Notes:**
  - All arguments are mandatory, case and order sensitive.
  - **socketBufferSize** is the number of bytes used in the custom communication protocol in the pipes.
  - **cyclicBufferSize** is the number of files which can be stored simultaneously into the shared circular buffer.
  - Minimum **sizeOfBloom** is 1, optimal 1000 (for the current input file) and maximum tested is 100K bytes.
  - **input_dir** must be the same string as the argument given to the [create_infiles.sh](https://github.com/john-fotis/SysPro3/blob/main/create_infiles.sh).
  - You can also run with `make valgrind` rule. The application has been tested for multiple leak types. In this case, mind giving the extra -o argument with a number which is big enough to avoid execution-aborts due to low **TIME_OUT** times.
  - The execution might abort on high waiting times during the initial step, because of hardware restrictions or the use of Valgrind. These issues can be resolved by increasing the time-out value as described in the above step. This, however, will not help in network issues.
 
## Copyright and License: <br/>
&copy; 2021 John Fotis <br/>
This project is licensed under the [MIT License](https://github.com/john-fotis/SysPro3/blob/main/LICENSE.md)
