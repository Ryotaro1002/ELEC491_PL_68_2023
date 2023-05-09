% Channel ID and APIkey
channelID = 2026263;
writeAPIKey = 'WN6UZKY58ABMJW89';  
readAPIKey = 'AQRWX7EYH43YYRGD'; 

% Write a initial value(0) for the field that will used to record and display detection numbers
thingSpeakWrite(channelID,'Fields',5,'Values',0,'WriteKey',writeAPIKey);
