# Project 4: Signaling with Multi-Process Programs:
By: Arun Agarwal

## Analysis/Results:

The following cases were tested (with log files created for those cases):
10 Second Time Run:
- logSendReceiveProc10Sec.txt
- logSendReceiveThread10Sec.txt
30 Second Time Run:
- logSendReceiveProc30Sec.txt
- logSendReceiveThread30Sec.txt
1 Minute Time Run:
- logSendReceiveProc1Min.txt
- logSendReceiveThread1Min.txt
1000 Signals Run:
- logSendReceiveProc1000Sig.txt
- logSendReceiveThread1000Sig.txt

The exact outputs of these files are seen in the txt files included in the submission of this assignment. Snippets are shown below:
For Processes:
For 10 Seconds:
![image](https://user-images.githubusercontent.com/55926421/164814060-349c45a4-b2f4-4cb1-87e9-0703fd51ecfc.png)
For 30 Seconds:
![image](https://user-images.githubusercontent.com/55926421/164814083-d2b890d6-31d6-4d86-8b3b-35e20c55d8a6.png)
For 1 Minute:
![image](https://user-images.githubusercontent.com/55926421/164814103-9510293e-ea54-4356-9979-e67bb4669583.png)
For 1000 Signals Received:
![image](https://user-images.githubusercontent.com/55926421/164814187-2b7fa400-bf29-4c87-9ef3-23800c2884e1.png)
For Threads:
For 10 Seconds:
![image](https://user-images.githubusercontent.com/55926421/164814263-34587ad3-6b82-4e10-97e1-83e9b940ac40.png)
For 30 Seconds:
![image](https://user-images.githubusercontent.com/55926421/164814275-3e8bcaaa-238f-4e7b-8eff-0eae7d917cd6.png)
For 1 Minute:
![image](https://user-images.githubusercontent.com/55926421/164814302-e776db90-ad0a-424c-9b36-9fca2c467dd6.png)
For 1000 Signals Received:
![image](https://user-images.githubusercontent.com/55926421/164814326-8e148bb3-1457-4a25-ba0f-ec8d1e11ed8b.png)

I was a bit confused if this is what I was expected to notice, but the overarching pattern is that, no matter what time limit I set, the type of results were the same. That is, I received the same type of results for running the thread and process programs for 10 secons, 30 seconds, and 1 minute. The most notable thing to point out is that the number of signals received is almost exactly double the number of signals sent. I believe that this is because we are generating one of two types of signals, and for each of these signals, we had created two signal receivers. I believe if I had constructed my program differently, I would have seen results that matched those differences. I have no way of checking whether one receiver had already received the generated signal to my knowledge, so I ended up with both signal receivers counting one signal. 

In terms of lost signals, our reporter function was updating the log for every 20 signals received, as was specified to be done by the instructions. We saw a loss of about 2 or 3 signals at the end of the log at process termination. I think there were also some signal loss throughout the running of the program (but I am not confident on this). I believe this is because the reporting process may be terminating while there are signals waiting to be written. This would make sense, especially when considering the abrupt way in which I end the running of the program (Control+C). It could also be because the reporting process gets terminated in the middle of counting to 20. 

I am not sure exactly what we wanted to interpret by looking at the average time between signals, but it is exactly what we expected for all runs tested. That is, the average time is always around .1 seconds, around the area that we specifically set it for. 

As pointed out earlier, I felt like I saw the same type of results whether I did a test comparing the number of signals (1000) or a certain time interval (10 seconds, 30 seconds, or 1 minute), so I don't think I have anything specific to report in this regard. 

One thing that the TA specified we should do is determine when the sending demand results in signal losses by looking at the logging time that the signals are sent and the receiving time. However, I wasn't able to understand exactly what I was supposed to look for/how to look for it, so I've kept my analysis to what is described above. 

## Errors/Issues/Discussion:
There are some issues that I would like to point out with my program. First, in terms of testing, I wanted to point out that I did not do the Excel analysis as described in the instructions partly due to running out of time, but also because I wasn't exactly sure how/why to do this. 

The other biggest issue or thing to point out is that I did not set up my program as intended. That is, we were supposed to have processes and threads existing concurrently, but because I did not know how to do this, I made separate programs, one that would use processes and another threads. I understand if I will lose points for this, but I was still able to accomplish the major goals for this assignment. That is, I have shown that I understand how to work with multiple processes, multiple threads, and signals. I also expanded upon my knowledge of various points learned throughout the course such as sychronization (through the creation of locks in some of my functions) and header files (great for organization).

Overall, I created the main process, the signal catching process, the threads, the signal masks, the signal catcher routines, the individual locks for shared structures, the description of testing and documentation, and a final executable. 
