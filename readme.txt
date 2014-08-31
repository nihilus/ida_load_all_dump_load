                dump_all/load_all set of tools
                                        deroko of ARTeam
                                        
                                        
dump_all.exe is program which will dump all regions of a certain executable into
             specified folder. All dumps are stored as r00000000.dmp where 
             00000000 is virtual address of a paticilar memory region.
             Advice is to create always new folder for these dumped regions, as
             load_all will load all of these regions to IDA database. Just to keep
             everything organized, and to avoid loading of wrong files, which could
             occur under some cicumstances.

load_all.plw is and IDA plugin which will actually load all of these memory regions
             into IDA database. Example plugin is compiled with IDA 5.2 SDK, but you
             may compile it for other versions too.
             Plugin will prompt you for file, so you are free to select any of these
             .dmp, and plugin will load all of them into database. This could be useful
             when analyzing malware or some protection with many buffers, for better 
             analyze of a VM, or import protection. This will avoid need to dump regions
             manually.
                          
                                                        deroko of ARTeam 
                                                        
there are some planned updates for both tools, but due to lack of time, and being
really busy with my exams I was not able to put those ideas into this release.               