% Read recording of detection data every 5 minutes from the data field specified from a ThingSpeak channel;
% Convert the received data to an array containing timestamps and collision numbers; 
% Write timestamps and collision numbers received seperately to another data field within one ThingSpeak channel. 

% Channel ID and APIkeys
channelID = 2026263;
writeAPIKey = 'WN6UZKY58ABMJW89';  
readAPIKey = 'AQRWX7EYH43YYRGD'; 

% Define variables
size = 0;
data = 0;
new_element = 0;
last_counter = 0;
isnew = 0;
rem_mm = 0;
time_hh = 0;
time_mm = 0;
time_ss = 0;
allNaN_num = 0;
checkallNaN = 0;
x = 0;
%cNums = zeros(size,1);
%time_data = zeros(size,1);
    
% Find the last collision number by reading data from the data field 5 to check whether the data is updated
last_size0 = thingSpeakRead(channelID,'Fields',5,NumPoints=10,ReadKey = readAPIKey)
j = 10;
k = 1;
last_counter = last_size0(j);

while isnan(last_counter)
     j = j-1;
     k = k+1;
     last_counter = last_size0(j);
     checklastNaN = 0;
     if k == 10
        checklastNaN = 1;
        break;
     end  
end 

% Read updated collision data from the Arduino
%size0 = thingSpeakRead(channelID,'Fields',1,NumPoints=4,ReadKey = readAPIKey,OutputFormat = 'TimeTable')
size0 = thingSpeakRead(channelID,'Fields',6,NumPoints=15,ReadKey = readAPIKey)
i = 15;
y = 1;
new_element = size0(i);

%Skip the 'NaN' data
while isnan(new_element)
     i = i-1;
     y = y+1;
     new_element = size0(i); 
     checkallNaN = 0;
     if y == 15
        checkallNaN = 1;
        break;
     end    
 end 

display(new_element,'updated data');
display(checkallNaN,'checkallNaN');

% Check whether the received collision data is updated and larger than the last collision numbers stored
if new_element > last_counter
    isnew = 1;
    size = new_element-last_counter;
else
    isnew = 0;
    size = 0;
end 

display(last_counter,'last_counter');
display(isnew,'isnew');
display(size,'difference_size');


if (checkallNaN == 0) && (isnew == 1)
    
    data0 = thingSpeakRead(channelID,'Fields',6,NumPoints=size+y,ReadKey = readAPIKey)
    if size == 1
        tStamps = data0(1);
        counter = new_element;
        display(tStamps,'tStamps');  
    else 
        tStamps = data0(1:size);
        counter = new_element;
        display(tStamps,'tStamps');
    end
    
    % Conversion of the date of today
    t = today('datetime');
    c = yyyymmdd(t);
    date_yyyy = fix(c/10000);
    rem_date = fix(rem(c,10000));
    date_mm = fix(rem_date/100);
    date_dd = fix(rem(rem_date,100));
    
    time_hh = fix(tStamps(1)/10000);
    rem_mm = fix(rem(tStamps(1),10000));
    time_mm = fix(rem_mm/100);
    time_ss = fix(rem(rem_mm,100));
    time_data = [datetime([date_yyyy,date_mm,date_dd,time_hh,time_mm,time_ss],'format','MM/dd/uuuu HH:mm:ss')]
    
    % Conversion of timestamps received 
    while size > 0
        time_hh = fix(tStamps(size)/10000);
        rem_mm = fix(rem(tStamps(size),10000));
        time_mm = fix(rem_mm/100);
        time_ss = fix(rem(rem_mm,100));
        cNums(size,1) = new_element-x;
        display(cNums,'cNums')
        time_data(size,1) = [datetime([date_yyyy,date_mm,date_dd,time_hh,time_mm,time_ss],'format','MM/dd/uuuu HH:mm:ss')];  
        x = x+1;
        size = size-1;
    end    
    
    % Write the updated collision numbers with its corresponding timestamp into the data field 5 to store and display
    thingSpeakWrite(channelID,'Fields',5,'Values',cNums,'TimeStamp',time_data,'WriteKey',writeAPIKey);
    
else
   allNaN_num = allNaN_num+1; % Increment if there is no data sent from the microcontroller
end   
