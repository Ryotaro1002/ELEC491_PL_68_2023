% Read recording of detection over the past hour from a ThingSpeak channel
% Convert the recording to an array containing the timestamp and collision number 
% Write the timestamp and collision number seperately to another ThingSpeak channel. 

% Define variables
size = 0;
data = 0;
rem_mm = 0;
time_hh = 0;
time_mm = 0;
time_ss = 0;
m = 0;

%tStamps = datetime(timeset);

% Channel ID and APIkey
channelID = 2026263;
writeAPIKey = 'WN6UZKY58ABMJW89';  
readAPIKey = 'AQRWX7EYH43YYRGD'; 

%Read collision data from Arduino
size0 = thingSpeakRead(channelID,'Fields',6,NumPoints=20,ReadKey = readAPIKey)
i = 20;
y=1;
size = size0(i);

while isnan(size)
     i = i-1;
     y=y+1;
     size = size0(i); 
     checkallNaN=1;
     if y == 20
        checkallNaN=0;
        break;
     end    
 end 
%%size = size0(1);

% TF = isnan(size0);
% size0(TF) = 0;
% checkallNaN = any(size0,'all')

if checkallNaN == 1
    
    data0 = thingSpeakRead(channelID,'Fields',6,NumPoints=size+y+1,ReadKey = readAPIKey)
    date_dd = data0(size+1)
    tStamps = data0(1:size);
    display(tStamps,'tStamps');  
    
    cNums = zeros(size,1);
    time_data = zeros(size,1);
    x = 0;
    
    %Conversion of date 
    t = today('datetime');
    c = yyyymmdd(t);
    date_yyyy = fix(c/10000);
    rem_date = fix(rem(c,10000));
    date_mm = fix(rem_date/100);
    %date_dd = fix(rem(rem_date,100));
    
    time_hh = fix(tStamps(1)/10000);
    rem_mm = fix(rem(tStamps(1),10000));
    time_mm = fix(rem_mm/100);
    time_ss = fix(rem(rem_mm,100));
    time_data = [datetime([date_yyyy,date_mm,date_dd,time_hh,time_mm,time_ss],'format','MM/dd/uuuu HH:mm:ss')]
    
    while  x < size  
    x = x+1;
    time_hh = fix(tStamps(x)/10000);
    rem_mm = fix(rem(tStamps(x),10000));
    time_mm = fix(rem_mm/100);
    time_ss = fix(rem(rem_mm,100));
    cNums(x,1) = x;
    time_data(x,1) = [datetime([date_yyyy,date_mm,date_dd,time_hh,time_mm,time_ss],'format','MM/dd/uuuu HH:mm:ss')];  
    end    
            
    thingSpeakWrite(channelID,'Fields',5,'Values',cNums,'TimeStamp',time_data,'WriteKey',writeAPIKey);
else
   m = m+1;
end   
