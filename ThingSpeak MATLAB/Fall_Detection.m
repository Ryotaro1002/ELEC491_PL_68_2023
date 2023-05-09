% Read the soil moisture channel data from the past two weeks.
% Send an email and tell the user to add water if the value
% is in the lowest 10 %.

% Store the channel ID for the moisture sensor channel.
channelID = 2026263;
readAPIKey = 'AQRWX7EYH43YYRGD'; 

% Provide the ThingSpeak alerts API key.  All alerts API keys start with TAK.
alertApiKey = 'TAKNEL7VNFw3MSbZ2p/';

% Set the address for the HTTTP call
alertUrl="https://api.thingspeak.com/alerts/send";

% webwrite uses weboptions to add required headers.  Alerts needs a ThingSpeak-Alerts-API-Key header.
options = weboptions("HeaderFields", ["ThingSpeak-Alerts-API-Key", alertApiKey ]);

% Set the email subject.
alertSubject = sprintf("Device has fallen down");

% Read the recent data.
%moistureData = thingSpeakRead(channelID,'NumDays',30,'Fields',7);
new_data = 0; 
Fall_Data = 0;
last_data = new_data
new_data = thingSpeakRead(channelID,'Fields',7,NumPoints=4,ReadKey = readAPIKey)

% Check to make sure the data was read correctly from the channel.
i = 0;
j = 0; 
counter = 0;

TF = isnan(new_data);
new_data(TF) = 0
checkallNaN = any(new_data,'all')
display(new_data,'new_data');


if checkallNaN == 1
    while i < 4
        i = i+1;
        if new_data(i) ~= 0
            j = j+1;
            if last_data ~= new_data(i) 
               Fall_Data(j) = new_data(i);
               last_data = Fall_Data(j); 
            end
            end
     end   
end


display((Fall_Data-last_data),'(Fall_Data-last_data)');
display(counter,'counter');

% Set the outgoing message
if ((Fall_Data-last_data) ~= 0)
    %alertBody = 'The device with the channelID'+ channelID + 'fell down!';
    alertBody = strcat("This email is to notify you that the bird collision detector sensor with the ChannelID ",num2str(channelID),' fell in the past 5 minutes. Please go to the channel page and to check which specific device fell. More instructions can be found in the user manual.');
else
    %alertBody = 'The device with the channelID'+ channelID + 'does not fall down! It works very well!';
    %alertBody = strcat('The device works well in the past 15 minutes! Its channelID is ', str_channelID);
    counter = counter+1;
end

 % Catch errors so the MATLAB code does not disable a TimeControl if it fails
try
    webwrite(alertUrl , "body", alertBody, "subject", alertSubject, options);
catch someException
    fprintf("Failed to send alert: %s\n", someException.message);
end
