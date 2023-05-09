% Scrape a website to identify the current temperature in UBC
% The temperature is then written to the data field specified

% Specify the url containing information on current air temperature in UBC
url = 'http://weather.eos.ubc.ca/wxfcst/users/Guest/ubcrs_withicons/index.php?location=3510';

% Channel ID and APIkey
ChannelID = 2026263;
writeAPIKey = 'WN6UZKY58ABMJW89';

% Fetch data and parse it to find the air temperature
webText = webread(url);
tempF = urlfilter(webText, 'Temperature:');
display(tempF, 'Current Inside Surface Temperature in UBC');

% Write the temperature data to the data field specified
thingSpeakWrite(ChannelID,'Fields',3,'Values',tempF,'WriteKey', writeAPIKey);
