These files allow running an application with the top-level run.sh script and allows addition of different setups.
In addition to the top-level run.sh, there are several scripts in the startscripts subfolder:
  start.sh					system-independant configuration and actual starting of the application
  start_SYSTEM.sh			system-specific parameters for the application, should call start.sh
  kill_SYSTEM.sh			generic script to kill all applications on the cluster of the respective system
  master_SYSTEM.sh			generic script to start the master on the cluster of the respective system
  slave_SYSTEM.sh			generic script to start one slave on the cluster of the respective system
  startslaves_SYSTEM.sh		generic script to start all slaves on the cluster of the respective system
  
Usually, one only needs to modify the files run.sh, start.sh, and the start_SYSTEM.sh files for the desired systems.
The other files are generic and should not be modified.