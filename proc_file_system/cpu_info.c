#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void main(int argc, char* argv[])
{
	FILE *fd,*cpu_info,*osrelease,*meminfo,*uptime;
	char processor_type[30],buffer[5000],*match,kernel_version[20],memory_info[20];
	int length,i;
	double time;

	if(!(argc == 1 || argc == 3))
	{
		printf("ERROR : Invalid Arguments!\n");
		exit(0);
	}
	
	fd = fopen("OUTPUT.txt","w");

	if(fd == NULL)
	{
		printf("ERROR : Output file can not be opened.\n");
		exit(0);
	}

	if(argc == 1)
	{
		/*Processor type from /proc/cpuinfo */

			/* start */

		cpu_info = fopen("/proc/cpuinfo","r");

		

		length = fread(buffer,1,sizeof(buffer),cpu_info);

		fclose(cpu_info);
		

		if(length == 0 || length == sizeof(buffer))
		{
			printf("Length : %d\n",length);
			printf("ERROR : Processor type can not be read.\n");
		}


		buffer[length] = '\0';

		match = strstr(buffer,"model name");

		if(match == NULL)
		{
			printf("ERROR : Unable in finding the model name .\n");
			exit(0);
		}

		while(*match != ':')
		{
			match++;
		}

		match+=2;
		i=0;

		while(*match != '\n')
		{
			processor_type[i] = *match;
			match++;
			i++;
		}

		processor_type[i] = '\0';
		


		printf("Processor Type	: %s\n",processor_type);

		fprintf(fd,"Processor Type	: %s\n",processor_type);

				/* end */

				/* kernel version from /proc/sys/kernel/osrelease */

				/* start */

		osrelease = fopen("/proc/sys/kernel/osrelease","r");

		if(osrelease == NULL)
		{
			printf("ERROR : Unable to open osrelease file.\n");
			exit(0);
		}

		length = fread(kernel_version,1,sizeof(kernel_version),osrelease);
		
		
		if(length == 0 || length == sizeof(kernel_version))
		{
			printf("Length : %d\n",length);
			printf("ERROR : Unable to read the file osrelease.\n");
			exit(0);
		}

		

		kernel_version[length] = '\0';

		printf("Kernel Version	: %s",kernel_version);
		fprintf(fd,"Kernel Version	: %s",kernel_version);

				/* end */

			/* memory of the system from /proc/meminfo */

				/* start */
	
		meminfo = fopen("/proc/meminfo","r");

		if(meminfo == NULL)
		{
			printf("ERROR : Unable to open the meminfo file.\n");
			exit(0);
		}

		length = fread(buffer,1,sizeof(buffer),meminfo);

		fclose(meminfo);

		if(length == 0 || length == sizeof(buffer))
		{
			printf("Length : %d\n",length);
			printf("ERROR : Unable to read from meminfo file.\n");
			exit(0);
		}

		match = strstr(buffer , "MemTotal:");

		if(match == NULL)
		{
			printf("ERROR : Unable to read total memory from meminfo buffer.\n");
			exit(0);
		}
		
		sscanf(match , "MemTotal:%s",memory_info);

		printf("Total Memory	: %s kB\n",memory_info);
		fprintf(fd,"Total Memory	: %s kB\n",memory_info);
	

				/* end */

				/* Last boot time from /proc/uptime */
		
					/* start */

		uptime = fopen("/proc/uptime","r");

		if(uptime == NULL)
		{
			printf("ERROR : Unable to open uptime file.\n");
			exit(0);
		}

		fscanf(uptime,"%lf",&time);

		fclose(uptime);

		printf("Last Boot	: %lf Before\n",time);
		fprintf(fd,"Last Boot	: %lf Before",time);


	
			/* end */

		fclose(osrelease);
	}
	else
	{
		int read_rate, out_rate,length,space_count,count;
		long int user,nice,system,idle,iowait,irq,softirq,total_mem, free_mem,reads,writes,io_time,context_switch,prev_context_switch,processes,prev_processes;
		FILE *stat,*meminfo,*diskstats;
		double cpu_time;
		char buffer[2000],*match;

		read_rate = atoi(argv[1]);
		out_rate = atoi(argv[2]);


					/* To know user mode, system mode and idle mode of the processor. */

							/* start */

		stat = fopen("/proc/stat","r");

		if(stat == NULL)
		{
			printf("ERROR : Unable to open the stat file.\n");
			exit(0);
		}

		fscanf(stat,"cpu %ld %ld %ld %ld %ld %ld %ld ", &user,&nice,&system,&idle,&iowait,&irq,&softirq);

		fclose(stat);
		
		cpu_time = (double)(user + nice + system + idle + iowait + irq + softirq) ;

		printf("User Mode		: %f%%\n",(user/cpu_time)*100);
		printf("System Mode		: %f%%\n",(system/cpu_time)*100);
		printf("Idle Mode		: %f%%\n",(idle/cpu_time)*100);

						
							/* end */

					/* To know about the available free memoey */

							/* start */

		meminfo = fopen("/proc/meminfo","r");

		if(meminfo == NULL)
		{
			printf("ERROR : Unable to open the meminfo file.\n");
			exit(0);
		}

		fscanf(meminfo,"MemTotal: %ld kB\nMemFree: %ld",&total_mem,&free_mem);

		fclose(meminfo);

		printf("Free Memory		: %ld kB\n",free_mem);
		printf("Free Memory		: %f%%\n",(free_mem/(float)(total_mem))*100);

							/* end */

					/* To know the disk read/write rate */

							/* start */

		diskstats = fopen("/proc/diskstats","r");

		if(diskstats == NULL)
		{
			printf("ERROR : Unable to open diskstats file.\n");
			exit(0);
		}

		length = fread(buffer,1,sizeof(buffer),diskstats);

		fclose(diskstats);
		
		if(length == 0 || length == sizeof(buffer))
		{
			printf("Length : %d\n",length);
			printf("ERROR : Unable to read from diskstats file.\n");
			exit(0);
		}

		buffer[length] = '\0';

		match = strstr(buffer,"sda6");

		if(match == NULL)
		{
			printf("ERROR : Unable to read hard drive.\n ");
			exit(0);
		}
		
		sscanf(match ,"sda6 %ld",&reads);

		space_count = 5;

		while(space_count >0)
		{
			if(*match == ' ')
			{
				space_count--;
			}
			match++;

		}

		sscanf(match,"%ld",&writes);

		space_count = 5;

		while(space_count > 0)
		{
			if(*match == ' ')
			{
				space_count--;
			}
			
			match++;

		}

		sscanf(match ,"%ld",&io_time);

		printf("Read/Write Rate		: %ld	blocks per second\n",(int)(reads+writes)/(io_time/1000));

							/* end */

					/* context switch from /proc/stat */

							/* start */

		stat = fopen("/proc/stat","r");

		if(stat == NULL)
		{
			printf("ERROR	: Unable to open stat file.\n");
			exit(0);
		}

		length = fread(buffer,1,sizeof(buffer),stat);

		if(length == 0 || length == sizeof(buffer))
		{
			printf("Length	: %d\n",length);
			printf("ERROR	: Unable to read from stat file.\n ");
			exit(0);
		}

		match = strstr(buffer,"ctxt");

		if(match == NULL)
		{
			printf("ERROR	: Unable to read from stat buffer.\n");
			exit(0);
		}

		sscanf(match ,"ctxt %ld",&context_switch);

		match = strstr(buffer,"processes");

		if(match == NULL)
		{
			printf("ERROR	: Unable to read from stat buffer.\n");
			exit(0);
		}

		sscanf(match,"processes %ld",&processes);



		printf("Context Switch Rate		: %ld	per second\n",(long)(context_switch/(cpu_time/100)));
		printf("Processes Creation Rate		: %f	per second\n",(processes/(cpu_time/100)));

		prev_context_switch = context_switch;
		prev_processes = processes;
			
							/* end */


		while(1)
		{


			printf("*************************************************************************************************************\n");
			count = out_rate / read_rate ;

			while(count > 0 )
			{
					/* To know user mode, system mode and idle mode of the processor. */

							/* start */

				stat = fopen("/proc/stat","r");

				if(stat == NULL)
				{
					printf("ERROR : Unable to open the stat file.\n");
					exit(0);
				}

				fscanf(stat,"cpu %ld %ld %ld %ld %ld %ld %ld ", &user,&nice,&system,&idle,&iowait,&irq,&softirq);

				fclose(stat);
		
				cpu_time = (double)(user + nice + system + idle + iowait + irq + softirq) ;


						
							/* end */

					/* To know about the available free memoey */

							/* start */

				meminfo = fopen("/proc/meminfo","r");

				if(meminfo == NULL)
				{
					printf("ERROR : Unable to open the meminfo file.\n");
					exit(0);
				}

				fscanf(meminfo,"MemTotal: %ld kB\nMemFree: %ld",&total_mem,&free_mem);

				fclose(meminfo);


							/* end */
					
					/* To know the disk read/write rate */

							/* start */

				diskstats = fopen("/proc/diskstats","r");

				if(diskstats == NULL)
				{
					printf("ERROR : Unable to open diskstats file.\n");
					exit(0);
				}

				length = fread(buffer,1,sizeof(buffer),diskstats);

				fclose(diskstats);
		
				if(length == 0 || length == sizeof(buffer))
				{
					printf("Length : %d\n",length);
					printf("ERROR : Unable to read from diskstats file.\n");
					exit(0);
				}

				buffer[length] = '\0';

				match = strstr(buffer,"sda6");

				if(match == NULL)
				{
					printf("ERROR : Unable to read hard drive.\n ");
					exit(0);
				}
		
				sscanf(match ,"sda6 %ld",&reads);

				space_count = 5;
	
				while(space_count >0)
				{
					if(*match == ' ')
					{
						space_count--;
					}
					match++;

				}

				sscanf(match,"%ld",&writes);

				space_count = 5;

				while(space_count > 0)
				{
					if(*match == ' ')
					{
						space_count--;
					}
			
					match++;

				}

				sscanf(match ,"%ld",&io_time);


							/* end */
					
					/* context switch from /proc/stat */

							/* start */

				prev_context_switch = context_switch;
				prev_processes = processes;
				
				stat = fopen("/proc/stat","r");

				if(stat == NULL)
				{	
					printf("ERROR	: Unable to open stat file.\n");
					exit(0);
				}

				length = fread(buffer,1,sizeof(buffer),stat);

				if(length == 0 || length == sizeof(buffer))
				{
					printf("Length	: %d\n",length);
					printf("ERROR	: Unable to read from stat file.\n ");
					exit(0);
				}

				match = strstr(buffer,"ctxt");

				if(match == NULL)
				{
					printf("ERROR	: Unable to read from stat buffer.\n");
					exit(0);
				}

				sscanf(match ,"ctxt %ld",&context_switch);

				match = strstr(buffer,"processes");

				if(match == NULL)
				{
					printf("ERROR	: Unable to read from stat buffer.\n");
					exit(0);
				}

				sscanf(match,"processes %ld",&processes);

			
							/* end */
				sleep(read_rate);
								
				count--;
			
			}
				printf("User Mode		: %f%%\n",(user/cpu_time)*100);
				printf("System Mode		: %f%%\n",(system/cpu_time)*100);
				printf("Idle Mode		: %f%%\n",(idle/cpu_time)*100);
				
				printf("Free Memory		: %ld kB\n",free_mem);
				printf("Free Memory		: %f%%\n",(free_mem/(float)(total_mem))*100);
				
				printf("Read/Write Rate		: %ld	blocks per second\n",(int)(reads+writes)/(io_time/1000));

				printf("Context Switch Rate	: %d	per second\n",(int)((context_switch - prev_context_switch)/2));
				printf("Processes Creation Rate	: %f	per second\n",((processes - prev_processes)/2.0));


		}




	}
}
