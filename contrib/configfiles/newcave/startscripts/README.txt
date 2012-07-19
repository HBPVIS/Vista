General cave config files for starting the cluster mode.
Usually, you only need to adjust the start_newcave.sh and the kill_newcave.sh
to your applications name, as well as the appropriate ini files (as well as 
optional application-dependent paramters)
The other files are generic and utilize start_newcave.sh and kill_newcave.sh
and usually dont need to be modified
  master_newcave.sh      : start the master with a given config
  slave_newcave.sh       : start the slave with a given config
  startslaves_newcave.sh : start all slaves of a config on the cluster clients
  run_newcave.sh         : start all slaves, the master, and kill all after the app ends
